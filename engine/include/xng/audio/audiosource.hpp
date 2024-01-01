/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2024  Julian Zampiccoli
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

#ifndef XENGINE_AUDIOSOURCE_HPP
#define XENGINE_AUDIOSOURCE_HPP

#include <memory>

#include "audiobuffer.hpp"

#include "xng/math/vector3.hpp"

namespace xng {
    class XENGINE_EXPORT AudioSource {
    public:
        enum SourceType {
            UNDETERMINED,
            STATIC,
            STREAMING
        };

        enum SourceState {
            INITIAL,
            STOPPED,
            PAUSED,
            PLAYING
        };

        virtual ~AudioSource() = default;

        virtual void play() = 0;

        virtual void pause() = 0;

        virtual void stop() = 0;

        virtual void rewind() = 0;

        virtual void setPitch(float pitch) = 0;

        virtual float getPitch() = 0;

        virtual void setGain(float gain) = 0;

        virtual float getGain() = 0;

        virtual void setMaxDistance(float maxDistance) = 0;

        virtual float getMaxDistance() = 0;

        virtual void setRollOffFactor(float rollOffFactor) = 0;

        virtual float getRollOffFactor() = 0;

        virtual void setReferenceDistance(float referenceDistance) = 0;

        virtual float getReferenceDistance() = 0;

        virtual void setMininumGain(float minGain) = 0;

        virtual float getMinimumGain() = 0;

        virtual void setMaximumGain(float maxGain) = 0;

        virtual float getMaximumGain() = 0;

        virtual void setConeOuterGain(float coneOuterGain) = 0;

        virtual float getConeOuterGain() = 0;

        virtual void setConeInnerAngle(float innerAngle) = 0;

        virtual float getConeInnerAngle() = 0;

        virtual void setConeOuterAngle(float outerAngle) = 0;

        virtual float getConeOuterAngle() = 0;

        virtual void setPosition(Vec3f position) = 0;

        virtual Vec3f getPosition() = 0;

        virtual void setVelocity(Vec3f velocity) = 0;

        virtual Vec3f getVelocity() = 0;

        virtual void setDirection(Vec3f direction) = 0;

        virtual Vec3f getDirection() = 0;

        virtual void setSourceRelative(bool relative) = 0;

        virtual bool getSourceRelative() = 0;

        virtual void setSourceType(SourceType type) = 0;

        virtual SourceType getSourceType() = 0;

        virtual void setLooping(bool looping) = 0;

        virtual bool getLooping() = 0;

        virtual SourceState getState() = 0;

        virtual void setBuffer(const AudioBuffer &buffer) = 0;

        virtual void clearBuffer() = 0;

        virtual void queueBuffers(std::vector<std::reference_wrapper<const AudioBuffer>> buffers) = 0;

        virtual std::vector<std::reference_wrapper<const AudioBuffer>> unqueueBuffers() = 0;
    };
}

#endif //XENGINE_AUDIOSOURCE_HPP
