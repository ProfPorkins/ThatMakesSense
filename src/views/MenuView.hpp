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

#include "UIFramework/Text.hpp"
#include "View.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <SFML/Graphics/Sprite.hpp>
#include <chrono>
#include <vector>

namespace views
{
    // --------------------------------------------------------------
    //
    // Provides a class from which all other "menu" type views should inherit.
    // It provides the background and game title all those views share.
    //
    // --------------------------------------------------------------
    class MenuView : public View
    {
      public:
        using View::View;
        virtual bool start() override;
        virtual void stop() override;

        ViewState update(const std::chrono::microseconds elapsedTime, const std::chrono::system_clock::time_point) override;
        void render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime) override;

      protected:
        virtual void navigateUp() {}
        virtual void navigateDown() {}
        virtual void navigateLeft() {}
        virtual void navigateRight() {}
        virtual void navigateBack() {}
        virtual void navigateSelect() {}
        virtual void onMouseMoved([[maybe_unused]] math::Point2f point, [[maybe_unused]] const std::chrono::microseconds elapsedTime) {}
        virtual void onMouseReleased([[maybe_unused]] sf::Mouse::Button button, [[maybe_unused]] math::Point2f point, [[maybe_unused]] const std::chrono::microseconds elapsedTime) {}

      private:
        bool m_initialized{ false };
        sf::Sprite m_background;
        ui::Text m_title{
            { true, 0, 0, misc::GAME_NAME,
              Content::get<sf::Font>(content::KEY_FONT_TITLE),
              Configuration::get<sf::Color>(config::FONT_TITLE_COLOR_FILL),
              Configuration::get<sf::Color>(config::FONT_TITLE_COLOR_OUTLINE),
              Configuration::get<std::uint8_t>(config::FONT_TITLE_SIZE) }
        };

        std::vector<std::uint32_t> m_keyboardHandlerIds;
        std::uint32_t m_mouseMovedHandlerId{ 0 };
        std::uint32_t m_mouseReleasedHandlerId{ 0 };
        std::uint32_t m_controllerDPadUpDownHandlerId{ 0 };
        std::uint32_t m_controllerDPadLeftRightHandlerId{ 0 };
        std::uint32_t m_controllerButtonBReleasedHandlerId{ 0 };
        std::uint32_t m_controllerButtonAReleasedHandlerId{ 0 };
        std::uint32_t m_controllerAxisHandlerId{ 0 };

        float m_axisLeftRight{ 0 };
        float m_axisUpDown{ 0 };
        std::chrono::microseconds m_controllerInputWait{ 0 };

        void registerKeyboardInputs();
        void registerMouseInputs();
        void registerControllerInputs();
    };
} // namespace views
