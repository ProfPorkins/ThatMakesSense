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

#include "ConcurrentTaskGraph.hpp"

#include <algorithm>
#include <atomic>
#include <cassert>

ConcurrentTaskGraph::ConcurrentTaskGraph(std::function<void(void)> onComplete) :
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

void ConcurrentTaskGraph::add(std::shared_ptr<Task> node)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    assert(m_finalized == false);
    //
    // Verify we aren't doing something stupid, check to see this task isn't already part of the queue
    assert(m_adjacencyList.find(node->getId()) == m_adjacencyList.end());

    m_nodes[node->getId()] = node;
    m_adjacencyList[node->getId()] = {};
    m_predecessorCount[node->getId()] = 0; // Start with 0, but the actual count gets done later on
}

void ConcurrentTaskGraph::declarePredecessor(std::uint64_t predecessor, std::uint64_t node)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    assert(m_finalized == false);
    assert(m_nodes.contains(node));
    assert(m_nodes.contains(predecessor));
    assert(m_adjacencyList.contains(predecessor));
    // NOTE: If there are a lot of dependencies, this assert could get close
    assert(std::find(m_adjacencyList[predecessor].begin(), m_adjacencyList[predecessor].end(), node) == m_adjacencyList[predecessor].end());

    // Add the directed edge : Muthor of assumptions the edge doesn't already exist!
    m_adjacencyList[predecessor].push_back(node);
}

// ------------------------------------------------------------------
//
// This method traverses the graph using a topological ordering, placing
// the values into the final item/task queue based upon the graph
// dependencies.  Once the graph is finalized, the 'add' methods should
// not be called again.
//
// ------------------------------------------------------------------
void ConcurrentTaskGraph::finalize()
{
    std::lock_guard<std::mutex> lock(m_mutex);

    assert(m_finalized == false);

    // Step 1: Count the predecessors
    for (auto&& [id, predecessor] : m_adjacencyList)
    {
        for (auto nodeId : predecessor)
        {
            m_predecessorCount[nodeId]++;
        }
    }

    // Step 2: For each node with a predecessor count of 0, add to the available task queue
    for (auto&& [id, count] : m_predecessorCount)
    {
        if (count == 0)
        {
            m_queueExecutable.enqueue(id);
            m_countEnqueued++;
        }
    }

    // Step 3: Performed in the taskCompete method below.  We have to wait for the
    //         task that have 0 predecessors to execute before continuing with the
    //         topological ordering any further.

    m_finalized = true;
}

void ConcurrentTaskGraph::taskComplete(std::uint64_t taskId)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    //
    // If both the graph and the task queue are empty, that means this was the last task
    // in this graph to finish.  Therefore, the onComplete function needs to be called.
    if (m_onComplete && queueEmpty() && graphEmpty())
    {
        m_onComplete();
    }

    // Step 3: Based on the completed executtion of this node, work through its
    //         successor nodes, updating predecessor counts and adding new nodes into
    //         the available task queue as appropriate.

    // For each successor of this node, subtract one from the predecessor count.
    // Then, if that predecessor count goes to 0, add it to the available task queue
    for (auto successorId : m_adjacencyList[taskId])
    {
        m_predecessorCount[successorId]--;
        if (m_predecessorCount[successorId] == 0)
        {
            m_queueExecutable.enqueue(successorId);
            m_countEnqueued++;
        }
    }
}

std::shared_ptr<Task> ConcurrentTaskGraph::dequeue()
{
    std::lock_guard<std::mutex> lock(m_mutex);
    assert(!queueEmpty());

    return m_nodes[m_queueExecutable.dequeue().value()];
}

bool ConcurrentTaskGraph::queueEmpty()
{
    return m_queueExecutable.size() == 0;
}

bool ConcurrentTaskGraph::graphEmpty()
{
    return m_countEnqueued == m_predecessorCount.size();
}
