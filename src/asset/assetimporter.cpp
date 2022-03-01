/**
 *  xEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "asset/assetimporter.hpp"

#include <filesystem>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <sndfile.h>

#include "extern/stb_image.h"
#include "extern/json.hpp"

#include "async/threadpool.hpp"
#include "asset/mesh.hpp"

#include "audio/audioformat.hpp"

namespace xengine {
    static ColorRGBA convertJsonColor(const nlohmann::json &j) {
        return ColorRGBA(j["r"], j["g"], j["b"], j["a"]);
    }

    static TextureBuffer::TextureType convertJsonTextureType(const std::string &v) {
        if (v == "texture2d")
            return TextureBuffer::TEXTURE_2D;
        else if (v == "cubemap")
            return TextureBuffer::TEXTURE_CUBE_MAP;
        throw std::runtime_error("Invalid texture type " + v);
    }

    static AssetBundle readJsonBundle(const std::string &path, Archive &archive, ThreadPool &pool);

    static void sideLoadBundle(const std::string &bundlePath,
                               ThreadPool &pool,
                               Archive &archive,
                               std::mutex &bundleMutex,
                               std::map<std::string, std::shared_ptr<Task>> bundleTasks,
                               std::map<std::string, AssetBundle> refBundles) {
        bundleMutex.lock();

        auto bundleIterator = refBundles.find(bundlePath);

        if (bundleIterator == refBundles.end()) {
            auto taskIterator = bundleTasks.find(bundlePath);
            if (taskIterator == bundleTasks.end()) {
                bundleTasks[bundlePath] = pool.addTask(
                        [&archive, &bundleMutex, &refBundles, &bundlePath, &pool]() {
                            std::filesystem::path path(bundlePath);

                            std::unique_ptr<std::istream> stream(archive.open(bundlePath));
                            auto bundle = AssetImporter::import(*stream, path.extension(), &archive);

                            std::lock_guard<std::mutex> guard(bundleMutex);
                            refBundles[bundlePath] = std::move(bundle);
                        });
            }
        }

        bundleMutex.unlock();
    }

    static ImageRGBA readImage(const std::string &buffer) {
        int width, height, nrChannels;
        stbi_uc *data = stbi_load_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                              buffer.size(),
                                              &width,
                                              &height,
                                              &nrChannels,
                                              4);
        if (data) {
            auto ret = ImageRGBA(width, height);
            std::memcpy(ret.getData(), data, (width * height) * (sizeof(stbi_uc) * 4));
            stbi_image_free(data);
            return ret;
        } else {
            stbi_image_free(data);
            std::string error = "Failed to load image";
            throw std::runtime_error(error);
        }
    }

    static Texture readJsonTexture(std::istream &stream, Archive &archive) {
        std::string buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        nlohmann::json j = nlohmann::json::parse(buffer);

        Texture texture;

        auto it = j.find("images");
        if (it != j.end()) {
            for (auto &element: *it) {
                texture.images.emplace_back((AssetPath) {element["bundle"], element["asset"]});
            }
        }

        auto attr = j.find("attributes");
        if (attr != j.end()) {
            texture.attributes.textureType = convertJsonTextureType(attr->value("textureType", "texture2d"));
            texture.attributes.generateMipmap = attr->value("generateMipmap", texture.attributes.textureType !=
                                                                              TextureBuffer::TEXTURE_CUBE_MAP);
            //TODO Parse texture attributes
        }

        return texture;
    }

    static AssetBundle readJsonBundle(std::istream &stream, Archive &archive, ThreadPool &pool) {
        std::string buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        nlohmann::json j = nlohmann::json::parse(buffer);

        std::mutex bundleMutex;

        std::map<std::string, std::shared_ptr<Task>> bundleTasks;
        std::map<std::string, AssetBundle> refBundles; //The referenced asset bundles by path

        //Begin sideload of all referenced asset bundles
        auto iterator = j.find("meshes");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                sideLoadBundle(element["bundle"], pool, archive, bundleMutex, bundleTasks, refBundles);
            }
        }

        iterator = j.find("images");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                sideLoadBundle(element["bundle"], pool, archive, bundleMutex, bundleTasks, refBundles);
            }
        }

        //Wait for sideload to finish
        for (auto &task: bundleTasks) {
            task.second->wait();
        }

        AssetBundle ret;

        //Load data from json and referenced bundles
        iterator = j.find("meshes");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];
                std::string bundle = element["bundle"];
                std::string asset = element["asset"];

                ret.add<Mesh>(name, std::make_unique<Mesh>(refBundles.at(bundle).get<Mesh>(asset)));
            }
        }

        iterator = j.find("materials");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];

                auto it = element.find("bundle");
                if (it != element.end()) {
                    std::string n = element.value("asset", "");
                    ret.add<AssetMaterial>(name,
                                           std::make_unique<AssetMaterial>(refBundles.at(*it).get<AssetMaterial>(n)));
                } else {
                    AssetMaterial mat;

                    if (element.find("diffuse") != element.end())
                        mat.diffuse = convertJsonColor(element["diffuse"]);
                    if (element.find("ambient") != element.end())
                        mat.ambient = convertJsonColor(element["ambient"]);
                    if (element.find("specular") != element.end())
                        mat.specular = convertJsonColor(element["specular"]);
                    if (element.find("emissive") != element.end())
                        mat.emissive = convertJsonColor(element["emissive"]);
                    if (element.find("shininess") != element.end())
                        mat.shininess = element["shininess"];

                    if (element.find("diffuseTexture") != element.end()) {
                        auto path = AssetPath(element["diffuseTexture"]["bundle"], element["diffuseTexture"]["asset"]);
                        mat.diffuseTexture = path;
                    }

                    if (element.find("ambientTexture") != element.end()) {
                        auto path = AssetPath(element["ambientTexture"]["bundle"], element["ambientTexture"]["asset"]);
                        mat.ambientTexture = path;
                    }

                    if (element.find("specularTexture") != element.end()) {
                        auto path = AssetPath(element["specularTexture"]["bundle"],
                                              element["specularTexture"]["asset"]);
                        mat.specularTexture = path;
                    }

                    if (element.find("emissiveTexture") != element.end()) {
                        auto path = AssetPath(element["emissiveTexture"]["bundle"],
                                              element["emissiveTexture"]["asset"]);
                        mat.emissiveTexture = path;
                    }

                    if (element.find("shininessTexture") != element.end()) {
                        auto path = AssetPath(element["shininessTexture"]["bundle"],
                                              element["shininessTexture"]["asset"]);
                        mat.shininessTexture = path;
                    }

                    if (element.find("normalTexture") != element.end()) {
                        auto path = AssetPath(element["normalTexture"]["bundle"], element["normalTexture"]["asset"]);
                        mat.normalTexture = path;
                    }

                    ret.add<AssetMaterial>(name, std::make_unique<AssetMaterial>(mat));
                }
            }
        }

        iterator = j.find("textures");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];
                auto s = std::stringstream(element.dump());
                auto tex = readJsonTexture(s, archive);
                ret.add<Texture>(name, std::make_unique<Texture>(tex));
            }
        }

        iterator = j.find("images");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];
                std::string bundle = element["bundle"];
                std::string asset = element.value("asset", "");

                ret.add<ImageRGBA>(name, std::make_unique<ImageRGBA>(
                        refBundles.at(bundle).get<ImageRGBA>(asset)));
            }
        }

        return ret;
    }

    static Mesh convertMesh(const aiMesh &assMesh) {
        Mesh ret;
        ret.primitive = Mesh::TRI;
        ret.indexed = true;
        for (int y = 0; y < assMesh.mNumFaces; y++) {
            const auto &face = dynamic_cast<const aiFace &>(assMesh.mFaces[y]);
            if (face.mNumIndices != 3)
                throw std::runtime_error("Mesh triangulation failed");
            for (int z = 0; z < face.mNumIndices; z++) {
                ret.indices.emplace_back(face.mIndices[z]);
            }
        }

        for (int y = 0; y < assMesh.mNumVertices; y++) {
            const auto &p = dynamic_cast<const aiVector3D &>(assMesh.mVertices[y]);

            Vec3f pos{p.x, p.y, p.z};
            Vec3f norm{};
            Vec2f uv{};
            Vec3f tangent{};
            Vec3f bitangent{};

            if (assMesh.mNormals != nullptr) {
                const auto &n = dynamic_cast<const aiVector3D &>(assMesh.mNormals[y]);
                norm = {n.x, n.y, n.z};
                const auto &t = dynamic_cast<const aiVector3D &>(assMesh.mTangents[y]);
                tangent = {t.x, t.y, t.z};
                const auto &bt = dynamic_cast<const aiVector3D &>(assMesh.mBitangents[y]);
                bitangent = {bt.x, bt.y, bt.z};
            }

            if (assMesh.mTextureCoords[0] != nullptr) {
                const auto &t = dynamic_cast<const aiVector3D &>(assMesh.mTextureCoords[0][y]);
                uv = {t.x, t.y};
            }

            ret.vertices.emplace_back(Vertex(pos, norm, uv, tangent, bitangent));
        }

        return ret;
    }

    static AssetMaterial convertMaterial(const aiMaterial &assMaterial) {
        AssetMaterial ret;

        aiColor3D c;
        assMaterial.Get(AI_MATKEY_COLOR_DIFFUSE, c);
        ret.diffuse = {static_cast<uint8_t>(255 * c.r),
                       static_cast<uint8_t>(255 * c.g),
                       static_cast<uint8_t>(255 * c.b),
                       255};

        assMaterial.Get(AI_MATKEY_COLOR_AMBIENT, c);
        ret.ambient = {static_cast<uint8_t>(255 * c.r),
                       static_cast<uint8_t>(255 * c.g),
                       static_cast<uint8_t>(255 * c.b),
                       255};

        assMaterial.Get(AI_MATKEY_COLOR_SPECULAR, c);
        ret.specular = {static_cast<uint8_t>(255 * c.r),
                        static_cast<uint8_t>(255 * c.g),
                        static_cast<uint8_t>(255 * c.b),
                        255};

        assMaterial.Get(AI_MATKEY_SHININESS, ret.shininess);

        return ret;
    }

    static AssetBundle readAsset(const std::string &assetBuffer, const std::string &hint, Archive *archive) {
        //TODO: Implement assimp IOSystem pointing to archive

        Assimp::Importer importer;

        const auto *scenePointer = importer.ReadFileFromMemory(assetBuffer.data(),
                                                               assetBuffer.size(),
                                                               aiPostProcessSteps::aiProcess_Triangulate |
                                                               aiProcess_CalcTangentSpace | aiProcess_FlipUVs,
                                                               hint.c_str());
        if (scenePointer == nullptr)
            throw std::runtime_error("Failed to read mesh data from memory");

        const auto &scene = dynamic_cast<const aiScene &>(*scenePointer);

        AssetBundle ret;

        for (auto i = 0; i < scene.mNumMeshes; i++) {
            const auto &mesh = dynamic_cast<const aiMesh &>(*scene.mMeshes[i]);
            std::string name = mesh.mName.C_Str();
            ret.add<Mesh>(name, std::make_unique<Mesh>(convertMesh(mesh)));
        }

        for (auto i = 0; i < scene.mNumMaterials; i++) {
            auto material = convertMaterial(dynamic_cast<const aiMaterial &>(*scene.mMaterials[i]));

            aiString materialName;
            scene.mMaterials[i]->Get(AI_MATKEY_NAME, materialName);

            ret.add<AssetMaterial>(materialName.data, std::make_unique<AssetMaterial>(material));
        }

        return ret;
    }

    struct LibSndBuffer {
        std::string data;
        size_t pos;
    };

    sf_count_t sf_vio_get_filelen(void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        return buffer->data.size();
    }

    sf_count_t sf_vio_seek(sf_count_t offset, int whence, void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        switch (whence) {
            case SF_SEEK_SET:
                buffer->pos = offset;
                break;
            case SF_SEEK_CUR:
                buffer->pos += offset;
                break;
            case SF_SEEK_END:
                buffer->pos = buffer->data.size() - offset;
                break;
            default:
                throw std::runtime_error("Invalid whence");
        }
        return buffer->pos;
    }

    sf_count_t sf_vio_read(void *ptr, sf_count_t count, void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        sf_count_t ret;
        for (ret = 0; ret < count && buffer->pos + ret < buffer->data.size(); ret++) {
            static_cast<char *>(ptr)[ret] = buffer->data.at(buffer->pos + ret);
        }
        buffer->pos += ret;
        return ret;
    }

    sf_count_t sf_vio_write(const void *ptr, sf_count_t count, void *user_data) {
        throw std::runtime_error("VIO Write");
    }

    sf_count_t sf_vio_tell(void *user_data) {
        auto *buffer = reinterpret_cast<LibSndBuffer *>(user_data);
        return buffer->pos;
    }

    static Audio readAudio(const std::string &buf) {
        SF_VIRTUAL_IO virtio;
        virtio.get_filelen = &sf_vio_get_filelen;
        virtio.seek = &sf_vio_seek;
        virtio.read = &sf_vio_read;
        virtio.write = &sf_vio_write;
        virtio.tell = &sf_vio_tell;

        LibSndBuffer buffer{buf, 0};
        SF_INFO sfinfo;
        SNDFILE *sndfile = sf_open_virtual(&virtio, SFM_READ, &sfinfo, &buffer);
        if (!sndfile) {
            throw std::runtime_error("Failed to open audio buffer");
        }

        if (sfinfo.frames<1
                          || sfinfo.frames>(sf_count_t)(std::numeric_limits<int>::max() / sizeof(short)) /
            sfinfo.channels) {
            sf_close(sndfile);
            throw std::runtime_error("Bad sample count in audio buffer");
        }

        Audio ret;

        if (sfinfo.channels == 1) {
            ret.format = MONO16;
        } else if (sfinfo.channels == 2) {
            ret.format = STEREO16;
        } else if (sfinfo.channels == 3) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                ret.format = BFORMAT2D_16;
            } else {
                sf_close(sndfile);
                throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
            }
        } else if (sfinfo.channels == 4) {
            if (sf_command(sndfile, SFC_WAVEX_GET_AMBISONIC, NULL, 0) == SF_AMBISONIC_B_FORMAT) {
                ret.format = BFORMAT3D_16;
            } else {
                sf_close(sndfile);
                throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
            }
        } else {
            sf_close(sndfile);
            throw std::runtime_error("Unsupported channel count: " + std::to_string(sfinfo.channels));
        }

        ret.frequency = sfinfo.samplerate;

        auto *buff = static_cast<short *>(malloc((size_t) (sfinfo.frames * sfinfo.channels) * sizeof(short)));

        sf_count_t num_frames = sf_readf_short(sndfile, buff, sfinfo.frames);
        if (num_frames < 1) {
            free(buff);
            sf_close(sndfile);
            throw std::runtime_error("Failed to read samples from audio data");
        }

        sf_count_t num_bytes = num_frames * sfinfo.channels * sizeof(short);

        ret.buffer.resize(num_bytes);

        std::memcpy(ret.buffer.data(), buff, num_bytes);

        free(buff);

        sf_close(sndfile);

        return ret;
    }

    AssetBundle AssetImporter::import(std::istream &stream, const std::string &hint, Archive *archive) {
        if (hint.empty()) {
            std::string buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

            try {
                //Try to read source as image
                int x, y, n;
                if (stbi_info_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                          buffer.size(),
                                          &x,
                                          &y,
                                          &n) == 1) {
                    //Source is image
                    AssetBundle ret;
                    ret.add<ImageRGBA>("0", std::make_unique<ImageRGBA>(readImage(buffer)));
                    return ret;
                }
            } catch (const std::exception &e) {}

            //Try to read source as json
            try {
                if (archive == nullptr)
                    throw std::runtime_error("Null archive while parsing json");

                return readJsonBundle(stream, *archive, ThreadPool::getPool());
            } catch (const std::exception &e) {}

            //Try to read source as asset
            try {
                return readAsset(buffer, hint, archive);
            } catch (const std::exception &e) {}

            //Try to read source as audio
            auto audio = readAudio(buffer);
            AssetBundle ret;
            ret.add<Audio>("0", std::make_unique<Audio>(audio));

            return ret;
        } else {
            if (hint == ".json") {
                //Try to read source as json
                return readJsonBundle(stream, *archive, ThreadPool::getPool());
            } else {
                Assimp::Importer importer;
                if (importer.IsExtensionSupported(hint)) {
                    //Try to read source as asset
                    std::string buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
                    return readAsset(buffer, hint, archive);
                } else {
                    std::string buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());

                    try {
                        //Try to read source as image

                        int x, y, n;
                        if (stbi_info_from_memory(reinterpret_cast<const stbi_uc *>(buffer.data()),
                                                  buffer.size(),
                                                  &x,
                                                  &y,
                                                  &n) == 1) {
                            AssetBundle ret;
                            ret.add<ImageRGBA>("0", std::make_unique<ImageRGBA>(readImage(buffer)));
                            return ret;
                        }
                    } catch (const std::exception &e) {}

                    //Try to read source as audio
                    auto audio = readAudio(buffer);
                    AssetBundle ret;
                    ret.add<Audio>("0", std::make_unique<Audio>(audio));
                    return ret;
                }
            }
        }
    }

    AssetBundle AssetImporter::import(const std::string &path, Archive &archive) {
        auto stream = archive.open(path);
        return import(*stream, std::filesystem::path(path).extension(), &archive);
    }
}