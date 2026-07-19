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
    struct Query {
        std::string passName;
        GLuint queries[2]{};

        explicit Query(std::string passName)
            : passName(std::move(passName)) {
            glGenQueries(2, queries);
        }

        ~Query() {
            glDeleteQueries(2, queries);
        }
    };

    class FenceGL final : public rg::Fence {
    public:
        FenceGL()
            : timelineFetched(true) {
            fence = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
        }

        explicit FenceGL(Timeline timeline, std::vector<Query> queries)
            : timeline(std::move(timeline)),
              queries(std::move(queries)),
              timelineFetched(false) {
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

        const Timeline &getTimeline() override {
            if (timelineFetched) {
                return timeline;
            }

            for (const auto &query: queries) {
                GLuint64 startTime, endTime;
                glGetQueryObjectui64v(query.queries[0], GL_QUERY_RESULT, &startTime);
                glGetQueryObjectui64v(query.queries[1], GL_QUERY_RESULT, &endTime);
                timeline.slices.emplace_back(query.passName, startTime, endTime);
            }
            queries.clear();

            GLint64 gpuNow;
            glGetInteger64v(GL_TIMESTAMP, &gpuNow);
            timeline.finishTime = gpuNow;

            timelineFetched = true;

            return timeline;
        }

    private:
        GLsync fence;
        bool done = false;

        Timeline timeline{};
        std::vector<Query> queries;
        bool timelineFetched;
    };
}

#endif //XENGINE_FENCEGL_HPP
