/*
Copyright (c) 2022 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#pragma once

#include "services/concurrency/ConcurrentTaskGraph.hpp"
#include "services/concurrency/Task.hpp"
#include "services/concurrency/WorkerThread.hpp"

#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <set>
#include <unordered_map>

// -----------------------------------------------------------------
//
// @details Provides an implementation of a Thread Pool.  It is intended to
// be used to create & manage worker threads that handle all tasks throughout
// the system.
//
// -----------------------------------------------------------------
class ThreadPool
{
  public:
    ThreadPool(const ThreadPool&) = delete;
    ThreadPool(ThreadPool&&) = delete;
    ThreadPool& operator=(const ThreadPool&) = delete;
    ThreadPool& operator=(ThreadPool&&) = delete;

    static ThreadPool& instance();
    static void terminate();

    void enqueueTask(std::shared_ptr<Task> task);
    void notifyEmpty(std::function<void(void)> onEmpty);

    std::shared_ptr<ConcurrentTaskGraph> createTaskGraph(std::function<void(void)> onComplete = nullptr);
    std::shared_ptr<Task> createTask(std::function<void(void)> job, std::function<void(void)> onComplete = nullptr);
    std::shared_ptr<Task> createIOTask(std::function<void(void)> job, std::function<void(void)> onComplete = nullptr);
    std::shared_ptr<Task> createTask(std::shared_ptr<ConcurrentTaskGraph>& graph, std::function<void(void)> job, std::function<void(void)> onComplete = nullptr);
    std::shared_ptr<Task> createIOTask(std::shared_ptr<ConcurrentTaskGraph>& graph, std::function<void(void)> job, std::function<void(void)> onComplete = nullptr);
    void submitTaskGraph(std::shared_ptr<ConcurrentTaskGraph> graph);

  protected:
    ThreadPool(uint16_t sizeInitial);

  private:
    std::function<void(void)> m_onEmpty{ nullptr };
    std::atomic_uint32_t m_activeTasks{ 0 };
    std::mutex m_mutexTaskComplete;

    std::set<std::shared_ptr<WorkerThread>> m_threads;
    ConcurrentQueue<std::shared_ptr<Task>> m_workQueue;
    std::condition_variable m_eventWorkQueue;
    std::mutex m_mutexWorkQueueEvent;
    std::unordered_map<std::uint64_t, std::shared_ptr<ConcurrentTaskGraph>> m_taskGraphs;

    // Yes, a whole set of separate threading items for the dedicated IO worker
    std::shared_ptr<WorkerThread> m_ioThread;
    ConcurrentQueue<std::shared_ptr<Task>> m_ioWorkQueue;
    std::condition_variable m_ioEventWorkQueue;
    std::mutex m_ioMutexWorkQueueEvent;

    void enqueueAvailableGraphTasks(std::shared_ptr<ConcurrentTaskGraph> graph);
    void taskComplete(const std::shared_ptr<Task>& task);

    friend class WorkerThread; // to allow it to call taskComplete
};
