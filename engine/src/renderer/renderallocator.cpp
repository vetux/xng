/**
 *   xEngine - C++ Game Engine Library
 *   Copyright (C) 2026 Julia Zampiccoli
 *
 *   This program is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU Lesser General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.
 *
 *   This program is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   Lesser General Public License for more details.
 *
 *   You should have received a copy of the Lesser General Public License
 *   along with this program.  If not, see <https://www.gnu.org/licenses/>.
 */

#include "xng/renderer/renderallocator.hpp"

namespace xng {
    RenderAllocator::RenderAllocator(rg::Heap &heap)
        : heap(heap), streams(heap, {1024, 1024}) {
    }

    RenderAllocator::~RenderAllocator() {
    }

    RenderScene RenderAllocator::createScene(rg::GraphBuilder &builder,
                                             const std::vector<RenderObjectHandle<RenderModel> > &models) {

    }

    void RenderAllocator::setCamera(const Camera &camera) {
    }

    RenderObjectHandle<RenderTexture> RenderAllocator::createTexture(const ImageRGBA &image) {
    }

    RenderObjectHandle<RenderMaterial> RenderAllocator::createMaterial(const ColorRGBA &albedo,
                                                                       float metallic,
                                                                       float roughness,
                                                                       float ambientOcclusion,
                                                                       Vec4f normalIntensity,
                                                                       RenderObjectHandle<RenderTexture> albedoTexture,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       metallicTexture,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       roughnessTexture,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       ambientOcclusionTexture,
                                                                       RenderObjectHandle<RenderTexture>
                                                                       normalTexture) {
    }

    RenderObjectHandle<RenderSkeleton> RenderAllocator::createSkeleton(const std::vector<std::string> &boneNames) {
    }

    RenderObjectHandle<RenderMesh> RenderAllocator::createMesh(const Mesh &mesh,
                                                               RenderObjectHandle<RenderSkeleton> skeleton) {
    }

    RenderObjectHandle<RenderModel> RenderAllocator::createModel(
        const std::vector<RenderObjectHandle<RenderMesh> > &meshes,
        RenderObjectHandle<RenderMaterial> material,
        bool receiveShadows) {
    }

    RenderObjectHandle<RenderPointLight> RenderAllocator::createPointLight() {
    }

    RenderObjectHandle<RenderSpotLight> RenderAllocator::createSpotLight() {
    }

    RenderObjectHandle<RenderDirectionalLight> RenderAllocator::createDirectionalLight() {
    }
}
