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

#include "misc/math.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <chrono>
#include <functional>
#include <string>
#include <unordered_map>
#include <vector>

// --------------------------------------------------------------
//
// Controller input handler
//
// Note: This is a Singleton
//
// --------------------------------------------------------------
class ControllerInput
{
  public:
    enum class Axis : std::uint8_t // The type is sf::Joystick::Axis, which is an enumeration, and can't be used as a type here
    {
        Joystick1LeftRight = 0, // Values: -100 to 100
        Joystick1UpDown = 1,    // Values: -100 to 100
        Joystick2LeftRight = 2, // Values: -100 to 100
        Joystick2UpDown = 3,    // Values: -100 to 100

        DPadLeftRight = 4, // Left Values: 0, -100, Right Values: 0, 100
        DPadUpDown = 5,    // Up Values: 0, 100, Down Values: 0, -100

        TriggerLeft = 6,  // Values: 0 to 100
        TriggerRight = 7, // Values: 0 to 100
    };

    enum class Button : decltype(sf::Event::JoystickButtonEvent::button)
    {
        Bottom = 0,
        Right = 1,
        Left = 2,
        Top = 3,

        LeftJoystick = 8,
        RightJoystick = 9,

        LeftBumper = 4,
        RightBumper = 5
    };

    static const auto VENDOR_ID_UNKNOWN = 0;
    static const auto VENDOR_ID_MICROSOFT = 1118;
    static const auto VENDOR_ID_SONY = 1356;

    using AxisHandler = std::function<void(Axis, float, const std::chrono::microseconds)>;
    using ButtonHandler = std::function<void(Button, const std::chrono::microseconds)>;

    ControllerInput(const ControllerInput&) = delete;
    ControllerInput(ControllerInput&&) = delete;
    ControllerInput& operator=(const ControllerInput&) = delete;
    ControllerInput& operator=(ControllerInput&&) = delete;

    static ControllerInput& instance();
    bool initialize(std::shared_ptr<sf::RenderWindow> window);
    void enumerateControllers();

    std::uint32_t registerButtonDownHandler(Button button, bool repeat, std::chrono::microseconds rate, ButtonHandler handler);
    std::uint32_t registerButtonDownHandler(Button button, ButtonHandler handler);
    std::uint32_t registerAnyButtonPressedHandler(ButtonHandler handler);
    std::uint32_t registerAnyButtonReleasedHandler(ButtonHandler handler);
    std::uint32_t registerButtonPressedHandler(Button button, ButtonHandler handler);
    std::uint32_t registerButtonReleasedHandler(Button button, ButtonHandler handler);
    std::uint32_t registerAxisHandler(Axis axis, AxisHandler handler);
    std::uint32_t registerAnyAxisHandler(AxisHandler handler);

    void unregisterButtonDownHandler(std::uint32_t id);
    void unregisterAnyButtonPressedHandler(std::uint32_t id);
    void unregisterAnyButtonReleasedHandler(std::uint32_t id);
    void unregisterButtonPressedHandler(std::uint32_t id);
    void unregisterButtonReleasedHandler(std::uint32_t id);
    void unregisterAxisHandler(std::uint32_t id);
    void unregisterAnyAxisHandler(std::uint32_t id);

    void signalButtonPressed(sf::Event::JoystickButtonEvent event);
    void signalButtonReleased(sf::Event::JoystickButtonEvent event);
    void signalJoystickMoved(sf::Event::JoystickMoveEvent event);

    bool hasControllerBeenUsed() { return m_controllerUsed; }
    int whichControllerVendorUsed() { return m_whichControllerVendorUsed; }

    void update(const std::chrono::microseconds elapsedTime);

  private:
    ControllerInput() {}

    struct ButtonInfo
    {
        ButtonInfo() = default;

        ButtonInfo(std::uint32_t id, Button button, bool repeat, std::chrono::microseconds rate, ButtonHandler handler) :
            id(id),
            button(button),
            repeat(repeat),
            rate(rate),
            elapsedTime(rate), // Set the elapsedTime to the 'rate' so that it is ready to fire immediately
            handler(handler)
        {
        }

        std::uint32_t id{};
        Button button{};
        bool repeat{ false };
        std::chrono::microseconds rate{ 0 };
        std::chrono::microseconds elapsedTime{ 0 };
        ButtonHandler handler{ nullptr };
    };

    struct AnyButtonInfo
    {
        AnyButtonInfo() = default;

        AnyButtonInfo(std::uint32_t id, ButtonHandler handler) :
            id(id),
            handler(handler)
        {
        }

        std::uint32_t id;
        ButtonHandler handler{ nullptr };
        bool signaled{ false };
    };

    struct ButtonPressedInfo
    {
        ButtonPressedInfo() = default;

        ButtonPressedInfo(std::uint32_t id, Button button, ButtonHandler handler) :
            id(id),
            button(button),
            handler(handler)
        {
        }

        std::uint32_t id{};
        Button button{};
        ButtonHandler handler{ nullptr };
        bool signaled{ false };
    };

    struct AxisInfo
    {
        AxisInfo() = default;

        AxisInfo(std::uint32_t id, Axis axis, AxisHandler handler) :
            id(id),
            axis(axis),
            handler(handler)
        {
        }

        std::uint32_t id;
        Axis axis;
        AxisHandler handler;
    };

    struct AnyAxisInfo
    {
        AnyAxisInfo() = default;

        AnyAxisInfo(std::uint32_t id, AxisHandler handler) :
            id(id),
            handler(handler)
        {
        }

        std::uint32_t id;
        AxisHandler handler;
    };

    enum class ButtonXB1 : decltype(sf::Event::JoystickButtonEvent::button)
    {
        A = 0,
        B = 1,
        X = 2,
        Y = 3,

        LeftJoystick = 8,
        RightJoystick = 9,

        LeftBumper = 4,
        RightBumper = 5
    };

    enum class ButtonPS4 : decltype(sf::Event::JoystickButtonEvent::button)
    {
        X = 1,
        O = 2,
        Square = 0,
        Triangle = 3,

        LeftBumper = 4,
        RightBumper = 5,

        LeftJoystick = 10,
        RightJoystick = 11
    };

    enum class AxisXB1 : std::uint8_t
    {
        Joystick1LeftRight = 0, // Values: -100 to 100
        Joystick1UpDown = 1,    // Values: -100 to 100

        Joystick2LeftRight = 4, // Values: -100 to 100
        Joystick2UpDown = 5,    // Values: -100 to 100

        TriggerLeft = 2,  // Values: 0 to 100
        TriggerRight = 2, // Values: 0 to -100

        DPadLeftRight = 6, // Left Values: 0, -100, Right Values: 0, 100
        DPadUpDown = 7     // Up Values: 0, 100, Down Values: 0, -100
    };

    enum class AxisPS4 : std::uint8_t
    {
        Joystick1LeftRight = 0,
        Joystick1UpDown = 1,

        Joystick2LeftRight = 2,
        Joystick2UpDown = 3,

        TriggerLeft = 4,  // Values: -100 to 100
        TriggerRight = 5, // Values: -100 to 100

        DPadLeftRight = 6, // Left Values: 0, -100, Right Values: 0, 100
        DPadUpDown = 7     // Up Values: 0, 100, Down Values: 0, -100
    };

    std::unordered_map<unsigned int, decltype(sf::Joystick::Identification::vendorId)> m_joystickIdToVendorId;

    std::shared_ptr<sf::RenderWindow> m_window;
    bool m_controllerUsed{ false };
    int m_whichControllerVendorUsed{ VENDOR_ID_UNKNOWN };
    std::uint32_t nextId{ 0 };
    std::unordered_map<Button, sf::Event::JoystickButtonEvent> m_buttonsPressed;
    std::unordered_map<Button, sf::Event::JoystickButtonEvent> m_buttonsReleased;
    std::vector<sf::Event::JoystickMoveEvent> m_axisEvents;

    std::unordered_map<Button, bool> m_buttonRepeat;

    std::unordered_map<std::uint32_t, ButtonInfo> m_handlersButtonDown;
    std::unordered_map<std::uint32_t, AnyButtonInfo> m_handlersAnyButtonPressed;
    std::unordered_map<std::uint32_t, AnyButtonInfo> m_handlersAnyButtonReleased;
    std::unordered_map<std::uint32_t, ButtonPressedInfo> m_handlersButtonPressed;
    std::unordered_map<std::uint32_t, ButtonPressedInfo> m_handlersButtonReleased;
    std::unordered_map<std::uint32_t, AxisInfo> m_handlersAxis;
    std::unordered_map<std::uint32_t, AnyAxisInfo> m_handlersAnyAxis;

    auto getNextId() { return nextId++; }

    decltype(sf::Event::JoystickButtonEvent::button) transformButtonXB1(const decltype(sf::Event::JoystickButtonEvent::button)& button);
    decltype(sf::Event::JoystickButtonEvent::button) transformButtonPS4(const decltype(sf::Event::JoystickButtonEvent::button)& button);

    sf::Event::JoystickMoveEvent transformAxisXB1(const sf::Event::JoystickMoveEvent& event);
    sf::Event::JoystickMoveEvent transformAxisPS4(const sf::Event::JoystickMoveEvent& event);
};
