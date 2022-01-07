/**
 *  Mana - 3D Game Engine
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

#ifndef MANA_TASK_HPP
#define MANA_TASK_HPP

#include <functional>
#include <condition_variable>
#include <mutex>
#include <atomic>

namespace xengine {
    class MANA_EXPORT Task {
    public:
        Task() : work(),
                 mutex(),
                 workDone(false),
                 workDoneCondition() {
        };

        Task(const Task &other) : work(other.work),
                                  mutex(),
                                  workDone(false),
                                  workDoneCondition() {
        }

        explicit Task(std::function<void()> work) : work(std::move(work)),
                                                    mutex(),
                                                    workDone(false),
                                                    workDoneCondition() {
        }

        Task &operator=(const Task &other) {
            work = other.work;
            return *this;
        }

        void start() {
            work();
            {
                std::unique_lock<std::mutex> lk(mutex);
                workDone = true;
                lk.unlock();
            }
            workDoneCondition.notify_all();
        }

        void wait() {
            std::unique_lock<std::mutex> lk(mutex);
            while (!workDone) {
                workDoneCondition.wait(lk, [this] { return static_cast<bool>(workDone); });
            }
        }

        bool wait(long ms) {
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

    private:
        std::function<void()> work;
        std::mutex mutex;
        std::atomic<bool> workDone;
        std::condition_variable workDoneCondition;
    };
}

#endif //MANA_TASK_HPP
