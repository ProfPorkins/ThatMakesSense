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

#include "Credits.hpp"

#include "services/Configuration.hpp"

namespace views
{
    bool Credits::start()
    {
        MenuView::start();
        // Need to ensure the next state stays the Credits view
        m_nextState = ViewState::Credits;

        //
        // Wipe everything out and reset the credits every time
        m_credits.clear();

        //
        // The credits are a scrolling list of text items that identify who and what are
        // responsible for different parts of the game.
        auto top = (0.50f * Configuration::getGraphics().getViewCoordinates().height);
        top = addCredit("Programming", "Daniel Snow", top);
        top = addCredit("Additional Programming", "Taylor Anderson", top);
        top = addCredit("Additional Programming", "Ryan Anderson", top);

        //
        // Level/Game Design credits
        top = addCredit("Game Design", "Daniel Snow", top);
        top = addCredit("Level Design", "Ryan Anderson", top);
        top = addCredit("Level Design", "Daniel Snow", top);

        // Third-Party code credits
        top = addCredit("Simple and Fast Multimedia Library", "https://www.sfml-dev.org", top);
        top = addCredit("RapidJSON", "https://rapidjson.org", top);
        top = addCredit("CTTI", "https://github.com/Manu343726/ctti", top);

        //
        // Font credits
        top = addCredit("Font: Chewy", "Sideshow", top);
        top = addCredit("Font: Roboto", "Christian Robertson", top);
        top = addCredit("Font: SourceCodePro", "Paul D. Hunt", top);

        //
        // Artwork credits
        top = addCredit("Void: Kam's Hero Tiles", "https://voidquail.itch.io/kams-hero-tiles", top);
        top = addCredit("Kenny: UI Pack", "https://opengameart.org/content/ui-pack", top);
        top = addCredit("yughues: HandPainted Plants 2", "https://opengameart.org/content/free-handpainted-plants-2", top);
        top = addCredit("rubberduck: Low-Poly Plants", "https://opengameart.org/content/free-isometric-plants-pack", top);
        top = addCredit("Xelu: Keyboard & Controller Icons", "https://opengameart.org/content/free-keyboard-and-controllers-prompts-pack", top);
        top = addCredit("Terazilla: Controller Icons", "https://opengameart.org/content/free-controller-prompts-xbox-playstation-switch-pc", top);
        top = addCredit("Other Artwork", "Daniel Snow", top);

        //
        // Music credits
        top = addCredit("Music by Matthew Pablo", "https://www.mathewpablo.com", top);

        //
        // Sound effects credits
        top = addCredit("Audio: menu-activate.wav", "SoundScapes - freesound.org", top);
        top = addCredit("Audio: menu-accept.wav", "LokiF - opengameart.org", top);
        top = addCredit("Audio: step.ogg", "IgnasD - opengameart.org", top);
        top = addCredit("Audio: level-complete.ogg", "Fupi - opengameart.org", top);
        top = addCredit("Audio: rule-changed.ogg", "DoKashiteru - opengameart.org", top);
        top = addCredit("Audio: watersplash.ogg", "Blender Foundation - opengameart.org", top);
        top = addCredit("Audio: fire.ogg", "artisticdude - opengameart.org", top);

        return true;
    }

    void Credits::stop()
    {
        MenuView::stop();
    }

    ViewState Credits::update(const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        static const auto MOVE_RATE_US = Configuration::getGraphics().getViewCoordinates().height / 8000000;
        static const auto LAST_CREDIT_POSITION_EXIT = -40;

        MenuView::update(elapsedTime, now);

        //
        // Scroll the credits up vertically over time
        for (auto&& item : m_credits)
        {
            item->setPosition({ item->getRegion().left, item->getRegion().top - MOVE_RATE_US * elapsedTime.count() });
        }

        // Check to see if we should automatically return to the main menu, because
        // the last item is no longer in the visible region.
        if (m_credits.back()->getRegion().top < LAST_CREDIT_POSITION_EXIT)
        {
            m_nextState = ViewState::MainMenu;
        }

        return m_nextState;
    }

    void Credits::render(sf::RenderTarget& renderTarget, [[maybe_unused]] const std::chrono::microseconds elapsedTime)
    {
        static const auto RENDER_TOP = -(0.35f * Configuration::getGraphics().getViewCoordinates().height);
        MenuView::render(renderTarget, elapsedTime);

        for (auto&& item : m_credits)
        {
            if (item->getRegion().top > RENDER_TOP)
            {
                item->render(renderTarget);
            }
        }
    }

    void Credits::navigateBack()
    {
        m_nextState = ViewState::MainMenu;
    }

    float Credits::addCredit(std::string title, std::string item, float top)
    {
        static const auto TITLE_SPACING_FACTOR = 1.5f;
        static const auto ITEM_SPACING_FACTOR = 2.0f;

        //
        // Creating a test item of the whole alphabet so we can always find the
        // tallest character in the font and use that for the height to separate menu items.
        // NOTE: I understand this is wasteful to do this everytime.  But because this is not performance
        //       sensitive, I'm not going to increase the complexity just to do these one time.
        sf::Text alphabetTitle("ABCDEFGHIHKLMNOPQRTSUVWXYZ", *Content::get<sf::Font>(content::KEY_FONT_CREDITS), Configuration::get<std::uint8_t>(config::FONT_CREDITS_TITLE_SIZE));
        alphabetTitle.scale(Configuration::getGraphics().getScaleUI());
        sf::Text alphabetItem("ABCDEFGHIHKLMNOPQRTSUVWXYZ", *Content::get<sf::Font>(content::KEY_FONT_CREDITS), Configuration::get<std::uint8_t>(config::FONT_CREDITS_ITEM_SIZE));
        alphabetItem.scale(Configuration::getGraphics().getScaleUI());

        ui::Text::Settings settingsTitle{
            true, 0, top, title,
            Content::get<sf::Font>(content::KEY_FONT_CREDITS),
            Configuration::get<sf::Color>(config::FONT_CREDITS_COLOR_TITLE_FILL),
            Configuration::get<sf::Color>(config::FONT_CREDITS_COLOR_TITLE_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_CREDITS_TITLE_SIZE)
        };
        m_credits.push_back(std::make_shared<ui::Text>(settingsTitle));

        top += alphabetTitle.getGlobalBounds().height * TITLE_SPACING_FACTOR;
        ui::Text::Settings settingsItem{
            true, 0, top, item,
            Content::get<sf::Font>(content::KEY_FONT_CREDITS),
            Configuration::get<sf::Color>(config::FONT_CREDITS_COLOR_ITEM_FILL),
            Configuration::get<sf::Color>(config::FONT_CREDITS_COLOR_ITEM_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_CREDITS_ITEM_SIZE)
        };
        m_credits.push_back(std::make_shared<ui::Text>(settingsItem));
        top += alphabetItem.getGlobalBounds().height * ITEM_SPACING_FACTOR;

        return top;
    }
} // namespace views
