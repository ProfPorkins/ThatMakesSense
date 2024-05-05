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

#include "System.hpp"
#include "components/Camera.hpp"
#include "misc/math.hpp"
#include "misc/misc.hpp"

#include <functional>
#include <optional>
#include <queue>

namespace systems
{
    class Camera : public System
    {
        enum class Input : std::uint8_t
        {
            Up,
            Down,
            Left,
            Right,
            ZoomIn,
            ZoomOut
        };

      public:
        Camera(std::uint16_t levelWidth, std::uint16_t levelHeight, std::function<void(math::Vector2f diff)> notifyPan, std::function<void()> notifyZoom);

        void update(std::chrono::microseconds elapsedTime) override;
        void shutdown() override;

        auto getCamera() { return m_entities.begin()->second; }

      private:
        std::uint16_t m_levelWidth;
        std::uint16_t m_levelHeight;
        std::function<void(math::Vector2f diff)> m_notifyPan;
        std::function<void()> m_notifyZoom;
        std::queue<Input> m_inputDirection;
        std::uint8_t m_cameraMinRange;
        std::uint8_t m_cameraMaxRange;
        bool m_mouseCapture{ false };
        math::Point2f m_previousMousePoint{ 0, 0 };

        std::vector<std::uint32_t> m_keyboardInputHandlers;
        std::uint32_t m_mouseWheelHandlerId{ 0 };
        std::uint32_t m_mouseMovedHandlerId{ 0 };
        std::uint32_t m_mousePressedHandlerId{ 0 };
        std::uint32_t m_mouseReleasedHandlerId{ 0 };
        std::optional<std::uint32_t> m_controllerInputHandler;

        float m_axisUpDown{ 0 };
        float m_axisLeftRight{ 0 };
        float m_axisRightTrigger{ 0 };
        std::chrono::microseconds m_controllerInputWait{ 0 };

        void updateImpl();
        void autoZoomPan(const misc::HexCoord& before, const misc::HexCoord& after);

        void registerMouseInput();
        void registerKeyboardInput();
        void registerControllerInput();
        std::optional<Input> computeControllerDirection();
    };
} // namespace systems
