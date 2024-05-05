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

#include "MenuView.hpp"

#include "services/ControllerInput.hpp"
#include "services/KeyboardInput.hpp"
#include "services/MouseInput.hpp"

namespace views
{
    bool MenuView::start()
    {
        if (!m_initialized)
        {
            //
            // Prepare the background image for rendering
            // Note: Reason for testing if it is there is because I have been going
            //       back and forth on whether to have one or not.  Right now the
            //       answer is not, but just in case I add one in the future, the code
            //       is here and ready to go.
            if (Content::has<sf::Texture>(content::KEY_IMAGE_MENU_BACKGROUND))
            {
                auto texture = Content::get<sf::Texture>(content::KEY_IMAGE_MENU_BACKGROUND);
                m_background.setTexture(*texture);
                m_background.setOrigin({ texture->getSize().x / 2.0f, texture->getSize().y / 2.0f });
                m_background.setPosition({ 0.0f, 0.0f });
                auto aspectRatio = static_cast<float>(Configuration::getGraphics().getResolution().width) / Configuration::getGraphics().getResolution().height;
                if (aspectRatio >= 1.0)
                {
                    m_background.setScale({ Configuration::getGraphics().getViewCoordinates().width / texture->getSize().x,
                                            Configuration::getGraphics().getViewCoordinates().width / texture->getSize().y });
                }
                else
                {
                    // Try to make it look decent for vertical resolutions
                    m_background.setScale({ Configuration::getGraphics().getViewCoordinates().width / texture->getSize().x,
                                            Configuration::getGraphics().getViewCoordinates().height / texture->getSize().y });
                }
            }

            //
            // Offset the game title a little from the top of the view
            m_title.setPosition({ -(m_title.getRegion().width / 2.0f),
                                  -(Configuration::getGraphics().getViewCoordinates().height / 2.0f) + m_title.getRegion().height * 0.5f });

            m_initialized = true;
        }

        registerKeyboardInputs();
        registerMouseInputs();
        registerControllerInputs();

        return true;
    }

    void MenuView::stop()
    {
        for (auto&& id : m_keyboardHandlerIds)
        {
            KeyboardInput::instance().unregisterKeyDownHandler(id);
        }
        m_keyboardHandlerIds.clear();

        MouseInput::instance().unregisterMouseMovedHandler(m_mouseMovedHandlerId);
        MouseInput::instance().unregisterMouseReleasedHandler(m_mouseReleasedHandlerId);

        ControllerInput::instance().unregisterAxisHandler(m_controllerDPadUpDownHandlerId);
        ControllerInput::instance().unregisterAxisHandler(m_controllerDPadLeftRightHandlerId);
        ControllerInput::instance().unregisterButtonReleasedHandler(m_controllerButtonBReleasedHandlerId);
        ControllerInput::instance().unregisterButtonReleasedHandler(m_controllerButtonAReleasedHandlerId);
        ControllerInput::instance().unregisterAnyAxisHandler(m_controllerAxisHandlerId);
    }

    ViewState MenuView::update(const std::chrono::microseconds elapsedTime, const std::chrono::system_clock::time_point)
    {
        static const auto CONTROLLER_INPUT_WAIT = misc::msTous(std::chrono::milliseconds(Configuration::get<std::uint16_t>(config::CONTROLLER_REPEAT_DELAY)));

        static const auto SENSITIVITY_UPDOWN = 0.5f;
        static const auto SENSITIVITY_LEFTRIGHT = 0.6f;

        m_controllerInputWait -= elapsedTime;
        if (m_controllerInputWait < std::chrono::microseconds::zero())
        {
            if (m_axisUpDown < -SENSITIVITY_UPDOWN)
            {
                navigateUp();
            }
            else if (m_axisUpDown > SENSITIVITY_UPDOWN)
            {
                navigateDown();
            }

            if (m_axisLeftRight > SENSITIVITY_LEFTRIGHT)
            {
                navigateRight();
            }
            else if (m_axisLeftRight < -SENSITIVITY_LEFTRIGHT)
            {
                navigateLeft();
            }

            m_controllerInputWait = CONTROLLER_INPUT_WAIT;
        }

        return ViewState::Undefined;
    }

    void MenuView::render(sf::RenderTarget& renderTarget, [[maybe_unused]] const std::chrono::microseconds elapsedTime)
    {
        renderTarget.clear(sf::Color::Black);
        // renderTarget.draw(m_background);

        m_title.render(renderTarget);
    }

    void MenuView::registerKeyboardInputs()
    {
        static const auto KEYBOARD_REPEAT_DELAY = misc::msTous(std::chrono::milliseconds(Configuration::get<std::uint16_t>(config::KEYBOARD_REPEAT_DELAY)));
        auto id = KeyboardInput::instance().registerKeyDownHandler(
            "up", KeyboardInput::Modifier::None,
            true,
            KEYBOARD_REPEAT_DELAY,
            [this](std::chrono::microseconds)
            {
                navigateUp();
            });
        m_keyboardHandlerIds.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            "down", KeyboardInput::Modifier::None,
            true,
            KEYBOARD_REPEAT_DELAY,
            [this](std::chrono::microseconds)
            {
                navigateDown();
            });
        m_keyboardHandlerIds.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            "left", KeyboardInput::Modifier::None,
            false,
            std::chrono::microseconds::zero(),
            [this](std::chrono::microseconds)
            {
                navigateLeft();
            });
        m_keyboardHandlerIds.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            "right", KeyboardInput::Modifier::None,
            false,
            std::chrono::microseconds::zero(),
            [this](std::chrono::microseconds)
            {
                navigateRight();
            });
        m_keyboardHandlerIds.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            "enter", KeyboardInput::Modifier::None,
            false,
            std::chrono::microseconds::zero(),
            [this](std::chrono::microseconds)
            {
                navigateSelect();
            });
        m_keyboardHandlerIds.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            "escape", KeyboardInput::Modifier::None,
            false,
            std::chrono::microseconds::zero(),
            [this](std::chrono::microseconds)
            {
                navigateBack();
            });
        m_keyboardHandlerIds.push_back(id);
    }

    void MenuView::registerMouseInputs()
    {

        //
        // Get the mouse inputs registered
        m_mouseMovedHandlerId = MouseInput::instance().registerMouseMovedHandler(
            [this](math::Point2f point, const std::chrono::microseconds elapsedTime)
            {
                onMouseMoved(point, elapsedTime);
            });
        m_mouseReleasedHandlerId = MouseInput::instance().registerMouseReleasedHandler(
            [this](sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime)
            {
                onMouseReleased(button, point, elapsedTime);
            });
    }

    void MenuView::registerControllerInputs()
    {

        //
        // Get the controller inputs registered
        m_controllerDPadUpDownHandlerId = ControllerInput::instance().registerAxisHandler(
            ControllerInput::Axis::DPadUpDown,
            [this](ControllerInput::Axis, float position, const std::chrono::microseconds)
            {
                // Testing for > < 0 because = 0 is an event we don't want to handle, because that
                // happens as the d-pad releases
                if (position > 0)
                {
                    navigateUp();
                }
                else if (position < 0)
                {
                    navigateDown();
                }
            });

        m_controllerDPadLeftRightHandlerId = ControllerInput::instance().registerAxisHandler(
            ControllerInput::Axis::DPadLeftRight,
            [this](ControllerInput::Axis, float position, const std::chrono::microseconds)
            {
                // Testing for > < 0 because = 0 is an event we don't want to handle, because that
                // happens as the d-pad releases
                if (position > 0)
                {
                    navigateRight();
                }
                else if (position < 0)
                {
                    navigateLeft();
                }
            });

        //
        // Get the controller inputs registered
        m_controllerButtonBReleasedHandlerId = ControllerInput::instance().registerButtonReleasedHandler(
            ControllerInput::Button::Right,
            [this](ControllerInput::Button, const std::chrono::microseconds)
            {
                navigateBack();
            });

        m_controllerButtonAReleasedHandlerId = ControllerInput::instance().registerButtonReleasedHandler(
            ControllerInput::Button::Bottom,
            [this](ControllerInput::Button, const std::chrono::microseconds)
            {
                navigateSelect();
            });

        m_controllerAxisHandlerId = ControllerInput::instance().registerAnyAxisHandler(
            [this](ControllerInput::Axis axis, float position, const std::chrono::microseconds)
            {
                switch (axis)
                {
                    case ControllerInput::Axis::Joystick1UpDown:
                    case ControllerInput::Axis::Joystick2UpDown:
                        m_axisUpDown = position;
                        break;
                    case ControllerInput::Axis::Joystick1LeftRight:
                    case ControllerInput::Axis::Joystick2LeftRight:
                        m_axisLeftRight = position;
                        break;
                    default: // Placed here to prevent a compiler warning
                        break;
                }
            });
    }

} // namespace views
