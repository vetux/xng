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
    class RenderMesh final : public RenderObject {
    public:
        RenderMesh(MeshStreamer &meshStream,
                   const Mesh &mesh,
                   std::shared_ptr<RenderSkeleton> _skeleton)
            : meshStream(meshStream), skeleton(std::move(_skeleton)) {
            if (skeleton) {
                meshHandle = meshStream.create(mesh, skeleton->getOffsets());
            } else {
                meshHandle = meshStream.create(mesh, {});
            }
        }

        ~RenderMesh() override {
            meshStream.destroy(meshHandle);
        }

        [[nodiscard]] MeshStreamer::Handle getHandle() const {
            return meshHandle;
        }

        [[nodiscard]] const MeshStreamer::Allocation &getAllocation() const {
            return meshStream.getAllocation(meshHandle);
        }

        [[nodiscard]] std::shared_ptr<RenderSkeleton> getSkeleton() const {
            return skeleton;
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

        std::shared_ptr<RenderSkeleton> skeleton{};
    };
}

#endif //XENGINE_RENDERMESH_HPP
