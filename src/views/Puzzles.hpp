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
#include "UIFramework/MenuItem.hpp"
#include "UIFramework/Text.hpp"
#include "services/Configuration.hpp"
#include "services/Content.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <tuple>
#include <vector>

namespace views
{
    // --------------------------------------------------------------
    //
    // Provides the ability to choose which puzzle to play, along with
    // the status of met challenges
    //
    // --------------------------------------------------------------
    class Puzzles : public MenuView
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
        void navigateBack() override;
        void onMouseMoved(math::Point2f point, const std::chrono::microseconds elapsedTime) override;
        void onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime) override;
        void onMouseWheel(sf::Event::MouseWheelScrollEvent event);

      private:
        bool m_initialized{ false };
        ViewState m_nextState{ ViewState::Puzzles };

        ui::Text m_headerPuzzles{
            { true, 0, 0,
              "Puzzles",
              Content::get<sf::Font>(content::KEY_FONT_CHALLENGES),
              Configuration::get<sf::Color>(config::FONT_CHALLENGES_HEADER_COLOR_FILL),
              Configuration::get<sf::Color>(config::FONT_CHALLENGES_HEADER_COLOR_OUTLINE),
              Configuration::get<std::uint8_t>(config::FONT_CHALLENGES_HEADER_SIZE) }
        };
        ui::Text m_headerChallenges{
            { true, 0, 0,
              "Challenges",
              Content::get<sf::Font>(content::KEY_FONT_CHALLENGES),
              Configuration::get<sf::Color>(config::FONT_CHALLENGES_HEADER_COLOR_FILL),
              Configuration::get<sf::Color>(config::FONT_CHALLENGES_HEADER_COLOR_OUTLINE),
              Configuration::get<std::uint8_t>(config::FONT_CHALLENGES_HEADER_SIZE) }
        };
        ui::Text m_headerDetail{
            { true, 0, 0,
              "Challenges Detail",
              Content::get<sf::Font>(content::KEY_FONT_CHALLENGES),
              Configuration::get<sf::Color>(config::FONT_CHALLENGES_HEADER_COLOR_FILL),
              Configuration::get<sf::Color>(config::FONT_CHALLENGES_HEADER_COLOR_OUTLINE),
              Configuration::get<std::uint8_t>(config::FONT_CHALLENGES_HEADER_SIZE) }
        };
        sf::RectangleShape m_boxDetail;

        std::uint32_t m_mouseWheelHandlerId{ 0 };
        std::uint8_t m_visibleStart{ 0 };
        std::uint8_t m_visibleCount{ 0 };
        std::int8_t m_activeLevel{ 0 }; // This needs to be signed, because it can temporarily go negative

        std::vector<std::shared_ptr<ui::MenuItem>> m_menuItems;
        std::vector<std::vector<std::tuple<bool, bool>>> m_statusChallenges; // <pre-defined, status>
        std::vector<std::vector<std::shared_ptr<ui::Text>>> m_textChallenges;
        sf::Sprite m_boxEmpty;
        sf::Sprite m_boxFilled;
        sf::Sprite m_boxFilledBC;

        void constructLevelItems(math::Dimension2f& coords, float& top, sf::Text& alphabetItem);
        void repositionLevelItems();
        void initializeSprites();
        void registerInputHanlders();

        static constexpr auto OFFSET_TOP = 0.30f;
        static constexpr auto OFFSET_BOTTOM = 0.15f;
        static constexpr auto OFFSET_PUZZLES_LEFT = 0.05f;
        static constexpr auto OFFSET_HEADER_PUZZLES = 0.10f;
        static constexpr auto OFFSET_HEADER_CHALLENGES = 0.39f;
        static constexpr auto OFFSET_DETAILBOX_LEFT = 0.17f;
        static constexpr auto OFFSET_DETAILBOX_RIGHT = 0.08f;
        static constexpr auto OFFSET_HEADER_DETAIL = 0.1f + (OFFSET_DETAILBOX_LEFT + OFFSET_DETAILBOX_RIGHT) / 2.0f;
    };
} // namespace views
