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

#ifndef XENGINE_OALAUDIOCONTEXT_HPP
#define XENGINE_OALAUDIOCONTEXT_HPP

#include "openalinclude.hpp"

#include "xng/audio/audiocontext.hpp"

#include "oalaudiolistener.hpp"

namespace xng {
    namespace openal {
        class OALAudioContext : public AudioContext {
        public:
            explicit OALAudioContext(ALCcontext *context);

            ~OALAudioContext() override;

            void makeCurrent() override;

            AudioListener &getListener() override;

            std::unique_ptr<AudioBuffer> createBuffer() override;

            std::unique_ptr<AudioSource> createSource() override;

            const ALCcontext *getContext();

        private:
            ALCcontext *context;
            OALAudioListener listener;
        };
    }
}

#endif //XENGINE_OALAUDIOCONTEXT_HPP
