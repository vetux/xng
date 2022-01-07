/**
 *  XEngine - C++ game engine library
 *  Copyright (C) 2021  Julian Zampiccoli
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License along
 *  with this program; if not, write to the Free Software Foundation, Inc.,
 *  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#ifndef XENGINE_AUDIO_HPP
#define XENGINE_AUDIO_HPP

#include "platform/audio/audioformat.hpp"

#include "asset/asset.hpp"

namespace xengine {
    class XENGINE_EXPORT Audio : public AssetBase {
    public:
        ~Audio() override = default;

        AssetBase *clone() override {
            return new Audio(*this);
        }

        std::vector<uint8_t> buffer;
        AudioFormat format;
        unsigned int frequency;
    };
}

#endif //XENGINE_AUDIO_HPP
