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

#include "ControllerInput.hpp"

#include <SFML/Window/Event.hpp>
#include <SFML/Window/Joystick.hpp>
#include <cassert>
#include <ranges>

// -----------------------------------------------------------------
//
// Using the Meyer's Singleton technique...this is thread safe
//
// -----------------------------------------------------------------
ControllerInput& ControllerInput::instance()
{
    static ControllerInput instance;
    return instance;
}

bool ControllerInput::initialize(std::shared_ptr<sf::RenderWindow> window)
{
    m_window = window;

    enumerateControllers();

    return true;
}

void ControllerInput::enumerateControllers()
{
    // Setup the mapping from joystick to vendor for the code later on that transforms
    // the events into standard joystick enumerations.
    for (unsigned int joystickId = 0; joystickId < sf::Joystick::Count; joystickId++)
    {
        m_joystickIdToVendorId[joystickId] = sf::Joystick::getIdentification(joystickId).vendorId;
    }
}

std::uint32_t ControllerInput::registerButtonDownHandler(Button button, bool repeat, std::chrono::microseconds rate, ButtonHandler handler)
{
    auto id = getNextId();
    m_handlersButtonDown[id] = { id, button, repeat, rate, handler };

    return id;
}

std::uint32_t ControllerInput::registerButtonDownHandler(Button button, ButtonHandler handler)
{
    auto id = getNextId();
    m_handlersButtonDown[id] = { id, button, false, std::chrono::microseconds::zero(), handler };

    return id;
}

std::uint32_t ControllerInput::registerAnyButtonPressedHandler(ButtonHandler handler)
{
    auto id = getNextId();
    m_handlersAnyButtonPressed[id] = { id, handler };

    return id;
}

std::uint32_t ControllerInput::registerAnyButtonReleasedHandler(ButtonHandler handler)
{
    auto id = getNextId();
    m_handlersAnyButtonReleased[id] = { id, handler };

    return id;
}

std::uint32_t ControllerInput::registerButtonPressedHandler(Button button, ButtonHandler handler)
{
    auto id = getNextId();
    m_handlersButtonPressed[id] = { id, button, handler };

    return id;
}

std::uint32_t ControllerInput::registerButtonReleasedHandler(Button button, ButtonHandler handler)
{
    auto id = getNextId();
    m_handlersButtonReleased[id] = { id, button, handler };

    return id;
}

std::uint32_t ControllerInput::registerAxisHandler(Axis axis, AxisHandler handler)
{
    auto id = getNextId();
    m_handlersAxis[id] = { id, axis, handler };

    return id;
}

std::uint32_t ControllerInput::registerAnyAxisHandler(AxisHandler handler)
{
    auto id = getNextId();
    m_handlersAnyAxis[id] = { id, handler };

    return id;
}

void ControllerInput::unregisterButtonDownHandler(std::uint32_t id)
{
    assert(m_handlersButtonDown.contains(id));
    m_handlersButtonDown.erase(id);
}

void ControllerInput::unregisterAnyButtonPressedHandler(std::uint32_t id)
{
    assert(m_handlersAnyButtonPressed.contains(id));
    m_handlersAnyButtonPressed.erase(id);
}

void ControllerInput::unregisterAnyButtonReleasedHandler(std::uint32_t id)
{
    assert(m_handlersAnyButtonReleased.contains(id));
    m_handlersAnyButtonReleased.erase(id);
}

void ControllerInput::unregisterButtonPressedHandler(std::uint32_t id)
{
    assert(m_handlersButtonPressed.contains(id));
    m_handlersButtonPressed.erase(id);
}

void ControllerInput::unregisterButtonReleasedHandler(std::uint32_t id)
{
    assert(m_handlersButtonReleased.contains(id));
    m_handlersButtonReleased.erase(id);
}

void ControllerInput::unregisterAxisHandler(std::uint32_t id)
{
    assert(m_handlersAxis.contains(id));
    m_handlersAxis.erase(id);
}

void ControllerInput::unregisterAnyAxisHandler(std::uint32_t id)
{
    assert(m_handlersAnyAxis.contains(id));
    m_handlersAnyAxis.erase(id);
}

void ControllerInput::signalButtonPressed(sf::Event::JoystickButtonEvent event)
{
    switch (m_joystickIdToVendorId[event.joystickId])
    {
        case VENDOR_ID_MICROSOFT:
            event.button = transformButtonXB1(event.button);
            m_whichControllerVendorUsed = VENDOR_ID_MICROSOFT;
            break;
        case VENDOR_ID_SONY:
            event.button = transformButtonPS4(event.button);
            m_whichControllerVendorUsed = VENDOR_ID_SONY;
            break;
    }

    m_buttonsPressed[static_cast<Button>(event.button)] = event;
    //
    // Because we can continuously receive the buttonpressed event, check to
    // see if we already have this property.  If we do, we don't want to
    // overwrite the value that already exists.
    if (!m_buttonRepeat.contains(static_cast<Button>(event.button)))
    {
        m_buttonRepeat[static_cast<Button>(event.button)] = false;
    }
}

void ControllerInput::signalButtonReleased(sf::Event::JoystickButtonEvent event)
{
    switch (m_joystickIdToVendorId[event.joystickId])
    {
        case VENDOR_ID_MICROSOFT:
            event.button = transformButtonXB1(event.button);
            m_whichControllerVendorUsed = VENDOR_ID_MICROSOFT;
            break;
        case VENDOR_ID_SONY:
            event.button = transformButtonPS4(event.button);
            m_whichControllerVendorUsed = VENDOR_ID_SONY;
            break;
    }

    m_buttonsPressed.erase(static_cast<Button>(event.button));
    m_buttonRepeat.erase(static_cast<Button>(event.button));

    m_buttonsReleased[static_cast<Button>(event.button)] = event;
}

void ControllerInput::signalJoystickMoved(sf::Event::JoystickMoveEvent event)
{
    //   Transform the vendor input into standardized input
    switch (m_joystickIdToVendorId[event.joystickId])
    {
        case VENDOR_ID_MICROSOFT:
            event = transformAxisXB1(event);
            m_whichControllerVendorUsed = VENDOR_ID_MICROSOFT;
            break;
        case VENDOR_ID_SONY:
            event = transformAxisPS4(event);
            m_whichControllerVendorUsed = VENDOR_ID_SONY;
            break;
    }

    m_axisEvents.push_back(event);
}

void ControllerInput::update(const std::chrono::microseconds elapsedTime)
{
    // If there are any events, disable the mouse cursor
    if (m_axisEvents.size() > 0 || m_buttonsPressed.size() > 0 || m_buttonsReleased.size() > 0)
    {
        m_window->setMouseCursorVisible(false);
        m_controllerUsed = true;
    }

    // Axis handlers
    for (auto&& event : m_axisEvents)
    {
        for (auto&& info : m_handlersAxis | std::views::values)
        {
            if (static_cast<std::uint8_t>(event.axis) == static_cast<std::uint8_t>(info.axis))
            {
                // Scale the position to [0, 1)
                info.handler(info.axis, event.position / 100.0f, elapsedTime);
            }
        }

        for (auto&& info : m_handlersAnyAxis | std::views::values)
        {
            info.handler(static_cast<ControllerInput::Axis>(event.axis), event.position / 100.0f, elapsedTime);
        }
    }
    m_axisEvents.clear();

    // Buttons down handlers
    for (auto&& button : m_buttonsPressed | std::views::keys)
    {
        // For this button, go through and update the state of all the 'button down' handlers
        for (auto&& info : m_handlersButtonDown | std::views::values)
        {
            if (info.button == button) // TODO: could turn this into a ranges filter using a lambda
            {
                info.elapsedTime += elapsedTime;
                if (info.repeat)
                {
                    // Check the rate vs elapsed time for this button before invoking the handler
                    if (info.elapsedTime >= info.rate)
                    {
                        info.handler(button, elapsedTime);
                        m_buttonRepeat[button] = true;
                        //
                        // Reset the elapsed time, leaving any extra time beyond the repeat rate
                        // that may have accumulated
                        info.elapsedTime -= info.rate;
                    }
                }
                else if (info.repeat == false && m_buttonRepeat[button] == false)
                {
                    info.handler(button, elapsedTime);
                    m_buttonRepeat[button] = true;
                }
            }
        }

        // "any button pressed" handlers
        for (auto&& info : m_handlersAnyButtonPressed | std::views::values)
        {
            info.handler(button, elapsedTime);
        }

        // "button pressed" handlers
        for (auto&& info : m_handlersButtonPressed | std::views::values)
        {
            if (info.button == button) // TODO: could turn this into a ranges filter using a lambda
            {
                if (!info.signaled)
                {
                    info.handler(button, elapsedTime);
                    info.signaled = true;
                }
            }
        }
    }

    //
    // button released handlers
    for (auto&& button : m_buttonsReleased | std::views::keys)
    {
        for (auto info : m_handlersAnyButtonReleased | std::views::values)
        {
            info.handler(button, elapsedTime);
        }

        // "button pressed" handlers
        for (auto&& info : m_handlersButtonReleased | std::views::values)
        {
            if (info.button == button) // TODO: could turn this into a ranges filter using a lambda
            {
                info.handler(button, elapsedTime);
            }
        }

        // Reset the signaled state of the button pressed handlers, so they'll
        // be signed when the button is pressed again.
        for (auto&& info : m_handlersButtonPressed | std::views::values)
        {
            if (info.button == button) // TODO: could turn this into a ranges filter using a lambda
            {
                info.signaled = false;
            }
        }
    }
    m_buttonsReleased.clear();
}

decltype(sf::Event::JoystickButtonEvent::button) ControllerInput::transformButtonXB1(const decltype(sf::Event::JoystickButtonEvent::button)& button)
{
    switch (button)
    {
        case static_cast<int>(ButtonXB1::A):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Bottom);
        case static_cast<int>(ButtonXB1::B):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Right);
        case static_cast<int>(ButtonXB1::X):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Left);
        case static_cast<int>(ButtonXB1::Y):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Top);
    }

    return button;
}

decltype(sf::Event::JoystickButtonEvent::button) ControllerInput::transformButtonPS4(const decltype(sf::Event::JoystickButtonEvent::button)& button)
{
    switch (button)
    {
        case static_cast<int>(ButtonPS4::X):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Bottom);
        case static_cast<int>(ButtonPS4::O):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Right);
        case static_cast<int>(ButtonPS4::Square):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Left);
        case static_cast<int>(ButtonPS4::Triangle):
            return static_cast<decltype(sf::Event::JoystickButtonEvent::button)>(Button::Top);
    }

    return button;
}

sf::Event::JoystickMoveEvent ControllerInput::transformAxisXB1(const sf::Event::JoystickMoveEvent& event)
{
    sf::Event::JoystickMoveEvent myEvent{ event };

    switch (event.axis)
    {
        case static_cast<int>(AxisXB1::Joystick1LeftRight):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick1LeftRight);
            break;
        case static_cast<int>(AxisXB1::Joystick1UpDown):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick1UpDown);
            break;
        case static_cast<int>(AxisXB1::Joystick2LeftRight):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick2LeftRight);
            break;
        case static_cast<int>(AxisXB1::Joystick2UpDown):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick2UpDown);
            break;

        case static_cast<int>(AxisXB1::DPadLeftRight):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::DPadLeftRight);
            break;
        case static_cast<int>(AxisXB1::DPadUpDown):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::DPadUpDown);
            break;

        case static_cast<int>(AxisXB1::TriggerLeft): // TriggerLeft & TriggerRight are the same value
        {
            // If the user quickly releases the right trigger, it can/will reset back to a very
            // small positive value, which would cause the code to think the left trigger had
            // been pressed.  To overcome this problem, the code below tests to see if the previous
            // position had been negative and now is positive, as a means to detect when the user
            // quickly releases the right trigger.
            static decltype(event.position) previousPosition = 0;
            if ((previousPosition < 0 && event.position > 0) || event.position < 0)
            {
                myEvent.position *= -1;
                myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::TriggerRight);
            }
            else
            {
                myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::TriggerLeft);
            }
            previousPosition = event.position;
        }
        break;

        default: // Placed here to eliminate a warning message
            break;
    }

    return myEvent;
}

sf::Event::JoystickMoveEvent ControllerInput::transformAxisPS4(const sf::Event::JoystickMoveEvent& event)
{
    sf::Event::JoystickMoveEvent myEvent{ event };

    switch (event.axis)
    {
        case static_cast<int>(AxisPS4::Joystick1LeftRight):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick1LeftRight);
            break;
        case static_cast<int>(AxisPS4::Joystick1UpDown):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick1UpDown);
            break;
        case static_cast<int>(AxisPS4::Joystick2LeftRight):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick2LeftRight);
            break;
        case static_cast<int>(AxisPS4::Joystick2UpDown):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::Joystick2UpDown);
            break;

        case static_cast<int>(AxisPS4::DPadLeftRight):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::DPadLeftRight);
            break;
        case static_cast<int>(AxisPS4::DPadUpDown):
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::DPadUpDown);
            break;

        case static_cast<int>(AxisPS4::TriggerLeft):
            myEvent.position = (event.position + 100) / 2;
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::TriggerLeft);
            break;
        case static_cast<int>(AxisPS4::TriggerRight):
            myEvent.position = (event.position + 100) / 2;
            myEvent.axis = static_cast<sf::Joystick::Axis>(Axis::TriggerRight);
            break;

        default: // Placed here to eliminate a warning message
            break;
    }

    return myEvent;
}
