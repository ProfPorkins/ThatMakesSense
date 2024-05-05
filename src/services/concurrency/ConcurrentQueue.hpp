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

#include <deque>
#include <mutex>
#include <optional>
#include <vector>

// ------------------------------------------------------------------
//
// @details This is a fairly simple concurrent queue that allows thread
// safe access.
//
// ------------------------------------------------------------------
template <typename T>
class ConcurrentQueue
{
  public:
    // ------------------------------------------------------------------
    //
    // Enqueues a new item onto the queue
    //
    // ------------------------------------------------------------------
    void enqueue(const T& item)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        m_queue.push_back(item);
    }

    // ------------------------------------------------------------------
    //
    // Enqueues a a group of items onto the queue, all at once
    //
    // ------------------------------------------------------------------
    void enqueue(const std::vector<T>& items)
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        for (auto&& item : items)
        {
            m_queue.push_back(item);
        }
    }

    // ------------------------------------------------------------------
    //
    // Attempts to dequeue an item.  An std::optional is use to return
    // the value, allowing the client code to know whether or not a
    // value was available.
    //
    // ------------------------------------------------------------------
    std::optional<T> dequeue()
    {
        std::lock_guard<std::mutex> lock(m_mutex);

        if (!m_queue.empty())
        {
            auto item = m_queue.front();
            m_queue.pop_front();
            return item;
        }

        return std::nullopt;
    }

    std::size_t size() { return m_queue.size(); }

  private:
    std::deque<T> m_queue;
    std::mutex m_mutex;
};
