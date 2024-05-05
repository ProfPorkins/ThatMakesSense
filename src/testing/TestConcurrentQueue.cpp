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

#include "../services/concurrency/ConcurrentQueue.hpp"

#include <gtest/gtest.h>
#include <string>

TEST(ConcurrentQueue, Integerals)
{
    auto queue = ConcurrentQueue<int>();

    EXPECT_FALSE(queue.dequeue().has_value());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);
    queue.enqueue(4);
    queue.enqueue(5);
    queue.enqueue(6);

    EXPECT_EQ(queue.dequeue().value(), 1);
    EXPECT_EQ(queue.dequeue().value(), 2);
    EXPECT_EQ(queue.dequeue().value(), 3);
    EXPECT_EQ(queue.dequeue().value(), 4);
    EXPECT_EQ(queue.dequeue().value(), 5);
    EXPECT_EQ(queue.dequeue().value(), 6);

    EXPECT_FALSE(queue.dequeue().has_value());
}

TEST(ConcurrentQueue, Integerals_MultiStage)
{
    auto queue = ConcurrentQueue<int>();

    EXPECT_FALSE(queue.dequeue().has_value());

    queue.enqueue(1);
    queue.enqueue(2);
    queue.enqueue(3);

    auto value = queue.dequeue();
    EXPECT_TRUE(value);

    EXPECT_EQ(queue.dequeue().value(), 2);
    EXPECT_EQ(queue.dequeue().value(), 3);
    EXPECT_FALSE(queue.dequeue().has_value());

    queue.enqueue(4);
    queue.enqueue(5);
    queue.enqueue(6);

    EXPECT_EQ(queue.dequeue().value(), 4);
    EXPECT_EQ(queue.dequeue().value(), 5);
    EXPECT_EQ(queue.dequeue().value(), 6);

    EXPECT_FALSE(queue.dequeue().has_value());
}

TEST(ConcurrentQueue, Strings)
{
    using namespace std::string_literals;

    auto queue = ConcurrentQueue<std::string>();

    EXPECT_FALSE(queue.dequeue().has_value());

    queue.enqueue("1"s);
    queue.enqueue("2"s);
    queue.enqueue("3"s);
    queue.enqueue("4"s);
    queue.enqueue("5"s);
    queue.enqueue("6"s);

    EXPECT_EQ(queue.dequeue().value(), "1"s);
    EXPECT_EQ(queue.dequeue().value(), "2"s);
    EXPECT_EQ(queue.dequeue().value(), "3"s);
    EXPECT_EQ(queue.dequeue().value(), "4"s);
    EXPECT_EQ(queue.dequeue().value(), "5"s);
    EXPECT_EQ(queue.dequeue().value(), "6"s);

    EXPECT_FALSE(queue.dequeue().has_value());
}
