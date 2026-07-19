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

#ifndef XENGINE_RENDERMESH_HPP
#define XENGINE_RENDERMESH_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/meshstreamer.hpp"
#include "xng/renderer/objects/renderskeleton.hpp"

namespace xng {
    //TODO: Design meshlet integration technique (For Meshlet volume based culling)
    class RenderMesh final : public RenderObject {
    public:
        RenderMesh() = default;

        RenderMesh(MeshStreamer &mesh_stream,
                   const MeshStreamer::Handle mesh_handle,
                   RenderObjectHandle<RenderSkeleton> skeleton)
            : meshStream(&mesh_stream),
              meshHandle(mesh_handle),
              skeleton(std::move(skeleton)) {
        }

        [[nodiscard]] MeshStreamer::Handle getHandle() const {
            return meshHandle;
        }

        [[nodiscard]] const MeshStreamer::Allocation &getAllocation() const {
            if (!meshStream) {
                throw std::runtime_error("Uninitialized RenderMesh");
            }
            return meshStream->getAllocation(meshHandle);
        }

        [[nodiscard]] const RenderObjectHandle<RenderSkeleton> &getSkeleton() const {
            return skeleton;
        }

        bool isUploadComplete() override {
            if (!meshStream) {
                throw std::runtime_error("Uninitialized RenderMesh");
            }
            if (skeleton.isAssigned() && !skeleton.get().isUploadComplete()) {
                return false;
            }
            return meshStream->isUploadComplete(meshHandle);
        }

        void flush() override {
            if (!meshStream) {
                throw std::runtime_error("Uninitialized RenderMesh");
            }
            if (skeleton.isAssigned()) {
                skeleton.get().flush();
            }
            meshStream->flush(meshHandle);
        }

    private:
        MeshStreamer *meshStream;
        MeshStreamer::Handle meshHandle;
        RenderObjectHandle<RenderSkeleton> skeleton{};
    };
}

#endif //XENGINE_RENDERMESH_HPP
