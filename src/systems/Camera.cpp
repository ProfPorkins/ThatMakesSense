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

#include "Camera.hpp"

#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/ControllerInput.hpp"
#include "services/KeyboardInput.hpp"
#include "services/MouseInput.hpp"

#include <algorithm> // std::max

namespace systems
{
    Camera::Camera(std::uint16_t levelWidth, std::uint16_t levelHeight, std::function<void(math::Vector2f diff)> notifyPan, std::function<void()> notifyZoom) :
        System({ ctti::unnamed_type_id<components::Camera>() }),
        m_levelWidth(levelWidth),
        m_levelHeight(levelHeight),
        m_notifyPan(notifyPan),
        m_notifyZoom(notifyZoom),
        m_cameraMinRange(Configuration::get<std::uint8_t>(config::CAMERA_RANGE_MIN)),
        m_cameraMaxRange(Configuration::get<std::uint8_t>(config::CAMERA_RANGE_MAX))
    {
        registerKeyboardInput();
        registerMouseInput();
        registerControllerInput();
    }

    void Camera::update([[maybe_unused]] const std::chrono::microseconds elapsedTime)
    {
        static const auto CONTROLLER_REPEAT_DELAY = misc::msTous(std::chrono::milliseconds(Configuration::get<std::uint16_t>(config::CONTROLLER_REPEAT_DELAY)));

        // Before handling camera movement, update input direction based on state of the joystick
        m_controllerInputWait -= elapsedTime;
        if (m_controllerInputWait < std::chrono::microseconds::zero())
        {
            auto direction = computeControllerDirection();
            if (direction.has_value())
            {
                m_inputDirection.push(direction.value());
            }
            m_controllerInputWait = CONTROLLER_REPEAT_DELAY;
        }

        this->updateImpl();
    }

    void Camera::shutdown()
    {
        for (auto&& id : m_keyboardInputHandlers)
        {
            KeyboardInput::instance().unregisterKeyDownHandler(id);
        }

        MouseInput::instance().unregisterMouseWheelHandler(m_mouseWheelHandlerId);
        MouseInput::instance().unregisterMouseMovedHandler(m_mouseMovedHandlerId);
        MouseInput::instance().unregisterMousePressedHandler(m_mousePressedHandlerId);
        MouseInput::instance().unregisterMouseReleasedHandler(m_mouseReleasedHandlerId);

        if (m_controllerInputHandler.has_value())
        {
            ControllerInput::instance().unregisterAnyAxisHandler(m_controllerInputHandler.value());
            m_controllerInputHandler.reset();
        }
    }

    void Camera::updateImpl()
    {
        // If there is more than one camera, too bad, only grabbing the first one...and there better be one
        auto&& [id, entity] = *m_entities.begin();
        auto camera = entity->getComponent<components::Camera>();
        auto originalCenter = camera->getCenter();

        while (!m_inputDirection.empty())
        {
            auto input = m_inputDirection.front();
            m_inputDirection.pop();

            switch (input)
            {
                case Input::Up:
                {
                    misc::HexCoord center = camera->getCenter();
                    center.r = static_cast<decltype(center.r)>(std::max(0, center.r - 1));
                    camera->setCenter(center);
                }
                break;
                case Input::Down:
                {
                    misc::HexCoord center = camera->getCenter();
                    center.r = static_cast<decltype(center.r)>(std::min(static_cast<int>(m_levelHeight - 1), center.r + 1));
                    camera->setCenter(center);
                }
                break;
                case Input::Left:
                {
                    misc::HexCoord center = camera->getCenter();
                    center.q = static_cast<decltype(center.q)>(std::max(0, center.q - 1));
                    camera->setCenter(center);
                }
                break;
                case Input::Right:
                {
                    misc::HexCoord center = camera->getCenter();
                    center.q = static_cast<decltype(center.q)>(std::min(static_cast<int>(m_levelWidth - 1), center.q + 1));
                    camera->setCenter(center);
                }
                break;
                case Input::ZoomIn:
                {
                    //{
                    //    // NOTE: Leaving this code here because I may come back to it again, but I could never get
                    //    // the pan adjustment to be as smooth as it should be.

                    //    // Let's see what hex cell we started under
                    //    auto cameraCenter = getCamera()->getComponent<components::Camera>()->getCenter();
                    //    auto size = misc::computeCellSize(getCamera()->getComponent<components::Camera>());
                    //    auto cellBefore = misc::HexCoord::pointToHex({ static_cast<float>(m_previousMousePoint.x), static_cast<float>(m_previousMousePoint.y) }, size, cameraCenter);

                    //    // Doing it this way because std::max doesn't work without doing a bunch of static_casts and that looks
                    //    // crazy messy.
                    //    std::uint8_t range{ m_cameraMinRange };
                    //    if (camera->getRange() - 1 >= m_cameraMinRange)
                    //    {
                    //        range = camera->getRange() - 1;
                    //    }
                    //    camera->setRange(range);

                    //    // Then let's see where we ended up
                    //    size = misc::computeCellSize(getCamera()->getComponent<components::Camera>());
                    //    auto cellAfter = misc::HexCoord::pointToHex({ static_cast<float>(m_previousMousePoint.x), static_cast<float>(m_previousMousePoint.y) }, size, cameraCenter);

                    //    autoZoomPan(cellBefore, cellAfter);
                    //}

                    // Doing it this way because std::max doesn't work without doing a bunch of static_casts and that looks
                    // crazy messy.
                    std::uint8_t range{ m_cameraMinRange };
                    if (camera->getRange() - 1 >= m_cameraMinRange)
                    {
                        range = camera->getRange() - 1;
                    }
                    camera->setRange(range);
                    m_notifyZoom();
                }
                break;
                case Input::ZoomOut:
                {
                    //{
                    //    // NOTE: Leaving this code here because I may come back to it again, but I could never get
                    //    // the pan adjustment to be as smooth as it should be.
                    //    //
                    //    // Let's see what hex cell we started under
                    //    auto cameraCenter = getCamera()->getComponent<components::Camera>()->getCenter();
                    //    auto size = misc::computeCellSize(getCamera()->getComponent<components::Camera>());
                    //    auto cellBefore = misc::HexCoord::pointToHex({ static_cast<float>(m_previousMousePoint.x), static_cast<float>(m_previousMousePoint.y) }, size, cameraCenter);

                    //    auto range = std::min(m_cameraMaxRange, static_cast<decltype(camera->getRange())>(camera->getRange() + 1));
                    //    camera->setRange(range);

                    //    // Then let's see where we ended up
                    //    size = misc::computeCellSize(getCamera()->getComponent<components::Camera>());
                    //    auto cellAfter = misc::HexCoord::pointToHex({ static_cast<float>(m_previousMousePoint.x), static_cast<float>(m_previousMousePoint.y) }, size, cameraCenter);

                    //    // Pan the camera based on how much we moved
                    //    autoZoomPan(cellBefore, cellAfter);
                    //}

                    auto range = std::min(m_cameraMaxRange, static_cast<decltype(camera->getRange())>(camera->getRange() + 1));
                    camera->setRange(range);
                    m_notifyZoom();
                }
                break;
            }
        }

        auto cellSize = misc::computeCellSize(this->getCamera()->getComponent<components::Camera>());
        math::Vector2f cellDiff{ 0, 0 };

        cellDiff.x += (originalCenter.q - camera->getCenter().q) * cellSize.x;
        cellDiff.y += (originalCenter.r - camera->getCenter().r) * cellSize.y;
        if (cellDiff.x != 0 || cellDiff.y != 0)
        {
            m_notifyPan(cellDiff);
        }
    }

    // --------------------------------------------------------------
    //
    // This method is used to pan the camera based on mouse wheel
    // scrolling in order to center the zoom on the hex cell the
    // mouse was originally over.
    //
    // --------------------------------------------------------------
    void Camera::autoZoomPan([[maybe_unused]] const misc::HexCoord& before, [[maybe_unused]] const misc::HexCoord& after)
    {
        misc::HexCoord after2 = after;

        // Pan the camera based on how much we moved
        auto diffR = after.r - before.r;
        if (diffR != 0)
        {
            m_inputDirection.push((diffR > 0) ? Input::Up : Input::Down);
            updateImpl();

            auto cameraCenter = getCamera()->getComponent<components::Camera>()->getCenter();
            auto size = misc::computeCellSize(getCamera()->getComponent<components::Camera>());
            after2 = misc::HexCoord::pointToHex({ static_cast<float>(m_previousMousePoint.x), static_cast<float>(m_previousMousePoint.y) }, size, cameraCenter);
        }

        auto diffQ = after2.q - before.q;
        if (diffQ != 0)
        {
            m_inputDirection.push((diffQ > 0) ? Input::Left : Input::Right);
            updateImpl();
        }
    }

    void Camera::registerKeyboardInput()
    {
        auto repeatDelay = Configuration::get<std::uint16_t>(config::KEYBOARD_REPEAT_DELAY);

        auto id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_UP),
            KeyboardInput::Modifier::None,
            true,
            misc::msTous(std::chrono::milliseconds(repeatDelay)),
            [this](std::chrono::microseconds)
            {
                m_inputDirection.push(Input::Up);
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_DOWN),
            KeyboardInput::Modifier::None,
            true,
            misc::msTous(std::chrono::milliseconds(repeatDelay)),
            [this](std::chrono::microseconds)
            {
                m_inputDirection.push(Input::Down);
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_LEFT),
            KeyboardInput::Modifier::None,
            true,
            misc::msTous(std::chrono::milliseconds(repeatDelay)),
            [this](std::chrono::microseconds)
            {
                m_inputDirection.push(Input::Left);
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_RIGHT),
            KeyboardInput::Modifier::None,
            true,
            misc::msTous(std::chrono::milliseconds(repeatDelay)),
            [this](std::chrono::microseconds)
            {
                m_inputDirection.push(Input::Right);
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_ZOOM_IN),
            KeyboardInput::Modifier::None,
            true,
            misc::msTous(std::chrono::milliseconds(repeatDelay)),
            [this](std::chrono::microseconds)
            {
                m_inputDirection.push(Input::ZoomIn);
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_ZOOM_OUT),
            KeyboardInput::Modifier::None,
            true,
            misc::msTous(std::chrono::milliseconds(repeatDelay)),
            [this](std::chrono::microseconds)
            {
                m_inputDirection.push(Input::ZoomOut);
            });
        m_keyboardInputHandlers.push_back(id);
    }

    void Camera::registerMouseInput()
    {
        m_mouseWheelHandlerId = MouseInput::instance().registerMouseWheelHandler(
            [this](sf::Event::MouseWheelScrollEvent event, const std::chrono::microseconds)
            {
                /*auto cameraCenter = getCamera()->getComponent<components::Camera>()->getCenter();
                auto size = computeCellSize();
                auto cell = misc::pointToHex({ static_cast<float>(event.x), static_cast<float>(event.y) }, size, cameraCenter);
                std::cout << fmt::format("cell[{0}, {1}] : point({2}, {3})", cell.q, cell.r, event.x, event.y) << std::endl;*/

                // Delta can be 0, therefore need to explicitly test for greater and less than for
                // the zoom to work correctly
                if (event.delta > 0)
                {
                    m_inputDirection.push(Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_ZOOM) ? Input::ZoomIn : Input::ZoomOut);
                }
                else if (event.delta < 0)
                {
                    m_inputDirection.push(Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_ZOOM) ? Input::ZoomOut : Input::ZoomIn);
                }
            });

        m_mouseMovedHandlerId = MouseInput::instance().registerMouseMovedHandler(
            [this](math::Point2f point, const std::chrono::microseconds)
            {
                if (m_mouseCapture)
                {
                    // I know it would be more efficient to compute when the camera is first
                    // added, then only when zoom occurs.  But, this is not performance sensitive code
                    // and I've decided to not do that.
                    auto cellSize = misc::computeCellSize(this->getCamera()->getComponent<components::Camera>());
                    auto vector = math::vector(m_previousMousePoint, point);

                    bool panX{ false };
                    bool panY{ false };

                    while (vector.x > cellSize.x)
                    {
                        m_inputDirection.push(Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_HORZ) ? Input::Left : Input::Right);
                        vector.x -= cellSize.x;
                        panX = true;
                    }
                    while (vector.x < -cellSize.x)
                    {
                        m_inputDirection.push(Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_HORZ) ? Input::Right : Input::Left);
                        vector.x += cellSize.x;
                        panX = true;
                    }
                    while (vector.y > cellSize.y)
                    {
                        m_inputDirection.push(Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_VERT) ? Input::Up : Input::Down);
                        vector.y -= cellSize.y;
                        panY = true;
                    }
                    while (vector.y < -cellSize.y)
                    {
                        m_inputDirection.push(Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_VERT) ? Input::Down : Input::Up);
                        vector.y += cellSize.y;
                        panY = true;
                    }

                    // We don't change the x, y values until after they individually meet the criteria.
                    // In this way, the distance accumulates in each direction so the pan can take
                    // place as the user would expect.
                    //
                    // Adding back in the leftover vector components to account for the unaccounted
                    // for parts of the mouse movement.
                    m_previousMousePoint = {
                        panX ? point.x + vector.x : m_previousMousePoint.x,
                        panY ? point.y + vector.y : m_previousMousePoint.y
                    };
                }
                else
                {
                    // This is needed to support centering while zooming using the mouse
                    m_previousMousePoint = point;
                }
            });

        m_mousePressedHandlerId = MouseInput::instance().registerMousePressedHandler(
            [this](sf::Mouse::Button, math::Point2f point, const std::chrono::microseconds)
            {
                m_mouseCapture = true;
                m_previousMousePoint = point;
            });
        m_mouseReleasedHandlerId = MouseInput::instance().registerMouseReleasedHandler(
            [this](sf::Mouse::Button, math::Point2f, const std::chrono::microseconds)
            {
                m_mouseCapture = false;
            });
    }

    void Camera::registerControllerInput()
    {
        m_controllerInputHandler = ControllerInput::instance().registerAnyAxisHandler(
            [this](ControllerInput::Axis axis, float position, const std::chrono::microseconds)
            {
                switch (axis)
                {
                    case ControllerInput::Axis::Joystick2UpDown:
                        m_axisUpDown = position;
                        break;
                    case ControllerInput::Axis::Joystick2LeftRight:
                        m_axisLeftRight = position;
                        break;
                    case ControllerInput::Axis::TriggerRight:
                        m_axisRightTrigger = position;
                        break;
                    default: // This is here to prevent a compiler warning
                        break;
                }
            });
    }

    // --------------------------------------------------------------
    //
    // Based on the current position of the joystick, determine the
    // camera movement direction (if any)
    //
    // --------------------------------------------------------------
    std::optional<Camera::Input> Camera::computeControllerDirection()
    {
        static const auto SENSITIVITY_GENERAL = 0.2f;

        std::optional<Camera::Input> direction = std::nullopt;
        if (m_axisRightTrigger < SENSITIVITY_GENERAL)
        { // Pan
            if (m_axisUpDown < -SENSITIVITY_GENERAL && std::abs(m_axisUpDown) > std::abs(m_axisLeftRight))
            {
                direction = Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_VERT) ? Input::Up : Input::Down;
            }
            if (m_axisUpDown > SENSITIVITY_GENERAL && std::abs(m_axisUpDown) > std::abs(m_axisLeftRight))
            {
                direction = Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_VERT) ? Input::Down : Input::Up;
            }

            if (m_axisLeftRight < -SENSITIVITY_GENERAL && std::abs(m_axisLeftRight) > std::abs(m_axisUpDown))
            {
                direction = Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_HORZ) ? Input::Left : Input::Right;
            }
            if (m_axisLeftRight > SENSITIVITY_GENERAL && std::abs(m_axisLeftRight) > std::abs(m_axisUpDown))
            {
                direction = Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_HORZ) ? Input::Right : Input::Left;
            }
        }
        else // Zoom
        {
            if (m_axisUpDown > SENSITIVITY_GENERAL)
            {
                direction = Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_ZOOM) ? Input::ZoomOut : Input::ZoomIn;
            }
            if (m_axisUpDown < -SENSITIVITY_GENERAL)
            {
                direction = Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_ZOOM) ? Input::ZoomIn : Input::ZoomOut;
            }
        }

        return direction;
    }
} // namespace systems