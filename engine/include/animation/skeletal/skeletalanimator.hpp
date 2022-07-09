/**
 *  This file is part of xEngine, a C++ game engine library.
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_SKELETALANIMATOR_HPP
#define XENGINE_SKELETALANIMATOR_HPP

#include "animation/skeletal/skeletalanimation.hpp"
#include "animation/skeletal/bone.hpp"

namespace xng {
    class SkeletalAnimator {
    public:
        SkeletalAnimator() = default;

        SkeletalAnimator(SkeletalAnimation animation, std::vector<Bone> bones, bool loop);

        /**
         * Advance the animation by deltaTime
         *
         * @param deltaTime
         */
        void update(float deltaTime);

        /**
         * @return Bone transformations containing the total transformation of a bone for the current animation state.
         */
        const std::vector<Mat4f> &getBoneMatrices();

        /**
         * @return The set of bones for each vertex with an iterator into the vector returned from getBoneMatrices and the corresponding weight.
         */
        const std::vector<std::vector<std::pair<std::vector<Mat4f>::iterator, float>>> getVertexMapping();

    private:
        SkeletalAnimation animation;
        std::vector<Bone> bones;
        bool loop = true;

        std::vector<Mat4f> boneMatrices;
        std::vector<std::vector<std::pair<std::vector<Mat4f>::iterator, float>>> vertexMapping;
    };
}

#endif //XENGINE_SKELETALANIMATOR_HPP
