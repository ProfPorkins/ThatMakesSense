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
    class Challenge : public PolymorphicComparable<Component, Challenge>
    {
      public:
        static constexpr std::chrono::microseconds CHALLENGE_TRANSITION_TIME = misc::msTous(std::chrono::milliseconds(1000));
        static constexpr std::chrono::microseconds CHALLENGE_STEADY_TIME = misc::msTous(std::chrono::milliseconds(60000));

        enum class State
        {
            In,
            Steady
        };

        Challenge(std::string text) :
            m_state(State::In),
            m_text(text)
        {
        }
        // Used only for cloning the component
        Challenge(State state, std::string text, std::chrono::microseconds timeInState) :
            m_state(state),
            m_text(text),
            m_timeInState(timeInState)
        {
        }
        const std::string& get() { return m_text; }

        auto getTimeInState() { return m_timeInState; }
        void updateTimeInState(std::chrono::microseconds elapsedTime) { m_timeInState += elapsedTime; }
        auto getState() { return m_state; }
        void setState(State state)
        {
            m_state = state;
            m_timeInState = std::chrono::microseconds::zero();
        }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Challenge>(), std::make_unique<Challenge>(m_state, m_text, m_timeInState) };
        }

      private:
        State m_state;
        std::string m_text;
        std::chrono::microseconds m_timeInState{ 0 };
    };
} // namespace components
