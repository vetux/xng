/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 3 of the License, or (at your option) any later version.

 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public License
 *  along with this program; if not, write to the Free Software Foundation,
 *  Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
 */

#ifndef XENGINE_PROPERTY_HPP
#define XENGINE_PROPERTY_HPP

#include <typeindex>

#include "xng/render/pointlight.hpp"
#include "xng/render/directionallight.hpp"
#include "xng/render/spotlight.hpp"

namespace xng {
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

    struct TransformProperty : public Property {
        std::type_index getType() override {
            return typeid(TransformProperty);
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

    struct DirectionalLightProperty : public Property {
        std::type_index getType() override {
            return typeid(DirectionalLightProperty);
        }

        DirectionalLight light;
    };

    struct SpotLightProperty : public Property {
        std::type_index getType() override {
            return typeid(SpotLightProperty);
        }

        SpotLight light;
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

    struct WireframeProperty : public Property {
        std::type_index getType() override {
            return typeid(CameraProperty);
        }

        ColorRGBA wireColor;
    };
}

#endif //XENGINE_PROPERTY_HPP
