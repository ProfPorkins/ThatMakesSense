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

#include "AnimatedSprite.hpp"

#include "components/InputControlled.hpp"
#include "components/Object.hpp"
#include "services/ThreadPool.hpp"

#include <chrono>
#include <memory>

namespace systems
{
    void AnimatedSprite::update(const std::chrono::microseconds elapsedTime)
    {
        // auto task = ThreadPool::instance().createTask(
        //     [this, elapsedTime]()
        //     {
        //         this->updateImpl(elapsedTime);
        //     });
        // ThreadPool::instance().enqueueTask(task);
        this->updateImpl(elapsedTime);
    }

    void AnimatedSprite::updateImpl(std::chrono::microseconds elapsedTime)
    {
        for (auto&& [id, entity] : m_entities)
        {
            auto sprite = entity->getComponent<components::AnimatedSprite>();
            sprite->updateElapsedTime(elapsedTime);

            // While loop because it could be that more than one frame occurs since the
            // last update.  That would be bad, but it could be the case.
            while (sprite->getElapsedTime() >= sprite->getSpriteTime())
            {
                sprite->incrementSprite();
                sprite->updateElapsedTime(-sprite->getSpriteTime());
            }
        }
    }
} // namespace systems