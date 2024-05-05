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

#include "KeyboardInput.hpp"

#include <cassert>
#include <ranges>

// -----------------------------------------------------------------
//
// Using the Meyer's Singleton technique...this is thread safe
//
// -----------------------------------------------------------------
KeyboardInput& KeyboardInput::instance()
{
    static KeyboardInput instance;
    return instance;
}

bool KeyboardInput::initialize()
{
    initializeStringToKeyMapping();

    return true;
}

std::uint32_t KeyboardInput::registerKeyDownHandler(std::string key, Modifier modifier, bool repeat, std::chrono::microseconds rate, std::function<void(std::chrono::microseconds)> handler)
{
    auto id = getNextId();

    m_handlersKeyDown[id] = { id, m_stringToKey[key], modifier, repeat, rate, handler };

    //
    // We set the initial elapsed time to the repeat rate so that the handler
    // is invoked immediately upon the key being pressed.
    m_handlersKeyDown[id].elapsedTime = rate;

    return id;
}

std::uint32_t KeyboardInput::registerKeyDownHandler(std::string key, Modifier modifier, std::function<void(std::chrono::microseconds)> handler)
{
    auto id = getNextId();
    m_handlersKeyDown[id] = { id, m_stringToKey[key], modifier, false, std::chrono::microseconds::zero(), handler };

    return id;
}

std::uint32_t KeyboardInput::registerAnyKeyPressedHandler(std::function<void(sf::Keyboard::Key)> handler)
{
    auto id = getNextId();
    m_handlersAnyPressed[id] = handler;

    return id;
}

std::uint32_t KeyboardInput::registerKeyPressedHandler(std::string key, std::function<void()> handler)
{
    auto id = getNextId();
    m_handlersPressed[id] = { id, m_stringToKey[key], handler };

    return id;
}

std::uint32_t KeyboardInput::registerAnyKeyReleasedHandler(std::function<void(sf::Keyboard::Key)> handler)
{
    auto id = getNextId();
    m_handlersAnyReleased[id] = handler;

    return id;
}

std::uint32_t KeyboardInput::registerKeyReleasedHandler(std::string key, std::function<void()> handler)
{
    auto id = getNextId();
    m_handlersReleased[id] = { id, m_stringToKey[key], handler };

    return id;
}

void KeyboardInput::unregisterKeyDownHandler(std::uint32_t id)
{
    assert(m_handlersKeyDown.contains(id));
    m_handlersKeyDown.erase(id);
}

void KeyboardInput::unregisterAnyKeyPressedHandler(std::uint32_t id)
{
    assert(m_handlersAnyPressed.contains(id));
    m_handlersAnyPressed.erase(id);
}

void KeyboardInput::unregisterKeyPressedHandler(std::uint32_t id)
{
    assert(m_handlersPressed.contains(id));
    m_handlersPressed.erase(id);
}

void KeyboardInput::unregisterAnyKeyReleasedHandler(std::uint32_t id)
{
    assert(m_handlersAnyReleased.contains(id));
    m_handlersAnyReleased.erase(id);
}

void KeyboardInput::unregisterKeyReleasedHandler(std::uint32_t id)
{
    assert(m_handlersReleased.contains(id));
    m_handlersReleased.erase(id);
}

void KeyboardInput::signalKeyPressed(sf::Event::KeyEvent event, [[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
{
    m_keysPressed[event.code] = event;
    //
    // Because we can continuously receive the keypressed event, check to
    // see if we already have this property.  If we do, we don't want to
    // overwrite the value that already exists.
    if (!m_keyRepeat.contains(event.code))
    {
        m_keyRepeat[event.code] = false;
    }
}

void KeyboardInput::signalKeyReleased(sf::Event::KeyEvent event, [[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
{
    m_keysPressed.erase(event.code);
    m_keyRepeat.erase(event.code);

    for (auto&& info : m_handlersKeyDown | std::views::values | std::views::filter([&](const auto& info)
                                                                                   {
                                                                                       return info.key == event.code;
                                                                                   }))
    {
        info.elapsedTime = info.rate;
    }

    m_keysReleased[event.code] = event;
}

void KeyboardInput::update(const std::chrono::microseconds elapsedTime)
{
    // General input handlers
    for (auto&& [key, event] : m_keysPressed)
    {
        auto hasModifier = [&](const auto& info)
        {
            bool handle{ false };
            if (info.key == key)
            {
                handle = false;
                // If the key and no shift modifier specified and no shift key pressed
                if (!(static_cast<std::uint8_t>(info.modifier) & static_cast<std::uint8_t>(Modifier::Shift)) && !event.shift)
                {
                    handle = true;
                }
                if (static_cast<std::uint8_t>(info.modifier) & static_cast<std::uint8_t>(Modifier::Shift) && event.shift)
                {
                    handle = true;
                }

                // If the key and no ctrl modifier specified and no ctrl key pressed
                if ((static_cast<std::uint8_t>(info.modifier) & static_cast<std::uint8_t>(Modifier::Ctrl)) && !event.control)
                {
                    handle = true;
                }
                if (static_cast<std::uint8_t>(info.modifier) & static_cast<std::uint8_t>(Modifier::Ctrl) && event.control)
                {
                    handle = true;
                }

                // If the key and no alt modifier specified and no alt key pressed
                if (!(static_cast<std::uint8_t>(info.modifier) ^ static_cast<std::uint8_t>(Modifier::Alt)) && !event.alt)
                {
                    handle = true;
                }
                if (static_cast<std::uint8_t>(info.modifier) & static_cast<std::uint8_t>(Modifier::Alt) && event.alt)
                {
                    handle = true;
                }
            }
            return handle;
        };

        for (auto&& info : m_handlersKeyDown | std::views::values | std::views::filter(hasModifier))
        {
            info.elapsedTime += elapsedTime;
            if (info.repeat == true)
            {
                //
                // Check the rate vs elapsed time for this key before invoking the handler
                if (info.elapsedTime >= info.rate)
                {
                    info.handler(elapsedTime);
                    m_keyRepeat[key] = true;
                    //
                    // Reset the elapsed time, leaving any extra time beyond the repeat rate
                    // that may have accumulated
                    info.elapsedTime -= info.rate;
                }
            }
            else if (info.repeat == false && m_keyRepeat[key] == false)
            {
                info.handler(elapsedTime);
                m_keyRepeat[key] = true;
            }
        }

        //
        // "any key pressed" handlers
        for (auto&& handler : m_handlersAnyPressed)
        {
            handler.second.handler(key);
        }

        //
        // "keypressed" handlers
        for (auto&& info : m_handlersPressed | std::views::values)
        {
            if (info.key == key) // TODO: could turn this into a ranges filter using a lambda
            {
                info.handler();
                info.signaled = true;
            }
        }
    }
    // Do not do the line of code below.  We *want* to keep the list of pressed
    // keys around to allow during every update, while the keys are pressed, they
    // are recognized as pressed.
    // m_keysPressed.clear();

    //
    // Key released handlers
    for (auto&& [key, event] : m_keysReleased)
    {
        //
        // "any key released" handlers
        for (auto&& handler : m_handlersAnyReleased)
        {
            handler.second(key);
        }

        for (auto&& info : m_handlersReleased | std::views::values)
        {
            if (info.key == key) // TODO: could turn this into a ranges filter using a lambda
            {
                info.handler();
            }
        }

        //
        // If there is a matching "keypressed" handler, reset it's signaled state
        for (auto&& info : m_handlersPressed | std::views::values)
        {
            if (info.key == key) // TODO: could turn this into a ranges filter using a lambda
            {
                info.signaled = false;
            }
        }
    }
    m_keysReleased.clear();
}

void KeyboardInput::initializeStringToKeyMapping()
{
    m_stringToKey["up"] = sf::Keyboard::Up;
    m_stringToKey["down"] = sf::Keyboard::Down;
    m_stringToKey["left"] = sf::Keyboard::Left;
    m_stringToKey["right"] = sf::Keyboard::Right;
    m_stringToKey["space"] = sf::Keyboard::Space;
    m_stringToKey["enter"] = sf::Keyboard::Enter;
    m_stringToKey["escape"] = sf::Keyboard::Escape;
    m_stringToKey["page-up"] = sf::Keyboard::PageUp;
    m_stringToKey["page-down"] = sf::Keyboard::PageDown;
    m_stringToKey["home"] = sf::Keyboard::Home;
    m_stringToKey["end"] = sf::Keyboard::End;

    m_stringToKey["comma"] = sf::Keyboard::Comma;
    m_stringToKey["period"] = sf::Keyboard::Period;
    m_stringToKey["semicolon"] = sf::Keyboard::Semicolon;
    m_stringToKey["quote"] = sf::Keyboard::Quote;
    m_stringToKey["backslash"] = sf::Keyboard::Backslash;
    m_stringToKey["forwardslash"] = sf::Keyboard::Slash;

    m_stringToKey["a"] = sf::Keyboard::A;
    m_stringToKey["b"] = sf::Keyboard::B;
    m_stringToKey["c"] = sf::Keyboard::C;
    m_stringToKey["d"] = sf::Keyboard::D;
    m_stringToKey["e"] = sf::Keyboard::E;
    m_stringToKey["f"] = sf::Keyboard::F;
    m_stringToKey["g"] = sf::Keyboard::G;
    m_stringToKey["h"] = sf::Keyboard::H;
    m_stringToKey["i"] = sf::Keyboard::I;
    m_stringToKey["j"] = sf::Keyboard::J;
    m_stringToKey["k"] = sf::Keyboard::K;
    m_stringToKey["l"] = sf::Keyboard::L;
    m_stringToKey["m"] = sf::Keyboard::M;
    m_stringToKey["n"] = sf::Keyboard::N;
    m_stringToKey["o"] = sf::Keyboard::O;
    m_stringToKey["p"] = sf::Keyboard::P;
    m_stringToKey["q"] = sf::Keyboard::Q;
    m_stringToKey["r"] = sf::Keyboard::R;
    m_stringToKey["s"] = sf::Keyboard::S;
    m_stringToKey["t"] = sf::Keyboard::T;
    m_stringToKey["u"] = sf::Keyboard::U;
    m_stringToKey["v"] = sf::Keyboard::V;
    m_stringToKey["w"] = sf::Keyboard::W;
    m_stringToKey["x"] = sf::Keyboard::X;
    m_stringToKey["y"] = sf::Keyboard::Y;
    m_stringToKey["z"] = sf::Keyboard::Z;
}
