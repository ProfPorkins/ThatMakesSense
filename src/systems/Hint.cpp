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

#include "Hint.hpp"

namespace systems
{
    void Hint::clear()
    {
        System::clear();
        // Have to also dump the queue of hints
        m_hints.clear();
    }

    bool Hint::addEntity(entities::EntityPtr entity)
    {
        bool added{ false };
        if (System::addEntity(entity))
        {
            added = true;

            if (entity->getComponent<components::Hint>()->getClearExisting())
            {
                while (m_hints.size() > 1)
                {
                    m_hints.pop_back();
                }
                if (m_hints.size() == 1)
                {
                    auto hint = m_hints.front()->getComponent<components::Hint>();
                    hint->resetTimeInState();
                    hint->setState(components::Hint::State::Out);
                    auto timeLeft = hint->getTimeLeft();
                    hint->updateTimeLeft(timeLeft - components::Hint::HINT_TRANSITION_TIME);
                }
            }

            m_hints.push_back(entity);
            // If this is the first one in the queue, set it to active
            // This is simply done by setting whatever is at the front of the queue to be active
            m_hints.front()->getComponent<components::Hint>()->setActive();
            m_notifyUpdated(m_hints.front()->getId());
        }

        return added;
    }

    void Hint::update([[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        if (!m_hints.empty())
        {
            auto hint = m_hints.front()->getComponent<components::Hint>();
            hint->updateTimeLeft(elapsedTime);

            // If this hint has expired, then remove it from the queue and set the next
            // one to active, also send a notification this entity should be removed
            if (hint->getTimeLeft() < std::chrono::microseconds::zero())
            {
                m_notifyRemove(m_hints.front()->getId());

                m_hints.pop_front();
                if (m_hints.size() > 0)
                {
                    m_hints.front()->getComponent<components::Hint>()->setActive();
                    m_notifyUpdated(m_hints.front()->getId());
                }
            }
            else
            {
                // Because it is still active, the state needs to be updated
                hint->updateTimeInState(elapsedTime);
                switch (hint->getState())
                {
                    case components::Hint::State::In:
                        if (hint->getTimeInState() >= components::Hint::HINT_TRANSITION_TIME)
                        {
                            hint->setState(components::Hint::State::Steady);
                            hint->resetTimeInState();
                        }
                        break;
                    case components::Hint::State::Steady:
                        if (hint->getTimeLeft() <= components::Hint::HINT_TRANSITION_TIME)
                        {
                            hint->setState(components::Hint::State::Out);
                            hint->resetTimeInState();
                        }
                        break;
                    case components::Hint::State::Out:
                        // Nothing to do, purposely left blank
                        break;
                }
            }
        }
    }

    void Hint::shutdown()
    {
        m_hints.clear();
    }

} // namespace systems
