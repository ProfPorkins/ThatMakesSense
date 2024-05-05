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

#include <SFML/Window/Event.hpp>
#include <chrono>
#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>

// --------------------------------------------------------------
//
// Keyboard input handler
//
// Note: This is a Singleton
//
// --------------------------------------------------------------
class KeyboardInput
{
  public:
    enum class Modifier : std::uint8_t
    {
        // clang-format off
          None =    0b00000000,
          Shift =   0b00000001,
          Ctrl =    0b00000010,
          Alt =     0b00000100
        // clang-format on
    };

    KeyboardInput(const KeyboardInput&) = delete;
    KeyboardInput(KeyboardInput&&) = delete;
    KeyboardInput& operator=(const KeyboardInput&) = delete;
    KeyboardInput& operator=(KeyboardInput&&) = delete;

    static KeyboardInput& instance();
    bool initialize();

    std::uint32_t registerKeyDownHandler(std::string key, Modifier modifier, bool repeat, std::chrono::microseconds rate, std::function<void(std::chrono::microseconds)> handler);
    std::uint32_t registerKeyDownHandler(std::string key, Modifier modifier, std::function<void(std::chrono::microseconds)> handler);
    std::uint32_t registerAnyKeyPressedHandler(std::function<void(sf::Keyboard::Key)> handler);
    std::uint32_t registerKeyPressedHandler(std::string key, std::function<void()> handler);
    std::uint32_t registerAnyKeyReleasedHandler(std::function<void(sf::Keyboard::Key)> handler);
    std::uint32_t registerKeyReleasedHandler(std::string key, std::function<void()> handler);

    void unregisterKeyDownHandler(std::uint32_t id);
    void unregisterKeyPressedHandler(std::uint32_t id);
    void unregisterAnyKeyPressedHandler(std::uint32_t id);
    void unregisterKeyReleasedHandler(std::uint32_t id);
    void unregisterAnyKeyReleasedHandler(std::uint32_t id);

    void signalKeyPressed(sf::Event::KeyEvent event, const std::chrono::microseconds elapsedTime, const std::chrono::system_clock::time_point now);
    void signalKeyReleased(sf::Event::KeyEvent event, const std::chrono::microseconds elapsedTime, const std::chrono::system_clock::time_point now);
    void update(const std::chrono::microseconds elapsedTime);

  private:
    KeyboardInput() {}
    struct KeyDownInfo
    {
        KeyDownInfo() = default;

        KeyDownInfo(std::uint32_t id, sf::Keyboard::Key key, Modifier modifier, bool repeat, std::chrono::microseconds rate, std::function<void(std::chrono::microseconds)> handler) :
            id(id),
            key(key),
            modifier(modifier),
            repeat(repeat),
            rate(rate),
            elapsedTime(rate), // Set the elapsedTime to the 'rate' so that it is ready to fire immediately
            handler(handler)
        {
        }

        std::uint32_t id;
        sf::Keyboard::Key key;
        Modifier modifier{ Modifier::None };
        bool repeat{ false };
        std::chrono::microseconds rate{ 0 };
        std::chrono::microseconds elapsedTime{ 0 };
        std::function<void(std::chrono::microseconds)> handler{ nullptr };
    };

    struct KeyPressedInfo
    {
        KeyPressedInfo() = default;

        KeyPressedInfo(std::uint32_t id, sf::Keyboard::Key key, std::function<void()> handler) :
            id(id),
            key(key),
            handler(handler)
        {
        }

        std::uint32_t id;
        sf::Keyboard::Key key;
        bool signaled{ false };
        std::function<void()> handler{ nullptr };
    };

    struct AnyKeyPressedInfo
    {
        AnyKeyPressedInfo() = default;

        AnyKeyPressedInfo(std::function<void(sf::Keyboard::Key)> handler) :
            handler(handler)
        {
        }

        bool signaled{ false };
        std::function<void(sf::Keyboard::Key)> handler{ nullptr };
    };

    std::uint32_t nextId{ 0 };

    std::unordered_map<std::string, sf::Keyboard::Key> m_stringToKey;
    std::unordered_map<sf::Keyboard::Key, sf::Event::KeyEvent> m_keysPressed;
    std::unordered_map<sf::Keyboard::Key, sf::Event::KeyEvent> m_keysReleased;
    std::unordered_map<sf::Keyboard::Key, bool> m_keyRepeat;

    std::unordered_map<std::uint32_t, KeyDownInfo> m_handlersKeyDown;
    std::unordered_map<std::uint32_t, KeyPressedInfo> m_handlersPressed;
    std::unordered_map<std::uint32_t, AnyKeyPressedInfo> m_handlersAnyPressed;
    std::unordered_map<std::uint32_t, KeyPressedInfo> m_handlersReleased;
    std::unordered_map<std::uint32_t, std::function<void(sf::Keyboard::Key)>> m_handlersAnyReleased;

    void initializeStringToKeyMapping();
    auto getNextId() { return nextId++; }
};
