/**
 *  xEngine - C++ Game Engine Library
 *  Copyright (C) 2023  Julian Zampiccoli
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

#include "xng/async/task.hpp"

namespace xng {
    Task &Task::operator=(const Task &other) {
        work = other.work;
        return *this;
    }

    void Task::start() {
        try {
            work();
        } catch (...) {
            exception = std::current_exception();
        }

        {
            std::unique_lock<std::mutex> lk(mutex);
            workDone = true;
            lk.unlock();
        }
        workDoneCondition.notify_all();
    }

    const std::exception_ptr &Task::join() {
        std::unique_lock<std::mutex> lk(mutex);
        while (!workDone) {
            workDoneCondition.wait(lk, [this] { return static_cast<bool>(workDone); });
        }
        return exception;
    }

    bool Task::wait(long ms) {
        auto dur = std::chrono::milliseconds(ms);
        auto start = std::chrono::high_resolution_clock::now();
        std::unique_lock<std::mutex> lk(mutex);
        while (!workDone && std::chrono::high_resolution_clock::now() - start < dur) {
            workDoneCondition.wait_for(lk,
                                       std::chrono::milliseconds(ms),
                                       [this] { return static_cast<bool>(workDone); });
        }
        return workDone;
    }
}