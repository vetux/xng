/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2025 Julian Zampiccoli
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

#ifndef XENGINE_MESHBUFFER2D_HPP
#define XENGINE_MESHBUFFER2D_HPP

#include <unordered_set>

#include "xng/graphics/canvas.hpp"
#include "xng/graphics/vertexbuilder.hpp"

#include "xng/rendergraph/renderprimitive.hpp"
#include "xng/rendergraph/drawcall.hpp"
#include "xng/rendergraph/rendergraphcontext.hpp"

namespace xng {
    /**
     * Allocates 2D meshes from 2d draw commands into a single vertex / index buffer
     */
    class MeshBuffer2D {
    public:
        // TODO: Scale a unit quad instead of separate vertex / index allocations for 2D planes / Squares

        struct MeshDrawData {
            RenderPrimitive primitive{};
            DrawCall drawCall{};
            size_t baseVertex{};

            MeshDrawData() = default;

            MeshDrawData(const RenderPrimitive primitive,
                         const DrawCall &drawCall,
                         const size_t baseVertex) : primitive(primitive),
                                                    drawCall(drawCall),
                                                    baseVertex(baseVertex) {
            }
        };

        void update(const std::vector<Canvas> &canvases) {
            std::unordered_set<Vec2f> usedPlanes;
            std::unordered_set<Vec2f> usedSquares;
            std::unordered_set<std::pair<Vec2f, Vec2f>, LinePairHash> usedLines;
            std::unordered_set<Vec2f> usedPoints;
            for (auto &canvas: canvases) {
                for (auto &paintCommand: canvas.getPaintCommands()) {
                    switch (paintCommand.type) {
                        case Paint::PAINT_POINT: {
                            auto &point = std::get<PaintPoint>(paintCommand.data);
                            if (pointMeshes.find(point.position) == pointMeshes.end()) {
                                pointMeshes[point.position] = createPoint(point.position);
                            }
                            usedPoints.insert(point.position);
                            break;
                        }
                        case Paint::PAINT_LINE: {
                            auto &line = std::get<PaintLine>(paintCommand.data);
                            if (lineMeshes.find({line.start, line.end}) == lineMeshes.end()) {
                                lineMeshes[{line.start, line.end}] = createLine(line.start, line.end);
                            }
                            usedLines.insert({line.start, line.end});
                            break;
                        }
                        case Paint::PAINT_RECTANGLE: {
                            auto &square = std::get<PaintRectangle>(paintCommand.data);
                            if (square.fill) {
                                if (planeMeshes.find(square.dstRect.dimensions) == planeMeshes.end()) {
                                    planeMeshes[square.dstRect.dimensions] = createPlane(square.dstRect.dimensions);
                                }
                                usedPlanes.insert(square.dstRect.dimensions);
                            } else {
                                if (squareMeshes.find(square.dstRect.dimensions) == squareMeshes.end()) {
                                    squareMeshes[square.dstRect.dimensions] = createSquare(square.dstRect.dimensions);
                                }
                                usedSquares.insert(square.dstRect.dimensions);
                            }
                            break;
                        }
                        case Paint::PAINT_IMAGE: {
                            auto &img = std::get<PaintImage>(paintCommand.data);
                            if (planeMeshes.find(img.dstRect.dimensions) == planeMeshes.end()) {
                                planeMeshes[img.dstRect.dimensions] = createPlane(img.dstRect.dimensions);
                            }
                            usedPlanes.insert(img.dstRect.dimensions);
                            break;
                        }
                        case Paint::PAINT_TEXT: {
                            auto &text = std::get<PaintText>(paintCommand.data);
                            for (auto &glyph: text.text.glyphs) {
                                auto dim = glyph.character->image.getResolution().convert<float>();
                                if (planeMeshes.find(dim) == planeMeshes.end()) {
                                    planeMeshes[dim] = createPlane(dim);
                                }
                                usedPlanes.insert(dim);
                            }
                            auto dim = text.text.size.convert<float>();
                            if (planeMeshes.find(dim) == planeMeshes.end()) {
                                planeMeshes[dim] = createPlane(dim);
                            }
                            usedPlanes.insert(dim);
                            break;
                        }
                    }
                }
            }

            std::vector<Vec2f> unusedPoints;
            for (auto &point: pointMeshes) {
                if (usedPoints.find(point.first) == usedPoints.end()) {
                    unusedPoints.push_back(point.first);
                }
            }
            for (auto &point: unusedPoints) {
                destroyPoint(point);
            }

            std::vector<std::pair<Vec2f, Vec2f> > unusedLines;
            for (auto &line: lineMeshes) {
                if (usedLines.find(line.first) == usedLines.end()) {
                    unusedLines.emplace_back(line.first);
                }
            }
            for (auto &line: unusedLines) {
                destroyLine(line.first, line.second);
            }

            std::vector<Vec2f> unusedPlanes;
            for (auto &plane: planeMeshes) {
                if (usedPlanes.find(plane.first) == usedPlanes.end()) {
                    unusedPlanes.emplace_back(plane.first);
                }
            }
            for (auto &plane: unusedPlanes) {
                destroyPlane(plane);
            }

            std::vector<Vec2f> unusedSquares;
            for (auto &square: squareMeshes) {
                if (usedSquares.find(square.first) == usedSquares.end()) {
                    unusedSquares.emplace_back(square.first);
                }
            }
            for (auto &square: unusedSquares) {
                destroySquare(square);
            }
        }

        void upload(const RenderGraphResource &vertexBuffer,
                    const RenderGraphResource &indexBuffer,
                    RenderGraphContext &ctx) {
            for (auto &upload: pendingUploads) {
                ctx.uploadBuffer(vertexBuffer,
                                 upload.vertexData.data(),
                                 upload.vertexData.size(),
                                 upload.vertexOffset);
                ctx.uploadBuffer(indexBuffer,
                                 reinterpret_cast<const uint8_t *>(upload.indexData.data()),
                                 upload.indexData.size() * sizeof(unsigned int),
                                 upload.indexOffset);
            }
            pendingUploads.clear();
        }

        const MeshDrawData &getPoint(const Vec2f &p) const {
            return pointMeshes.at(p);
        }

        const MeshDrawData &getLine(Vec2f a, Vec2f b) const {
            return lineMeshes.at({a, b});
        }

        const MeshDrawData &getPlane(const Vec2f &dimensions) const {
            return planeMeshes.at(dimensions);
        }

        const MeshDrawData &getSquare(const Vec2f &dimensions) const {
            return squareMeshes.at(dimensions);
        }

        size_t getVertexBufferSize() const {
            return vertexBufferSize;
        }

        size_t getIndexBufferSize() const {
            return indexBufferSize;
        }

    private:
        struct BufferUpload {
            std::vector<uint8_t> vertexData;
            size_t vertexOffset = 0;

            std::vector<unsigned int> indexData;
            size_t indexOffset = 0;
        };

        class LinePairHash {
        public:
            std::size_t operator()(const std::pair<Vec2f, Vec2f> &k) const {
                size_t ret = 0;
                hash_combine(ret, k.first);
                hash_combine(ret, k.second);
                return ret;
            }
        };

        std::vector<BufferUpload> pendingUploads;

        std::unordered_map<Vec2f, MeshDrawData> planeMeshes;
        std::unordered_map<Vec2f, MeshDrawData> squareMeshes;
        std::unordered_map<std::pair<Vec2f, Vec2f>, MeshDrawData, LinePairHash> lineMeshes;
        std::unordered_map<Vec2f, MeshDrawData> pointMeshes;

        // start and size of free ranges of vertices with layout vertexLayout in the vertex buffer
        std::map<size_t, size_t> freeVertexBufferRanges;

        // start and size of free ranges of bytes in the index buffer
        std::map<size_t, size_t> freeIndexBufferRanges;

        std::map<size_t, size_t> allocatedVertexRanges;
        std::map<size_t, size_t> allocatedIndexRanges;

        size_t vertexBufferSize = 0;
        size_t indexBufferSize = 0;

        ShaderAttributeLayout vertexLayout{
            {
                {"position", ShaderPrimitiveType::vec2()},
                {"uv", ShaderPrimitiveType::vec2()}
            },
        };

        MeshDrawData createPlane(const Vec2f &size) {
            std::vector<uint8_t> vertexBuffer;
            auto vertData = VertexBuilder()
                    .addVec2(Vec2f(0, 0))
                    .addVec2(Vec2f(0, 0))
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            vertData = VertexBuilder()
                    .addVec2(Vec2f(size.x, 0))
                    .addVec2(Vec2f(1, 0))
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            vertData = VertexBuilder()
                    .addVec2(Vec2f(0, size.y))
                    .addVec2(Vec2f(0, 1))
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            vertData = VertexBuilder()
                    .addVec2(Vec2f(size.x,
                                   size.y))
                    .addVec2(Vec2f(1, 1))
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            auto vertexBufferOffset = allocateVertexData(vertexBuffer.size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);
            indices.emplace_back(1);
            indices.emplace_back(2);

            indices.emplace_back(1);
            indices.emplace_back(2);
            indices.emplace_back(3);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = DrawCall(indexBufferOffset, indices.size());

            planeMeshes[size] = MeshDrawData(TRIANGLES,
                                             drawCall,
                                             vertexBufferOffset / vertexLayout.getLayoutSize());

            BufferUpload upload;
            upload.vertexData = vertexBuffer;
            upload.vertexOffset = vertexBufferOffset;
            upload.indexData = indices;
            upload.indexOffset = indexBufferOffset;
            pendingUploads.emplace_back(upload);

            return planeMeshes.at(size);
        }

        MeshDrawData createSquare(const Vec2f &size) {
            std::vector<uint8_t> vertexBuffer;

            auto vertData = VertexBuilder()
                    .addVec2(Vec2f(0, 0))
                    .addVec2(Vec2f())
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            vertData = VertexBuilder()
                    .addVec2(Vec2f(size.x, 0))
                    .addVec2(Vec2f())
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            vertData = VertexBuilder()
                    .addVec2(Vec2f(0, size.y))
                    .addVec2(Vec2f())
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            vertData = VertexBuilder()
                    .addVec2(Vec2f(size.x,
                                   size.y))
                    .addVec2(Vec2f())
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            auto vertexBufferOffset = allocateVertexData(vertexBuffer.size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);
            indices.emplace_back(1);

            indices.emplace_back(1);
            indices.emplace_back(3);

            indices.emplace_back(3);
            indices.emplace_back(2);

            indices.emplace_back(2);
            indices.emplace_back(0);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = DrawCall(indexBufferOffset, indices.size());

            squareMeshes[size] = MeshDrawData(LINES,
                                              drawCall,
                                              vertexBufferOffset / vertexLayout.getLayoutSize());

            BufferUpload upload;
            upload.vertexData = vertexBuffer;
            upload.vertexOffset = vertexBufferOffset;
            upload.indexData = indices;
            upload.indexOffset = indexBufferOffset;
            pendingUploads.emplace_back(upload);

            return squareMeshes.at(size);
        }

        MeshDrawData createLine(const Vec2f &start, const Vec2f &end) {
            std::vector<uint8_t> vertexBuffer;

            auto vertData = VertexBuilder()
                    .addVec2(start)
                    .addVec2(Vec2f())
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            vertData = VertexBuilder()
                    .addVec2(end)
                    .addVec2(Vec2f())
                    .build();
            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            auto vertexBufferOffset = allocateVertexData(vertexBuffer.size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);
            indices.emplace_back(1);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = DrawCall(indexBufferOffset, indices.size());

            lineMeshes[{start, end}] = MeshDrawData(LINES,
                                                    drawCall,
                                                    vertexBufferOffset / vertexLayout.getLayoutSize());

            BufferUpload upload;
            upload.vertexData = vertexBuffer;
            upload.vertexOffset = vertexBufferOffset;
            upload.indexData = indices;
            upload.indexOffset = indexBufferOffset;
            pendingUploads.emplace_back(upload);

            return lineMeshes.at({start, end});
        }

        MeshDrawData createPoint(const Vec2f &point) {
            std::vector<uint8_t> vertexBuffer;

            auto vertData = VertexBuilder()
                    .addVec2(point)
                    .addVec2(Vec2f())
                    .build();

            vertexBuffer.insert(vertexBuffer.end(), vertData.begin(), vertData.end());

            auto vertexBufferOffset = allocateVertexData(vertexBuffer.size());

            std::vector<unsigned int> indices;
            indices.emplace_back(0);

            auto indexBufferOffset = allocateIndexData(indices.size() * sizeof(unsigned int));

            auto drawCall = DrawCall(indexBufferOffset, indices.size());

            pointMeshes[point] = MeshDrawData(POINTS,
                                              drawCall,
                                              vertexBufferOffset / vertexLayout.getLayoutSize());

            BufferUpload upload;
            upload.vertexData = vertexBuffer;
            upload.vertexOffset = vertexBufferOffset;
            upload.indexData = indices;
            upload.indexOffset = indexBufferOffset;
            pendingUploads.emplace_back(upload);

            return pointMeshes.at(point);
        }

        void destroyPlane(const Vec2f &size) {
            auto drawData = planeMeshes.at(size);
            planeMeshes.erase(size);
            deallocateVertexData(drawData.baseVertex * vertexLayout.getLayoutSize());
            deallocateIndexData(drawData.drawCall.offset);
        }

        void destroySquare(const Vec2f &size) {
            auto drawData = squareMeshes.at(size);
            squareMeshes.erase(size);
            deallocateVertexData(drawData.baseVertex * vertexLayout.getLayoutSize());
            deallocateIndexData(drawData.drawCall.offset);
        }

        void destroyLine(const Vec2f &start, const Vec2f &end) {
            auto pair = std::make_pair(start, end);
            auto drawData = lineMeshes.at(pair);
            lineMeshes.erase(pair);
            deallocateVertexData(drawData.baseVertex * vertexLayout.getLayoutSize());
            deallocateIndexData(drawData.drawCall.offset);
        }

        void destroyPoint(const Vec2f &point) {
            auto drawData = pointMeshes.at(point);
            pointMeshes.erase(point);
            deallocateVertexData(drawData.baseVertex * vertexLayout.getLayoutSize());
            deallocateIndexData(drawData.drawCall.offset);
        }

        size_t allocateVertexData(size_t size) {
            bool foundFreeRange = false;

            auto dataOffset = 0UL;
            for (auto &range: freeVertexBufferRanges) {
                if (range.second >= size) {
                    dataOffset = range.first;
                    foundFreeRange = true;
                }
            }

            if (foundFreeRange) {
                auto rangeSize = freeVertexBufferRanges.at(dataOffset) -= size;
                freeVertexBufferRanges.erase(dataOffset);
                if (rangeSize > 0) {
                    freeVertexBufferRanges[dataOffset + size] = rangeSize;
                }
            } else {
                dataOffset = vertexBufferSize;
            }

            if (vertexBufferSize <= dataOffset || vertexBufferSize < dataOffset + size) {
                vertexBufferSize = dataOffset + size;
            }

            allocatedVertexRanges[dataOffset] = size;

            return dataOffset;
        }

        void deallocateVertexData(size_t offset) {
            auto size = allocatedVertexRanges.at(offset);
            allocatedVertexRanges.erase(offset);
            freeVertexBufferRanges[offset] = size;
        }

        size_t allocateIndexData(size_t size) {
            bool foundFreeRange = false;
            auto ret = 0UL;
            for (auto &range: freeIndexBufferRanges) {
                if (range.second >= size) {
                    ret = range.first;
                    foundFreeRange = true;
                }
            }
            if (foundFreeRange) {
                auto rangeSize = freeIndexBufferRanges.at(ret) -= size;
                freeIndexBufferRanges.erase(ret);
                if (rangeSize > 0) {
                    freeIndexBufferRanges[ret + size] = rangeSize;
                }
            } else {
                ret = indexBufferSize;
            }

            if (indexBufferSize <= ret
                || indexBufferSize <= ret + size) {
                indexBufferSize = ret + size;
            }

            allocatedIndexRanges[ret] = size;

            return ret;
        }

        void deallocateIndexData(size_t offset) {
            auto size = allocatedIndexRanges.at(offset);
            allocatedIndexRanges.erase(offset);
            freeIndexBufferRanges[offset] = size;
        }

        void mergeFreeVertexBufferRanges() {
            bool merged = true;
            while (merged) {
                merged = false;
                auto vertexRanges = freeVertexBufferRanges;
                for (auto range = freeVertexBufferRanges.begin(); range != freeVertexBufferRanges.end(); ++range) {
                    auto next = range;
                    ++next;
                    if (next != freeVertexBufferRanges.end()) {
                        if (range->first + range->second == next->first
                            && vertexRanges.find(range->first) != vertexRanges.end()
                            && vertexRanges.find(next->first) != vertexRanges.end()) {
                            merged = true;
                            vertexRanges.at(range->first) += next->second;
                            vertexRanges.erase(next->first);
                            range = next;
                        }
                    }
                }
                freeVertexBufferRanges = vertexRanges;
            }
        }

        void mergeFreeIndexBufferRanges() {
            bool merged = true;
            while (merged) {
                merged = false;
                auto indexRanges = freeIndexBufferRanges;
                for (auto range = freeIndexBufferRanges.begin(); range != freeIndexBufferRanges.end(); ++range) {
                    auto next = range;
                    ++next;
                    if (next != freeIndexBufferRanges.end()) {
                        if (range->first + range->second == next->first
                            && indexRanges.find(range->first) != indexRanges.end()
                            && indexRanges.find(next->first) != indexRanges.end()) {
                            merged = true;
                            indexRanges.at(range->first) += next->second;
                            indexRanges.erase(next->first);
                            range = next;
                        }
                    }
                }
                freeIndexBufferRanges = indexRanges;
            }
        }
    };
}

#endif //XENGINE_MESHBUFFER2D_HPP
