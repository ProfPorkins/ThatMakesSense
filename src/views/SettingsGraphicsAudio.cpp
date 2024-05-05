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

#include "SettingsGraphicsAudio.hpp"

#include "UIFramework/Selection.hpp"
#include "services/Configuration.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <sstream>

namespace views
{
    bool SettingsGraphicsAudio::start()
    {
        MenuView::start();
        m_nextState = ViewState::SettingsGraphicsAudio;

        if (!m_initialized)
        {
// Sorry Linux, just turns out to be too much of a pain to deal with for now
#if !defined(__GNUC__) || defined(__clang__)
            addFullScreenOption();
#endif
            addResolutionOption();
            addMusicOption();

            m_activeOption = 0;
            m_options[m_activeOption]->setActive();

            m_initialized = true;
        }

        return true;
    }

    void SettingsGraphicsAudio::stop()
    {
        MenuView::stop();

        for (auto&& option : m_options)
        {
            option->stop();
        }
    }

    ViewState SettingsGraphicsAudio::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        MenuView::update(elapsedTime, now);

        return m_nextState;
    }

    void SettingsGraphicsAudio::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        MenuView::render(renderTarget, elapsedTime);

        for (auto&& option : m_options)
        {
            option->render(renderTarget);
        }

        m_resizeInfo->render(renderTarget);
    }

    void SettingsGraphicsAudio::navigateUp()
    {
        m_options[m_activeOption]->setInactive();
        m_activeOption--;
        if (m_activeOption < 0)
        {
            m_activeOption = static_cast<std::uint8_t>(m_options.size() - 1);
        }
        m_options[m_activeOption]->setActive();
    }
    void SettingsGraphicsAudio::navigateDown()
    {
        m_options[m_activeOption]->setInactive();
        m_activeOption = (static_cast<std::size_t>(m_activeOption) + 1) % m_options.size();
        m_options[m_activeOption]->setActive();
    }
    void SettingsGraphicsAudio::navigateLeft()
    {
        m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Left);
    }
    void SettingsGraphicsAudio::navigateRight()
    {
        m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Right);
    }
    void SettingsGraphicsAudio::navigateBack()
    {
        m_nextState = ViewState::MainMenu;
    }
    void SettingsGraphicsAudio::navigateSelect()
    {
        m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Enter);
    }

    // --------------------------------------------------------------
    //
    // The full screen options of either Yes or No
    //
    // --------------------------------------------------------------
    void SettingsGraphicsAudio::addFullScreenOption()
    {
        //
        // Doing the dirty deed and using 1/0 to represent true/false
        std::vector<std::tuple<std::string, std::uint8_t>> fullScreenOptions = { { "Yes", static_cast<std::uint8_t>(1) }, { "No", static_cast<std::uint8_t>(0) } };
        ui::Selection::Settings settings{
            { true, 0, (-0.30f * Configuration::getGraphics().getViewCoordinates().height),
              "Full Screen",
              Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
              Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_FILL),
              Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_OUTLINE),
              Configuration::get<std::uint8_t>(config::FONT_SETTINGS_ITEM_SIZE) },
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_OUTLINE),
            [this](std::uint8_t option)
            {
                auto value = static_cast<bool>(option);
                Configuration::set<bool>(config::GRAPHICS_FULL_SCREEN, value);

                // If the full screen is true, then hide the resize message
                if (value)
                {
                    m_resizeInfo->hide();
                }
                else
                {
                    m_resizeInfo->show();
                }

                //
                // Any graphics option change means it is time to restart the window
                Configuration::getGraphics().setRestart(true);
            },
            fullScreenOptions,
            Configuration::get<bool>(config::GRAPHICS_FULL_SCREEN) ? 0u : 1u
        };
        auto fullScreen = std::make_shared<ui::Selection>(settings);
        m_options.push_back(fullScreen);
    }

    // --------------------------------------------------------------
    //
    // Have to query the system for the available (full screen) resolutions to
    // make those available for selection.  In reality, the game uses the
    // desktop resolution for full screen mode.  Therefore, the querying of
    // the full-screen modes is just to give the player something to
    // choose for windowed mode.
    //
    // --------------------------------------------------------------
    void SettingsGraphicsAudio::addResolutionOption()
    {
#if !defined(__GNUC__) || defined(__clang__)
        float verticalPlacement = -0.17f;
#else
        float verticalPlacement = -0.25f;
#endif
        ui::Text::Settings settingsResize{
            true, 0, (verticalPlacement * Configuration::getGraphics().getViewCoordinates().height),
            "Drag window corner/edge to change resolution",
            Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_SETTINGS_ITEM_SIZE)
        };
        m_resizeInfo = std::make_shared<ui::Text>(settingsResize);

        if (Configuration::get<bool>(config::GRAPHICS_FULL_SCREEN))
        {
            m_resizeInfo->hide();
        }
    }

    // --------------------------------------------------------------
    //
    // The play background music options of either Yes or No
    //
    // --------------------------------------------------------------
    void SettingsGraphicsAudio::addMusicOption()
    {
        //
        // Doing the dirty deed and using 1/0 to represent true/false
        std::vector<std::tuple<std::string, std::uint8_t>> musicOptions = { { "Yes", static_cast<std::uint8_t>(1) }, { "No", static_cast<std::uint8_t>(0) } };
        ui::Selection::Settings settings{
            { true, 0, (-0.10f * Configuration::getGraphics().getViewCoordinates().height),
              "Background Music",
              Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
              Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_FILL),
              Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_OUTLINE),
              Configuration::get<std::uint8_t>(config::FONT_SETTINGS_ITEM_SIZE) },
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_OUTLINE),
            [](std::uint8_t option)
            {
                auto value = static_cast<bool>(option);
                Configuration::set<bool>(config::PLAY_BACKGROUND_MUSIC, value);
            },
            musicOptions,
            Configuration::get<bool>(config::PLAY_BACKGROUND_MUSIC) ? 0u : 1u
        };
        auto music = std::make_shared<ui::Selection>(settings);
        m_options.push_back(music);
    }

    void SettingsGraphicsAudio::onMouseMoved(math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        for (decltype(m_options.size()) item = 0; item < m_options.size(); item++)
        {
            if (m_options[item]->getRegion().contains(point))
            {
                if (!m_options[item]->isActive())
                {
                    m_options[m_activeOption]->setInactive();
                    m_activeOption = static_cast<std::int8_t>(item);
                    m_options[m_activeOption]->setActive();
                }
            }
        }
    }

    void SettingsGraphicsAudio::onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime)
    {
        m_options[m_activeOption]->onMouseReleased(button, point, elapsedTime);
    }
} // namespace views
