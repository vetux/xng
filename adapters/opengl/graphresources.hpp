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

#ifndef XENGINE_GRAPHCONTEXT_HPP
#define XENGINE_GRAPHCONTEXT_HPP

#include "xng/rendergraph/rendergraph.hpp"

#include "ogl/ogltexture.hpp"
#include "ogl/oglshaderprogram.hpp"
#include "ogl/oglvertexbuffer.hpp"
#include "ogl/oglindexbuffer.hpp"
#include "ogl/oglshaderstoragebuffer.hpp"

struct GraphResources {
    RenderGraph graph;

    std::unordered_map<RenderGraphResource, RenderGraphPipeline, RenderGraphResourceHash> pipelines;
    std::unordered_map<RenderGraphResource, CompiledPipeline, RenderGraphResourceHash> compiledPipelines;

    std::unordered_map<RenderGraphResource, std::shared_ptr<OGLShaderProgram>, RenderGraphResourceHash> shaderPrograms;
    std::unordered_map<RenderGraphResource, std::shared_ptr<OGLTexture>, RenderGraphResourceHash> textures;
    std::unordered_map<RenderGraphResource, std::shared_ptr<OGLVertexBuffer>, RenderGraphResourceHash> vertexBuffers;
    std::unordered_map<RenderGraphResource, std::shared_ptr<OGLIndexBuffer>, RenderGraphResourceHash> indexBuffers;
    std::unordered_map<RenderGraphResource, std::shared_ptr<OGLShaderStorageBuffer>, RenderGraphResourceHash> storageBuffers;
};

#endif //XENGINE_GRAPHCONTEXT_HPP
