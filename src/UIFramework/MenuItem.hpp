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

#include "Activate.hpp"
#include "Element.hpp"
#include "Text.hpp"

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <functional>
#include <memory>
#include <string>

namespace ui
{
    // --------------------------------------------------------------
    //
    // Represents a menu item.  A menu item can be selected by either
    // the keyboard or mouse (click).
    //
    // --------------------------------------------------------------
    class MenuItem : public Element, public Activate
    {
      public:
        class Settings : public Text::Settings
        {
          public:
            sf::Color activeColor;
            sf::Color activeOutline;
            std::function<void()> handler;
        };
        MenuItem(const Settings& settings);

        void start() override;
        void stop() override;

        void onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime) override;

        void render(sf::RenderTarget& renderTarget) override;
        void setRegion(const math::Rectangle2f& region) override;
        void setPosition(const math::Point2f& position) override;

        void setActive() override;
        void setInactive() override;
        bool isActive() override { return Activate::isActive(); }
        void select() { m_handler(); }

      private:
        Text m_inactiveLabel;
        Text m_activeLabel;
        Text* m_displayLabel;
        std::function<void()> m_handler;
        std::uint32_t m_handlerKeyboardId{ 0 };
        std::uint32_t m_handlerControllerId{ 0 };
    };
} // namespace ui