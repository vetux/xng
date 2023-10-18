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

#ifndef XENGINE_THREADPOOL_HPP
#define XENGINE_THREADPOOL_HPP

#include <memory>
#include <vector>
#include <thread>
#include <functional>
#include <map>
#include <cassert>

#include "task.hpp"

namespace xng {
    class XENGINE_EXPORT ThreadPool {
    public:
        static ThreadPool &getPool();

        explicit ThreadPool(unsigned int numberOfThreads = std::thread::hardware_concurrency()) : mShutdown(false) {
            assert(numberOfThreads > 0);
            for (int i = 0; i < numberOfThreads; i++) {
                threads.emplace_back([this]() { pollTasks(); });
            }
        }

        ~ThreadPool() {
            shutdown();
            for (auto &thread: threads) {
                thread.join();
            }
        }

        std::shared_ptr<Task> addTask(const std::function<void()> &work) {
            if (mShutdown)
                throw std::runtime_error("Thread pool was shut down");

            std::shared_ptr<Task> ret;
            {
                const std::lock_guard<std::mutex> l(taskMutex);
                int index;
                if (unusedTaskIndices.empty()) {
                    if (taskIndex == std::numeric_limits<int>::max())
                        throw std::runtime_error("Maximum concurrent tasks reached");
                    index = taskIndex++;
                } else {
                    index = *unusedTaskIndices.begin();
                    unusedTaskIndices.erase(unusedTaskIndices.begin());
                }
                ret = std::make_shared<Task>(work);
                tasks[index] = ret;
            }

            taskVar.notify_one();

            return ret;
        }

        void shutdown() {
            mShutdown = true;
            taskVar.notify_all();
        }

        bool isShutdown() const { return mShutdown; }

    private:
        std::vector<std::thread> threads;
        std::map<int, std::shared_ptr<Task>> tasks;

        int taskIndex = 0;
        std::vector<int> unusedTaskIndices;

        std::mutex taskMutex;
        std::condition_variable taskVar;
        std::atomic<bool> mShutdown = false;

        void pollTasks() {
            std::unique_lock<std::mutex> taskLock(taskMutex);
            while (!mShutdown) {
                if (!tasks.empty()) {
                    auto pair = tasks.begin();
                    int index = pair->first;
                    std::shared_ptr<Task> task = pair->second;

                    tasks.erase(index);
                    unusedTaskIndices.emplace_back(index);

                    taskLock.unlock();
                    task->start();
                    taskLock.lock();
                }
                taskVar.wait(taskLock, [this] {
                    if (mShutdown)
                        return true;
                    return !tasks.empty();
                });
            }
        }
    };
}

#endif //XENGINE_THREADPOOL_HPP
