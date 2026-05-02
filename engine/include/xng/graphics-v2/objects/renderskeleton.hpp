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

#ifndef XENGINE_RENDERSKELETON_HPP
#define XENGINE_RENDERSKELETON_HPP

#include "xng/graphics-v2/renderobject.hpp"
#include "xng/graphics-v2/shadertypes.hpp"
#include "xng/graphics-v2/stream/bufferstreamer.hpp"

namespace xng {
    class RenderSkeleton final : public RenderObject {
    public:
        RenderSkeleton(const Id id,
                       BufferStreamer<ShaderTransform::CPU> &boneStream,
                       const std::vector<std::string> &boneNames)
            : RenderObject(id, OBJECT_SKELETON), boneStream(boneStream) {
            for (const auto &name: boneNames) {
                boneHandles[name] = boneStream.create();
            }
        }

        ~RenderSkeleton() override {
            for (const auto &pair: boneHandles) {
                boneStream.destroy(pair.second);
            }
        }

        /**
         * @param bones The absolute transforms (animated) of the bones.
         */
        void setBones(const std::unordered_map<std::string, Mat4f> &bones) {
            for (const auto &pair: bones) {
                boneStream.upload(boneHandles[pair.first], {pair.second});
            }
        }

        const std::unordered_map<std::string, BufferStreamer<ShaderTransform::CPU>::Handle> &getBoneHandles() const {
            return boneHandles;
        }

        bool isUploadComplete() override {
            for (const auto &pair: boneHandles) {
                if (!boneStream.isUploadComplete(pair.second)) {
                    return false;
                }
            }
            return true;
        }

        void flush() override {
            for (const auto &pair: boneHandles) {
                boneStream.flush(pair.second);
            }
        }

    private:
        BufferStreamer<ShaderTransform::CPU> &boneStream;
        std::unordered_map<std::string, BufferStreamer<ShaderTransform::CPU>::Handle> boneHandles;
    };
}

#endif //XENGINE_RENDERSKELETON_HPP