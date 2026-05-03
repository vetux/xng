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

#include <vector>

#include "oalaudiobuffer.hpp"
#include "oalcheckerror.hpp"


namespace xng {
    namespace openal {
        int convertFormat(AudioFormat format) {
            switch (format) {
                case MONO8:
                    return AL_FORMAT_MONO8;
                case MONO16:
                    return AL_FORMAT_MONO16;
                case STEREO8:
                    return AL_FORMAT_STEREO8;
                case STEREO16:
                    return AL_FORMAT_STEREO16;
                case BFORMAT2D_16:
                    return AL_FORMAT_BFORMAT2D_16;
                case BFORMAT3D_16:
                    return AL_FORMAT_BFORMAT3D_16;
            }
            throw std::runtime_error("Unrecognized format");
        }

        OALAudioBuffer::OALAudioBuffer(ALuint handle) : handle(handle) {}

        OALAudioBuffer::~OALAudioBuffer() {
            alDeleteBuffers(1, &handle);
            checkOALError();
        }

        void OALAudioBuffer::upload(const std::vector<uint8_t> &buffer, AudioFormat format, unsigned int frequency) {
            alBufferData(handle,
                         convertFormat(format),
                         buffer.data(),
                         static_cast<ALsizei>(buffer.size()),
                         static_cast<ALsizei>(frequency));
            checkOALError();
        }
    }
}