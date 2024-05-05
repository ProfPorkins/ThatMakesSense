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
#include <cstdint>
#include <functional>
#include <memory>
#include <string>
#include <tuple>
#include <vector>

namespace ui
{
    // --------------------------------------------------------------
    //
    // Represents an option item.  An option item allows the user
    // to select from a set of possible values.
    //
    // NOTE: I tried VERY HARD to have the tuple be <std::string, std::any> to
    //       allow the actual option data to come in and be reported back out
    //       by the handler.  It compiles and works perfectly using the MSVC
    //       compiler.  However, the g++ compiler refused to compile it
    //       saying the type for std::any must be copy constructable...which is WAS!!
    //       Not being able to do this causes some copied data to occur
    //       when creating the handlers for this class.  It isn't the end
    //       of the world, but I was trying to avoid it.
    //
    // --------------------------------------------------------------
    class Selection : public Element, public Activate
    {
      public:
        class Settings : public Text::Settings
        {
          public:
            sf::Color activeColor;
            sf::Color activeOutline;
            sf::Color selectionColor;
            sf::Color selectionOutline;
            std::function<void(std::uint8_t)> handler;
            const std::vector<std::tuple<std::string, std::uint8_t>> options;
            std::size_t selectedOption;
        };
        Selection(const Settings& settings);

        virtual void onKeyPressed(sf::Keyboard::Key key) override;
        virtual void onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime) override;

        virtual void render(sf::RenderTarget& renderTarget) override;

        virtual void setActive() override;
        virtual void setInactive() override;
        virtual bool isActive() override { return Activate::isActive(); }

      private:
        std::vector<std::tuple<std::string, std::uint8_t>> m_options;
        std::size_t m_selectedOption;
        std::function<void(std::uint8_t)> m_handler;
        Text m_inactiveLabel;
        Text m_activeLabel;
        Text* m_displayLabel;
        Text m_displayOption;

        void selectLeft();
        void selectRight();
        void updateSelectionText();
    };

} // namespace ui