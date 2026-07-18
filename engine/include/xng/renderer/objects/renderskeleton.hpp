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

#ifndef XENGINE_RENDERSKELETON_HPP
#define XENGINE_RENDERSKELETON_HPP

#include "xng/renderer/renderobject.hpp"
#include "xng/renderer/stream/skeletonstreamer.hpp"

namespace xng {
    class RenderSkeleton final : public RenderObject {
    public:
        RenderSkeleton(SkeletonStreamer &boneStream,
                       const std::vector<std::string> &boneNames)
            : skeletonStream(boneStream),
              skeletonHandle(skeletonStream.create(boneNames.size())) {
            for (size_t i = 0; i < boneNames.size(); i++) {
                boneOffsets[boneNames.at(i)] = static_cast<unsigned int>(i);
            }
        }

        ~RenderSkeleton() override {
            skeletonStream.destroy(skeletonHandle);
        }

        /**
         * Update a set of bones in the skeleton.
         * The passed map must contain all bones.
         *
         * @param bones The absolute transforms (animated) of the bones.
         */
        void setBones(const std::unordered_map<std::string, Mat4f> &bones) const {
            std::vector<Mat4f> boneMatrices;
            boneMatrices.resize(boneOffsets.size());
            for (const auto &pair: boneOffsets) {
                boneMatrices[pair.second] = bones.at(pair.first);
            }
            skeletonStream.upload(skeletonHandle, boneMatrices);
        }

        SkeletonStreamer::BaseBone getBaseBone() const {
            return skeletonHandle;
        }

        const std::unordered_map<std::string, unsigned int> &getOffsets() const {
            return boneOffsets;
        }

        bool isUploadComplete() override {
            return skeletonStream.isUploadComplete(skeletonHandle);
        }

        void flush() override {
            skeletonStream.flush(skeletonHandle);
        }

    private:
        SkeletonStreamer &skeletonStream;
        SkeletonStreamer::BaseBone skeletonHandle;
        std::unordered_map<std::string, unsigned int> boneOffsets; // Indices into the skeleton buffer relative to base
    };
}

#endif //XENGINE_RENDERSKELETON_HPP
