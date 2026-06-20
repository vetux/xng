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

#ifndef XENGINE_HEAPTRANSFERGL_HPP
#define XENGINE_HEAPTRANSFERGL_HPP

#include <memory>
#include <mutex>

#include "glad/glad.h"

#include "xng/rendergraph/semaphore.hpp"

namespace xng::opengl {
    struct HeapTransferSync {
        std::mutex mutex;
        std::condition_variable cv;
        GLsync fence = nullptr;
        bool done = false;
    };

    class SemaphoreGL final : public rg::Semaphore {
    public:
        explicit SemaphoreGL() = default;

        explicit SemaphoreGL(std::shared_ptr<HeapTransferSync> sync)
            : sync(std::move(sync)) {}

        bool isSignaled() override {
            return sync->done;
        }

        bool wait(const size_t timeOut) override {
            if (sync->done) return true;

            std::unique_lock lock(sync->mutex);
            sync->cv.wait(lock, [this] { return sync->fence != nullptr || sync->done; });

            if (sync->done) return true;

            const auto result = glClientWaitSync(sync->fence, GL_SYNC_FLUSH_COMMANDS_BIT, timeOut);
            if (result == GL_ALREADY_SIGNALED || result == GL_CONDITION_SATISFIED) {
                glDeleteSync(sync->fence);
                sync->fence = nullptr;
                sync->done = true;
                return true;
            }
            return false;
        }

    private:
        std::shared_ptr<HeapTransferSync> sync;
    };
}

#endif //XENGINE_HEAPTRANSFERGL_HPP
