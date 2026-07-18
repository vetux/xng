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

#ifndef XENGINE_FENCEGL_HPP
#define XENGINE_FENCEGL_HPP

#include <memory>
#include <mutex>
#include <condition_variable>

#include "glad/glad.h"

#include "xng/rendergraph/fence.hpp"

namespace xng::opengl {
    struct HeapTransferSync {
        GLsync fence = nullptr;
    };

    class FenceGL final : public rg::Fence {
    public:
        explicit FenceGL() {
            fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }

        ~FenceGL() override {
            glDeleteSync(fence);
        }

        bool isSignaled() override {
            return wait(0);
        }

        bool wait(const size_t timeOut) override {
            if (done) {
                return true;
            }

            const auto result = glClientWaitSync(fence, GL_SYNC_FLUSH_COMMANDS_BIT, timeOut);
            if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                done = true;
            }
            return done;
        }

    private:
        GLsync fence;
        bool done = false;
    };
}

#endif //XENGINE_FENCEGL_HPP
