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

#include "About.hpp"

#include "services/Configuration.hpp"

namespace views
{
    bool About::start()
    {
        MenuView::start();
        m_nextState = ViewState::About;

        // We just put a left of 0, because it gets centered later on
        ui::Text::Settings settingsVersion{
            true, 0,
            -0.40f * Configuration::getGraphics().getViewCoordinates().height,
            "version 1.07",
            Content::get<sf::Font>(content::KEY_FONT_TITLE),
            Configuration::get<sf::Color>(config::FONT_TITLE_COLOR_FILL),
            std::nullopt,
            static_cast<std::uint8_t>(0.35 * Configuration::get<std::uint8_t>(config::FONT_TITLE_SIZE))
        };
        m_version = std::make_shared<ui::Text>(settingsVersion);

        //
        // Wipe everything out and reset the story every time
        m_items.clear();

        auto top = (0.50f * Configuration::getGraphics().getViewCoordinates().height);

        top = addItem("That Makes Sense is a puzzle game where you define the rules", top);
        top = addItem("...yes, like that other game you know...", top);
        top = addItem("", top);
        top = addItem("Play takes place on a hex-based grid", top);
        top = addItem("allowing phrases to be formed in interesting patterns", top);
        top = addItem("right, left, up, down, diagonal, wandering, meandering, compact, branching...", top);
        top = addItem("", top);
        top = addItem("Puzzles may be large, use the camera to zoom and pan", top);
        top = addItem("", top);
        top = addItem("Puzzles have multiple challenges", top);
        top = addItem("find approaches to create, combine, and change phrases", top);
        top = addItem("to meet each puzzle's challenges", top);
        top = addItem("", top);
        top = addItem("Test yourself to see if you can solve puzzles in", top);
        top = addItem("creative ways, beyond the preset challenges!", top);

        //
        // Go through and center everything
        for (auto&& item : m_items)
        {
            item->setPosition({ -item->getRegion().width / 2.0f, item->getRegion().top });
        }

        return true;
    }

    void About::stop()
    {
        MenuView::stop();
    }

    ViewState About::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        static const auto MOVE_RATE_US = Configuration::getGraphics().getViewCoordinates().height / 20000000;
        static const auto LAST_ITEM_POSITION_EXIT = -40;

        MenuView::update(elapsedTime, now);

        // Scroll the text up vertically over time
        for (auto&& item : m_items)
        {
            item->setPosition({ item->getRegion().left, item->getRegion().top - MOVE_RATE_US * elapsedTime.count() });
        }

        // Check to see if we should automatically return to the main menu, because
        // the last item is no longer in the visible region.
        if (m_items.back()->getRegion().top < LAST_ITEM_POSITION_EXIT)
        {
            m_nextState = ViewState::MainMenu;
        }

        return m_nextState;
    }

    void About::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        static const auto RENDER_TOP = -(0.35f * Configuration::getGraphics().getViewCoordinates().height);
        MenuView::render(renderTarget, elapsedTime);

        m_version->render(renderTarget);

        for (auto&& item : m_items)
        {
            if (item->getRegion().top > RENDER_TOP)
            {
                item->render(renderTarget);
            }
        }
    }

    void About::navigateBack()
    {
        m_nextState = ViewState::MainMenu;
    }

    float About::addItem(std::string item, float top)
    {
        static const auto ITEM_SPACING_FACTOR = 2.0f;

        //
        // Creating a test item of the whole alphabet so we can always find the
        // tallest character in the font and use that for the height to separate menu items.
        // NOTE: I understand this is wasteful to do this everytime.  But because this is not performance
        //       sensitive, I'm not going to increase the complexity just to do these one time.
        sf::Text alphabetItem("ABCDEFGHIHKLMNOPQRTSUVWXYZ", *Content::get<sf::Font>(content::KEY_FONT_CREDITS), Configuration::get<std::uint8_t>(config::FONT_CREDITS_ITEM_SIZE));
        alphabetItem.scale(Configuration::getGraphics().getScaleUI());

        // We just put a left of 0, because it gets centered later on
        ui::Text::Settings settingsItem{
            true, 0, top, item,
            Content::get<sf::Font>(content::KEY_FONT_CREDITS),
            Configuration::get<sf::Color>(config::FONT_CREDITS_COLOR_ITEM_FILL),
            Configuration::get<sf::Color>(config::FONT_CREDITS_COLOR_ITEM_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_CREDITS_ITEM_SIZE)
        };
        m_items.push_back(std::make_shared<ui::Text>(settingsItem));
        top += alphabetItem.getGlobalBounds().height * ITEM_SPACING_FACTOR;

        return top;
    }
} // namespace views
