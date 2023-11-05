/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#ifndef XENGINE_SCENE_HPP
#define XENGINE_SCENE_HPP

#include <utility>

#include "xng/render/camera.hpp"

#include "xng/render/pointlight.hpp"

#include "xng/render/material.hpp"
#include "xng/render/skybox.hpp"
#include "xng/shader/shader.hpp"
#include "xng/render/mesh.hpp"
#include "xng/render/skinnedmesh.hpp"

#include "xng/util/genericmap.hpp"

#include "xng/resource/resourcehandle.hpp"

namespace xng {
    /**
     * The runtime scene render data.
     */
    struct XENGINE_EXPORT Scene {
        struct XENGINE_EXPORT Property {
            virtual std::type_index getType() = 0;

            template<typename T>
            T &get() {
                return dynamic_cast<T &>(*this);
            }

            template<typename T>
            const T &get() const {
                return dynamic_cast<T &>(*this);
            }
        };

        struct Node;

        struct XENGINE_EXPORT Node {
            template<typename T>
            T &getProperty() {
                return dynamic_cast< T &>(*properties.at(typeid(T)));
            }

            template<typename T>
            const T &getProperty() const {
                return dynamic_cast<const T &>(*properties.at(typeid(T)));
            }

            template<typename T>
            bool hasProperty() const {
                return properties.find(typeid(T)) != properties.end();
            }

            template<typename T>
            void addProperty(const T &property) {
                properties[typeid(T)] = std::make_shared<T>(property);
            }

            template<typename T>
            Node find() const {
                return findAll({typeid(T)}).at(0);
            }

            std::vector<Node> findAll(std::vector<std::type_index> propertyTypes) const {
                std::vector<Node> ret;
                for (auto &c: childNodes) {
                    auto nodes = c.findAll(propertyTypes);
                    ret.insert(ret.end(), nodes.begin(), nodes.end());
                }
                for (auto &prop: propertyTypes) {
                    if (properties.find(prop) != properties.end()) {
                        ret.emplace_back(*this);
                        break;
                    }
                }
                return ret;
            }

            std::vector<Node> childNodes;
            std::map<std::type_index, std::shared_ptr<Property>> properties;
        };

        struct TransformProperty : public Property {
            std::type_index getType() override {
                return typeid(ShadowProperty);
            }

            Transform transform;
        };

        struct ShadowProperty : public Property {
            std::type_index getType() override {
                return typeid(ShadowProperty);
            }

            bool castShadows = false;
            bool receiveShadows = false;
        };

        struct MeshProperty : public Property {
            std::type_index getType() override {
                return typeid(MeshProperty);
            }

            ResourceHandle<Mesh> mesh;
        };

        struct SkinnedMeshProperty : public Property {
            std::type_index getType() override {
                return typeid(SkinnedMeshProperty);
            }

            ResourceHandle<SkinnedMesh> mesh;
        };

        struct MaterialProperty : public Property {
            std::type_index getType() override {
                return typeid(MaterialProperty);
            }

            std::map<size_t, ResourceHandle<Material>> materials;
        };

        struct BoneTransformsProperty : public Property {
            std::type_index getType() override {
                return typeid(BoneTransformsProperty);
            }

            std::map<std::string, Mat4f> boneTransforms; // Optional dynamic bone transform values which override the values in SkinnedMesh.rig
        };

        struct PointLightProperty : public Property {
            std::type_index getType() override {
                return typeid(PointLightProperty);
            }

            PointLight light;
        };

        struct SkyboxProperty : public Property {
            std::type_index getType() override {
                return typeid(SkyboxProperty);
            }

            Skybox skybox;
        };

        struct CameraProperty : public Property {
            std::type_index getType() override {
                return typeid(CameraProperty);
            }

            Camera camera;
        };

        Node rootNode;
    };
}

#endif //XENGINE_SCENE_HPP
