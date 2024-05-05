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
#include <functional>
#include <memory>
#include <string>

namespace ui
{
    // --------------------------------------------------------------
    //
    // Represents a single keyboard character that can be configured
    // by the user.
    //
    // --------------------------------------------------------------
    class KeyboardOption : public Element, public Activate
    {
      public:
        class Settings
        {
          public:
            float centerX;
            float top;
            const std::string label;
            const std::string key;
            std::shared_ptr<sf::Font> font;
            sf::Color color;
            sf::Color outlineColor;
            sf::Color activeColor;
            sf::Color activeOutline;
            sf::Color selectionColor;
            sf::Color selectionOutline;
            unsigned int fontSize;
            std::function<void()> onStart;
            std::function<void(std::string)> onComplete;
        };
        KeyboardOption(const Settings& settings);

        virtual void onKeyPressed(sf::Keyboard::Key key) override;
        virtual void onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime) override;
        virtual void render(sf::RenderTarget& renderTarget) override;

        virtual void setActive() override;
        virtual void setInactive() override;
        virtual bool isActive() override { return Activate::isActive(); }

        void setKey(std::string key)
        {
            m_key.setText(key);
            // Recenter
            m_key.setPosition({ m_region.getCenter().x - m_key.getRegion().width / 2.0f,
                                m_region.top + m_inactiveLabel.getRegion().height * 1.5f });
        }

      private:
        bool m_expectingInput{ false };
        Text m_inactiveLabel;
        Text m_activeLabel;
        Text* m_displayLabel;
        Text m_key;
        std::function<void()> m_onStart;
        std::function<void(std::string)> m_onComplete;

        void acceptInput(const std::string key);
    };
} // namespace ui