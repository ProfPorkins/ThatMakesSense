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

#include "MenuView.hpp"
#include "UIFramework/Element.hpp"
#include "UIFramework/Selection.hpp"
#include "UIFramework/Text.hpp"
#include "services/Configuration.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <cstdint>
#include <vector>

namespace views
{
    // --------------------------------------------------------------
    //
    // Provides the Settings view, allowing the user to change
    // various configuration properties.
    //
    // --------------------------------------------------------------
    class SettingsControlsCamera : public MenuView
    {
      public:
        using MenuView::MenuView;
        virtual bool start() override;
        virtual void stop() override;

        virtual ViewState update(const std::chrono::microseconds elapsedTime, const std::chrono::system_clock::time_point now) override;
        virtual void render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime) override;

      protected:
        void navigateUp() override;
        void navigateDown() override;
        void navigateLeft() override;
        void navigateRight() override;
        void navigateBack() override;
        void navigateSelect() override;
        void onMouseMoved(math::Point2f point, const std::chrono::microseconds elapsedTime) override;
        void onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime) override;

      private:
        bool m_initialized{ false };
        ViewState m_nextState{ ViewState::SettingsControlsCamera };
        std::uint32_t m_keyboardHandlerId{ 0 };

        ui::Text m_selectCameraKey{ { true, 0, 0, "Press a key to select the input", Content::get<sf::Font>(content::KEY_FONT_SETTINGS), sf::Color::Yellow, sf::Color::Black, Configuration::get<std::uint8_t>(config::FONT_SETTINGS_MESSAGE_SIZE) } };

        std::vector<std::shared_ptr<ui::Element>> m_options;
        std::vector<std::shared_ptr<ui::Element>> m_headers;
        std::int8_t m_activeOption{ 0 };

        void addKeyboardCameraOptions();
        void addMouseCameraOptions();
        void onKeyReleased(sf::Keyboard::Key key);
    };
} // namespace views
