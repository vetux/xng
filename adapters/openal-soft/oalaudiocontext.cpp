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

#include "oalaudiocontext.hpp"
#include "oalaudiobuffer.hpp"
#include "oalaudiosource.hpp"

#include "oalcheckerror.hpp"

namespace xng {
    namespace openal {
        OALAudioContext::OALAudioContext(ALCcontext *context) : context(context), listener() {}

        OALAudioContext::~OALAudioContext() {
            if (alcGetCurrentContext() == context)
                alcMakeContextCurrent(nullptr);
            alcDestroyContext(
                    context); //TODO:Fix: Destroying a openal context seems to always set invalid operation error
        }

        void OALAudioContext::makeCurrent() {
            alcMakeContextCurrent(context);
            checkOALError();
        }

        AudioListener &OALAudioContext::getListener() {
            return listener;
        }

        std::unique_ptr<AudioBuffer> OALAudioContext::createBuffer() {
            ALuint n;
            alGenBuffers(1, &n);
            checkOALError();
            return std::make_unique<OALAudioBuffer>(n);
        }

        std::unique_ptr<AudioSource> OALAudioContext::createSource() {
            ALuint n;
            alGenSources(1, &n);
            checkOALError();
            return std::make_unique<OALAudioSource>(n);
        }

        const ALCcontext *OALAudioContext::getContext() {
            return context;
        }
    }
}