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

#include "Component.hpp"
#include "misc/misc.hpp"

#include <chrono>
#include <string>
#include <utility>

namespace components
{
    class Hint : public PolymorphicComparable<Component, Hint>
    {
      public:
        static constexpr std::chrono::microseconds HINT_TRANSITION_TIME = misc::msTous(std::chrono::milliseconds(1000));
        static constexpr std::chrono::microseconds HINT_MIN_TIME = HINT_TRANSITION_TIME * 4; // 2 seconds for transitions, 2 seconds for primary display

        enum class State
        {
            In,
            Steady,
            Out
        };

        Hint(std::string text, std::chrono::microseconds timeLeft, bool clearExisting = false) :
            m_state(State::In),
            m_text(text),
            m_clearExisting(clearExisting),
            m_timeLeft(std::max(timeLeft, HINT_MIN_TIME)),
            m_active(false)
        {
            // Require hints to be at least 4 seconds
            //   1 second to transition in
            //   2 seconds at full size
            //   1 second to transition out
        }

        const std::string& get() { return m_text; }
        bool getClearExisting() { return m_clearExisting; }
        auto getTimeLeft() { return m_timeLeft; }
        void updateTimeLeft(std::chrono::microseconds elapsedTime) { m_timeLeft -= elapsedTime; }

        auto getTimeInState() { return m_timeInState; }
        void updateTimeInState(std::chrono::microseconds elapsedTime) { m_timeInState += elapsedTime; }
        void resetTimeInState() { m_timeInState = std::chrono::microseconds::zero(); }
        auto getState() { return m_state; }
        void setState(State state) { m_state = state; }

        bool isActive() { return m_active; }
        void setActive() { m_active = true; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Hint>(), std::make_unique<Hint>(m_text, m_timeLeft) };
        }

      private:
        State m_state;
        std::string m_text;
        bool m_clearExisting{ false };
        std::chrono::microseconds m_timeLeft{ 0 };
        std::chrono::microseconds m_timeInState{ 0 };
        bool m_active{ false };
    };
} // namespace components
