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

#include "Task.hpp"

#include <atomic>

// -----------------------------------------------------------------
//
// The only important thing that happens here is that a unique id
// is assigned to the task.
//
// -----------------------------------------------------------------
Task::Task(std::uint64_t graphId, bool isIO, std::function<void()> job, std::function<void()> onComplete) :
    m_graphId(graphId),
    m_isIO(isIO),
    m_job(job),
    m_onComplete(onComplete)
{

    //
    // I know this isn't a guarantee, but this is something I'm willing to live with.
    // If we go over 2^64 of these during the normal operation of the program, that's
    // when I'll revise the code, until then, this is just fine.  For reference, I
    // did a quick calc:  at 10 billion tasks per second, it would take 58 years to
    // run out of 64 bit integers.  For now, we are okay.
    static std::atomic_uint64_t currentId{ 1 };

    m_id = currentId.load();
    currentId++;
}

void Task::execute()
{
    m_job();
    if (m_onComplete)
    {
        m_onComplete();
    }
}