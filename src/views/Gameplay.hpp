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

#include "GameModel.hpp"
#include "View.hpp"
#include "ViewState.hpp"

#include <memory>

namespace views
{
    // --------------------------------------------------------------
    //
    // This is pretty much a simple pass-through to the GameModel.
    //
    // --------------------------------------------------------------
    class Gameplay : public View
    {
      public:
        virtual bool start() override;
        virtual void stop() override;

        virtual ViewState update(const std::chrono::microseconds elapsedTime, const std::chrono::system_clock::time_point now) override;
        virtual void render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime) override;

      private:
        enum class CompleteState
        {
            None,
            Stay,
            NextLevel
        };
        ViewState m_nextState{ ViewState::GamePlay };
        std::unique_ptr<GameModel> m_model{ nullptr };
        bool m_levelComplete{ false };
        CompleteState m_completeState{ CompleteState::None };
        std::string m_levelUUID;

        std::uint32_t m_keyExitHandlerId{ 0 };
        std::uint32_t m_keyStayHandlerId{ 0 };
        std::uint32_t m_keyNextHandlerId{ 0 };
        std::uint32_t m_controllerExitHandlerId{ 0 };
        std::uint32_t m_controllerStayHandlerId{ 0 };
        std::uint32_t m_controllerNextHandlerId{ 0 };
    };
} // namespace views
