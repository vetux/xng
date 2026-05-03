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

#ifndef XENGINE_AUDIODATA_HPP
#define XENGINE_AUDIODATA_HPP

#include <vector>

#include "xng/audio/audioformat.hpp"

#include "xng/resource/resourcebase.hpp"

namespace xng {
    struct AudioData final : ResourceBase {
        RESOURCE_TYPENAME(AudioData)

        ~AudioData() override = default;

        std::unique_ptr<ResourceBase> clone() override {
            return std::make_unique<AudioData>(*this);
        }

        std::vector<uint8_t> buffer;
        AudioFormat format;
        unsigned int frequency;
    };
}

#endif //XENGINE_AUDIODATA_HPP
