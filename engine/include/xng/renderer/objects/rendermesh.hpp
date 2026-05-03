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

#include "xng/renderer/stream/meshstreamer.hpp"
#include "xng/renderer/objects/renderskeleton.hpp"

namespace xng {
    class RenderMesh final : public RenderObject {
    public:
        RenderMesh(const Id id,
                   MeshStreamer &meshStream,
                   const Mesh &mesh,
                   RenderObjectHandle<RenderSkeleton> _skeleton)
            : RenderObject(id, OBJECT_MESH), meshStream(meshStream), skeleton(std::move(_skeleton)) {
            meshHandle = meshStream.create();

            if (skeleton) {
                meshStream.upload(meshHandle, mesh, skeleton->getBoneHandles());
            } else {
                meshStream.upload(meshHandle, mesh, {});
            }
        }

        ~RenderMesh() override {
            meshStream.destroy(meshHandle);
        }

        [[nodiscard]] MeshStreamer::Handle getMeshHandle() const {
            return meshHandle;
        }

        bool isUploadComplete() override {
            if (skeleton && !skeleton->isUploadComplete()) {
                return false;
            }
            return meshStream.isUploadComplete(meshHandle);
        }

        void flush() override {
            if (skeleton) {
                skeleton->flush();
            }
            meshStream.flush(meshHandle);
        }

    private:
        MeshStreamer &meshStream;
        MeshStreamer::Handle meshHandle;

        RenderObjectHandle<RenderSkeleton> skeleton{};
    };
}

#endif //XENGINE_RENDERMESH_HPP
