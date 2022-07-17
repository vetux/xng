/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "resource/parsers/jsonparser.hpp"

#include "async/threadpool.hpp"
#include "extern/json.hpp"

#include "asset/texture.hpp"
#include "asset/mesh.hpp"
#include "asset/material.hpp"

#include "resource/resourceimporter.hpp"

namespace xng {
    static TextureType convertTextureType(const std::string &v) {
        if (v == "texture2d")
            return TEXTURE_2D;
        else if (v == "cubemap")
            return TEXTURE_CUBE_MAP;
        throw std::runtime_error("Invalid texture type " + v);
    }

    static void loadBundle(const std::string &bundlePath,
                           ThreadPool &pool,
                           Archive &archive,
                           const ResourceImporter &importer,
                           std::mutex &bundleMutex,
                           std::map<std::string, std::shared_ptr<Task>> bundleTasks,
                           std::map<std::string, ResourceBundle> refBundles) {
        bundleMutex.lock();

        auto bundleIterator = refBundles.find(bundlePath);

        if (bundleIterator == refBundles.end()) {
            auto taskIterator = bundleTasks.find(bundlePath);
            if (taskIterator == bundleTasks.end()) {
                bundleTasks[bundlePath] = pool.addTask(
                        [&archive, &importer, &bundleMutex, &refBundles, &bundlePath, &pool]() {
                            std::filesystem::path path(bundlePath);

                            std::unique_ptr<std::istream> stream(archive.open(bundlePath));
                            auto bundle = importer.import(*stream, path.extension(), &archive);

                            std::lock_guard<std::mutex> guard(bundleMutex);
                            refBundles[bundlePath] = std::move(bundle);
                        });
            }
        }

        bundleMutex.unlock();
    }

    static ColorRGBA readColor(const nlohmann::json &j) {
        return ColorRGBA(j["r"], j["g"], j["b"], j["a"]);
    }

    static Texture readJsonTexture(std::istream &stream, Archive &archive) {
        std::string buffer((std::istreambuf_iterator<char>(stream)), std::istreambuf_iterator<char>());
        nlohmann::json j = nlohmann::json::parse(buffer);

        Texture texture;

        auto it = j.find("images");
        if (it != j.end()) {
            for (auto &element: *it) {
                texture.images.emplace_back(Uri(element));
            }
        }

        auto attr = j.find("attributes");
        if (attr != j.end()) {
            texture.textureDescription.textureType = convertTextureType(attr->value("textureType", "texture2d"));
            texture.textureDescription.generateMipmap = attr->value("generateMipmap",
                                                                    texture.textureDescription.textureType !=
                                                                    TEXTURE_CUBE_MAP);
            //TODO Parse texture attributes
        }

        return texture;
    }

    static ResourceBundle
    readJsonBundle(const std::string &buffer, const ResourceImporter &importer, Archive &archive, ThreadPool &pool) {
        nlohmann::json j = nlohmann::json::parse(buffer);

        std::mutex bundleMutex;

        std::map<std::string, std::shared_ptr<Task>> bundleTasks;
        std::map<std::string, ResourceBundle> referencedBundles; //The referenced json bundles by path

        //Begin loading of all referenced asset bundles on a thread pool
        auto iterator = j.find("meshes");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                loadBundle(Uri(element["uri"]).getFile(), pool, archive, importer, bundleMutex, bundleTasks,
                           referencedBundles);
            }
        }

        iterator = j.find("materials");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                auto it = element.find("uri");
                if (it != element.end()) {
                    loadBundle(Uri(*it).getFile(), pool, archive, importer, bundleMutex, bundleTasks,
                               referencedBundles);
                }
            }
        }

        iterator = j.find("images");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                loadBundle(Uri(element).getFile(), pool, archive, importer, bundleMutex, bundleTasks,
                           referencedBundles);
            }
        }

        //Wait for loading to finish
        for (auto &task: bundleTasks) {
            task.second->join();
        }

        ResourceBundle ret;

        //Load data from json and referenced bundles

        iterator = j.find("meshes");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];
                auto uri = Uri(element["uri"]);
                ret.add(name, std::make_unique<Mesh>(referencedBundles.at(uri.getFile()).get<Mesh>(uri.getAsset())));
            }
        }

        iterator = j.find("materials");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];
                auto it = element.find("uri");
                if (it != element.end()) {
                    auto uri = Uri(*it);
                    ret.add(name,
                            std::make_unique<Material>(
                                    referencedBundles.at(uri.getFile()).get<Material>(uri.getAsset())));
                } else {
                    Material mat;

                    if (element.find("diffuse") != element.end())
                        mat.diffuse = readColor(element["diffuse"]);
                    if (element.find("ambient") != element.end())
                        mat.ambient = readColor(element["ambient"]);
                    if (element.find("specular") != element.end())
                        mat.specular = readColor(element["specular"]);
                    if (element.find("emissive") != element.end())
                        mat.emissive = readColor(element["emissive"]);
                    if (element.find("shininess") != element.end())
                        mat.shininess = element["shininess"];

                    if (element.find("diffuseTexture") != element.end()) {
                        auto path = Uri(element["diffuseTexture"]);
                        mat.diffuseTexture = ResourceHandle<Texture>(path);
                    }

                    if (element.find("ambientTexture") != element.end()) {
                        auto path = Uri(element["ambientTexture"]);
                        mat.ambientTexture = ResourceHandle<Texture>(path);
                    }

                    if (element.find("specularTexture") != element.end()) {
                        auto path = Uri(element["specularTexture"]);
                        mat.specularTexture = ResourceHandle<Texture>(path);
                    }

                    if (element.find("emissiveTexture") != element.end()) {
                        auto path = Uri(element["emissiveTexture"]);
                        mat.emissiveTexture = ResourceHandle<Texture>(path);
                    }

                    if (element.find("shininessTexture") != element.end()) {
                        auto path = Uri(element["shininessTexture"]);
                        mat.shininessTexture = ResourceHandle<Texture>(path);
                    }

                    if (element.find("normalTexture") != element.end()) {
                        auto path = Uri(element["normalTexture"]);
                        mat.normalTexture = ResourceHandle<Texture>(path);
                    }

                    ret.add(name, std::make_unique<Material>(mat));
                }
            }
        }

        iterator = j.find("textures");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];
                auto s = std::stringstream(element.dump());
                auto tex = readJsonTexture(s, archive);
                ret.add(name, std::make_unique<Texture>(tex));
            }
        }

        iterator = j.find("images");
        if (iterator != j.end()) {
            for (auto &element: *iterator) {
                std::string name = element["name"];
                auto uri = Uri(element["uri"]);
                ret.add(name, std::make_unique<ImageRGBA>(
                        referencedBundles.at(uri.getFile()).get<ImageRGBA>(uri.getAsset())));
            }
        }

        return ret;
    }

    ResourceBundle JsonParser::read(const std::string &buffer,
                                    const std::string &hint,
                                    const ResourceImporter &importer,
                                    Archive *archive) const {
        if (archive == nullptr)
            throw std::runtime_error("Json parser invoked with nullptr archive");
        return readJsonBundle(buffer, importer, *archive, ThreadPool::getPool());
    }

    const std::set<std::string> &JsonParser::getSupportedFormats() const {
        static const std::set<std::string> formats = {"json", "bundle"};
        return formats;
    }
}