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

#ifndef XENGINE_OALAUDIOBUFFER_HPP
#define XENGINE_OALAUDIOBUFFER_HPP

#include "openalinclude.hpp"

#include "xng/audio/audiobuffer.hpp"

namespace xng {
    namespace openal {
        class OALAudioBuffer : public AudioBuffer {
        public:
            const ALuint handle;

            explicit OALAudioBuffer(ALuint handle);

            ~OALAudioBuffer() override;

            void upload(const std::vector<uint8_t> &buffer, AudioFormat format, unsigned int frequency) override;
        };
    }
}

#endif //XENGINE_OALAUDIOBUFFER_HPP
