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

#ifndef XENGINE_ASSETRENDERMANAGER_HPP
#define XENGINE_ASSETRENDERMANAGER_HPP

#include <typeindex>

#include "assetmanager.hpp"
#include "asset/shader.hpp"
#include "asset/texture.hpp"

#include "graphics/renderallocator.hpp"

namespace xengine {
    /**
     * Handles allocation of render objects on the main thread.
     */
    class XENGINE_EXPORT AssetRenderManager {
    public:
        AssetRenderManager(AssetManager &assetManager, RenderAllocator &renderAllocator)
                : assetManager(assetManager), renderAllocator(renderAllocator) {}

        void incrementRef(const AssetPath &path) {
            objectRefCount[path]++;
        }

        template<typename T>
        void decrementRef(const AssetPath &path) {
            if (objectRefCount[path] < 1)
                throw std::runtime_error("Bundle reference counter underflow");
            auto ref = --objectRefCount[path];
            if (ref == 0) {
                unloadObject<T>(path);
                objects.erase(path);
                objectRefCount.erase(path);
            }
        }

        template<typename T>
        T &get(const AssetPath &path) {
            if (objects.find(path) == objects.end()) {
                loadObject<T>(path);
            }
            return dynamic_cast<T &>(*objects.at(path));
        }

    private:
        template<typename T>
        void loadObject(const AssetPath &path) {
            std::type_index tid = typeid(T);

            assetManager.incrementRef(path);

            if (tid == typeid(MeshBuffer)) {
                objects[path] = renderAllocator.createMeshBuffer(assetManager.getAsset<Mesh>(path));
            } else if (tid == typeid(ShaderProgram)) {
                auto shader = assetManager.getAsset<Shader>(path);
                if (shader.geometryShader.isEmpty())
                    objects[path] = renderAllocator.createShaderProgram(shader.vertexShader, shader.fragmentShader);
                else
                    objects[path] = renderAllocator.createShaderProgram(shader.vertexShader,
                                                                        shader.geometryShader,
                                                                        shader.fragmentShader);
            } else if (tid == typeid(TextureBuffer)) {
                auto texture = assetManager.getAsset<Texture>(path);
                auto texbuf = renderAllocator.createTextureBuffer(texture.attributes);

                for (auto &img: texture.images)
                    assetManager.incrementRef(img);

                if (texture.attributes.textureType == TextureBuffer::TEXTURE_CUBE_MAP) {
                    for (int i = TextureBuffer::POSITIVE_X; i <= TextureBuffer::NEGATIVE_Z; i++) {
                        texbuf->upload(static_cast<TextureBuffer::CubeMapFace>(i),
                                       assetManager.getAsset<Image<ColorRGBA>>(texture.images.at(i)));
                    }
                } else {
                    texbuf->upload(assetManager.getAsset<Image<ColorRGBA>>(texture.images.at(0)));
                }

                objects[path] = std::move(texbuf);
            } else {
                assetManager.decrementRef(path);
                throw std::runtime_error("Invalid object type");
            }
        }

        template<typename T>
        void unloadObject(const AssetPath &path) {
            std::type_index tid = typeid(T);

            //Check if get() was called for the given path.
            if (objects.find(path) != objects.end()) {
                if (tid == typeid(TextureBuffer)) {
                    auto texture = assetManager.getAsset<Texture>(path);
                    for (auto &img: texture.images)
                        assetManager.decrementRef(img);
                }
                assetManager.decrementRef(path);
            }
        }

        AssetManager &assetManager;
        RenderAllocator &renderAllocator;

        std::map<AssetPath, std::unique_ptr<RenderObject>> objects;
        std::map<AssetPath, uint> objectRefCount;
    };
}

#endif //XENGINE_ASSETRENDERMANAGER_HPP
