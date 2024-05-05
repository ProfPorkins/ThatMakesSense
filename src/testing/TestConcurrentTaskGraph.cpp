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

#include "../services/concurrency/ConcurrentTaskGraph.hpp"

#include <gtest/gtest.h>
#include <memory>
#include <unordered_set>

TEST(ConcurrentTaskGraph, NoDependencies)
{
    ConcurrentTaskGraph graph;
    std::unordered_set<std::uint64_t> taskSet;

    auto t1 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t2 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t3 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t4 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });

    taskSet.insert(t1->getId());
    taskSet.insert(t2->getId());
    taskSet.insert(t3->getId());
    taskSet.insert(t4->getId());

    graph.add(t1);
    graph.add(t2);
    graph.add(t3);
    graph.add(t4);

    graph.finalize();

    while (!graph.queueEmpty())
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        EXPECT_TRUE(taskSet.contains(task->getId()));
    }

    EXPECT_TRUE(graph.graphEmpty());
}

TEST(ConcurrentTaskGraph, LinearDependencies)
{
    ConcurrentTaskGraph graph;

    auto t1 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t2 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t3 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t4 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });

    graph.add(t1);
    graph.add(t2);
    graph.add(t3);
    graph.add(t4);

    graph.declarePredecessor(t1->getId(), t2->getId());
    graph.declarePredecessor(t2->getId(), t3->getId());
    graph.declarePredecessor(t3->getId(), t4->getId());

    graph.finalize();

    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_EQ(task->getId(), t1->getId());
    }
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_EQ(task->getId(), t2->getId());
    }
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_EQ(task->getId(), t3->getId());
    }
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_EQ(task->getId(), t4->getId());
    }

    EXPECT_TRUE(graph.queueEmpty());
    EXPECT_TRUE(graph.graphEmpty());
}

TEST(ConcurrentTaskGraph, MultiDependencies1)
{
    // ConcurrentTaskGraph graph;
    // std::unordered_set<std::uint64_t> taskSet;

    // auto t1 = std::make_shared<Task>(graph.getId(), false, []()
    //                                  {
    //                                  });
    // auto t2 = std::make_shared<Task>(graph.getId(), false, []()
    //                                  {
    //                                  });
    // auto t3 = std::make_shared<Task>(graph.getId(), false, []()
    //                                  {
    //                                  });
    // auto t4 = std::make_shared<Task>(graph.getId(), false, []()
    //                                  {
    //                                  });

    // graph.add(t1);
    // graph.add(t2);
    // graph.add(t3);
    // graph.add(t4);
    // graph.add(t3);

    // graph.declarePredecessor(t1->getId(), t2->getId());
    // graph.declarePredecessor(t2->getId(), t3->getId());
    // graph.declarePredecessor(t4->getId(), t3->getId());

    // taskSet.insert(t1->getId());
    // taskSet.insert(t4->getId());

    // graph.finalize();

    //{
    //    auto task = graph.dequeue();
    //    task->execute();
    //    graph.taskComplete(task->getId());
    //    // Using getId() to make understanding the test results easier
    //    EXPECT_EQ(task->getId(), t1->getId());
    //}
    //{
    //    auto task = graph.dequeue();
    //    task->execute();
    //    graph.taskComplete(task->getId());
    //    // Using getId() to make understanding the test results easier
    //    EXPECT_EQ(task->getId(), t4->getId());
    //}
    //{
    //    auto task = graph.dequeue();
    //    task->execute();
    //    graph.taskComplete(task->getId());
    //    // Using getId() to make understanding the test results easier
    //    EXPECT_EQ(task->getId(), t2->getId());
    //}
    //{
    //    auto task = graph.dequeue();
    //    task->execute();
    //    graph.taskComplete(task->getId());
    //    // Using getId() to make understanding the test results easier
    //    EXPECT_EQ(task->getId(), t3->getId());
    //}

    // EXPECT_TRUE(graph.queueEmpty());
    // EXPECT_TRUE(graph.graphEmpty());
}

TEST(ConcurrentTaskGraph, MultiDependencies2)
{
    ConcurrentTaskGraph graph;
    std::unordered_set<std::uint64_t> taskSet;

    auto t1 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t2 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t3 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t4 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });
    auto t0 = std::make_shared<Task>(graph.getId(), false, []()
                                     {
                                     });

    graph.add(t0);
    graph.add(t1);
    graph.add(t2);
    graph.add(t3);
    graph.add(t4);

    graph.declarePredecessor(t2->getId(), t1->getId());
    graph.declarePredecessor(t3->getId(), t1->getId());
    graph.declarePredecessor(t4->getId(), t1->getId());
    graph.declarePredecessor(t1->getId(), t0->getId());

    taskSet.insert(t2->getId());
    taskSet.insert(t3->getId());
    taskSet.insert(t4->getId());

    graph.finalize();

    // Using getId() to make understanding the test results easier to understand
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_TRUE(taskSet.contains(task->getId()));
    }
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_TRUE(taskSet.contains(task->getId()));
    }
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_TRUE(taskSet.contains(task->getId()));
    }
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_EQ(task->getId(), t1->getId());
    }
    {
        auto task = graph.dequeue();
        task->execute();
        graph.taskComplete(task->getId());
        // Using getId() to make understanding the test results easier
        EXPECT_EQ(task->getId(), t0->getId());
    }

    EXPECT_TRUE(graph.queueEmpty());
    EXPECT_TRUE(graph.graphEmpty());
}