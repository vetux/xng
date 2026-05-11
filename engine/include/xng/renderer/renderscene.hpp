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

#ifndef XENGINE_RENDERSCENE_HPP
#define XENGINE_RENDERSCENE_HPP

#include "xng/rendergraph/heap.hpp"

#include "xng/renderer/vertexattribute.hpp"
#include "xng/renderer/objects/rendermodel.hpp"
#include "xng/renderer/objects/renderpointlight.hpp"
#include "xng/renderer/objects/renderspotlight.hpp"
#include "xng/renderer/objects/renderdirectionallight.hpp"

namespace xng {
    /**
     * The scene is the product of the render allocator packing / allocation strategy.
     *
     * It constrains the basic allocation model for the scene data while allowing the allocator to perform packing.
     */
    struct RenderScene {
        struct Mesh {
            rg::Primitive primitive;
            rg::DrawCall drawCall;

            bool indexed;
            int baseIndex; // The offset applied to each index.
            size_t indexCount; // The number of indices for this mesh

            int baseBone; // The offset applied to each bone index
            size_t boneCount; // The number of bones for this mesh

            Mesh(const rg::Primitive primitive,
                 const rg::DrawCall &drawCall,
                 const bool indexed,
                 const int baseIndex,
                 const size_t indexCount,
                 const int baseBone,
                 const size_t boneCount)
                : primitive(primitive),
                  drawCall(drawCall),
                  indexed(indexed),
                  baseIndex(baseIndex),
                  indexCount(indexCount),
                  baseBone(baseBone),
                  boneCount(boneCount) {
            }
        };

        struct Model {
            std::vector<Mesh> meshes;

            unsigned int transformIndex; // Index into transformBuffer
            unsigned int materialIndex; // Index into materialBuffer

            bool receiveShadows;

            std::unordered_map<TextureResolution, std::vector<size_t> > materialTextureIndices;

            Model(std::vector<Mesh> meshes,
                  const size_t transformIndex,
                  const size_t materialIndex,
                  const bool receiveShadows,
                  std::unordered_map<TextureResolution, std::vector<size_t> > _materialTextureIndices)
                : meshes(std::move(meshes)),
                  transformIndex(transformIndex),
                  materialIndex(materialIndex),
                  receiveShadows(receiveShadows),
                  materialTextureIndices(std::move(_materialTextureIndices)) {
            }
        };

        RenderScene(std::vector<Model> _models,
                    rg::HeapResource<rg::Buffer> _cameraBuffer,
                    rg::HeapResource<rg::Buffer> _transformBuffer,
                    rg::HeapResource<rg::Buffer> _boneBuffer,
                    rg::HeapResource<rg::Buffer> _materialBuffer,
                    rg::HeapResource<rg::Buffer> _pointLightBuffer,
                    rg::HeapResource<rg::Buffer> _spotLightBuffer,
                    rg::HeapResource<rg::Buffer> _directionalLightBuffer,
                    rg::HeapResource<rg::Texture> _pointShadowMaps,
                    rg::HeapResource<rg::Texture> _spotShadowMaps,
                    rg::HeapResource<rg::Texture> _directionalShadowMaps,
                    std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > _vertexBuffers,
                    rg::HeapResource<rg::Buffer> _indexBuffer,
                    std::unordered_map<TextureResolution, rg::HeapResource<rg::Texture> > _textures)
            : models(std::move(_models)),
              cameraBuffer(std::move(_cameraBuffer)),
              transformBuffer(std::move(_transformBuffer)),
              boneBuffer(std::move(_boneBuffer)),
              materialBuffer(std::move(_materialBuffer)),
              pointLightBuffer(std::move(_pointLightBuffer)),
              spotLightBuffer(std::move(_spotLightBuffer)),
              directionalLightBuffer(std::move(_directionalLightBuffer)),
              pointShadowMaps(std::move(_pointShadowMaps)),
              spotShadowMaps(std::move(_spotShadowMaps)),
              directionalShadowMaps(std::move(_directionalShadowMaps)),
              vertexBuffers(std::move(_vertexBuffers)),
              indexBuffer(std::move(_indexBuffer)),
              textures(std::move(_textures)) {
        }

        std::vector<Model> models;

        rg::HeapResource<rg::Buffer> cameraBuffer;
        rg::HeapResource<rg::Buffer> transformBuffer;
        rg::HeapResource<rg::Buffer> boneBuffer;
        rg::HeapResource<rg::Buffer> materialBuffer;

        rg::HeapResource<rg::Buffer> pointLightBuffer;
        rg::HeapResource<rg::Buffer> spotLightBuffer;
        rg::HeapResource<rg::Buffer> directionalLightBuffer;

        rg::HeapResource<rg::Texture> pointShadowMaps;
        rg::HeapResource<rg::Texture> spotShadowMaps;
        rg::HeapResource<rg::Texture> directionalShadowMaps;

        std::unordered_map<VertexAttribute, rg::HeapResource<rg::Buffer> > vertexBuffers;
        rg::HeapResource<rg::Buffer> indexBuffer;

        std::unordered_map<TextureResolution, rg::HeapResource<rg::Texture> > textures;
    };
}

#endif //XENGINE_RENDERSCENE_HPP
