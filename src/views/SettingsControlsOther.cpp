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

#include "SettingsControlsOther.hpp"

#include "UIFramework/KeyboardOption.hpp"
#include "services/KeyboardInput.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <sstream>

namespace views
{
    bool SettingsControlsOther::start()
    {
        MenuView::start();
        m_nextState = ViewState::SettingsControlsOther;

        if (!m_initialized)
        {
            // addHintOption();
            addKeyboardOptions();

            m_activeOption = 0;
            m_options[m_activeOption]->setActive();

            m_initialized = true;
        }

        //
        // Get the keyboard inputs registered
        m_keyboardHandlerId = KeyboardInput::instance().registerAnyKeyReleasedHandler(
            [this](sf::Keyboard::Key key)
            {
                onKeyReleased(key);
            });

        return true;
    }

    void SettingsControlsOther::stop()
    {
        MenuView::stop();

        KeyboardInput::instance().unregisterAnyKeyReleasedHandler(m_keyboardHandlerId);

        for (auto&& option : m_options)
        {
            option->stop();
        }
    }

    ViewState SettingsControlsOther::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        MenuView::update(elapsedTime, now);

        return m_nextState;
    }

    void SettingsControlsOther::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        MenuView::render(renderTarget, elapsedTime);

        for (auto&& option : m_options)
        {
            option->render(renderTarget);
        }

        m_selectKey.render(renderTarget);
    }

    void SettingsControlsOther::navigateUp()
    {
        if (!m_selectKey.isVisible())
        {
            m_options[m_activeOption]->setInactive();
            m_activeOption--;
            if (m_activeOption < 0)
            {
                m_activeOption = static_cast<std::uint8_t>(m_options.size() - 1);
            }
            m_options[m_activeOption]->setActive();
        }
    }

    void SettingsControlsOther::navigateDown()
    {
        if (!m_selectKey.isVisible())
        {
            m_options[m_activeOption]->setInactive();
            m_activeOption = (static_cast<std::size_t>(m_activeOption) + 1) % m_options.size();
            m_options[m_activeOption]->setActive();
        }
    }

    void SettingsControlsOther::navigateBack()
    {
        if (!m_selectKey.isVisible())
        {
            m_nextState = ViewState::SettingsControls;
        }
        else
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Escape);
        }
    }

    void SettingsControlsOther::navigateSelect()
    {
        if (!m_selectKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Enter);
        }
    }

    void SettingsControlsOther::addHintOption()
    {
        auto top = (-0.35f * Configuration::getGraphics().getViewCoordinates().height);

        //
        // Doing the dirty deed and using 1/0 to represent true/false
        std::vector<std::tuple<std::string, std::uint8_t>> optionsYesNo = {
            { "Yes", static_cast<std::uint8_t>(1) },
            { "No", static_cast<std::uint8_t>(0) }
        };
        ui::Selection::Settings settings{
            { true, 0, top,
              "Show Hints",
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
                Configuration::set<bool>(config::OPTIONS_SHOW_HINTS, value);
            },
            optionsYesNo,
            Configuration::get<bool>(config::OPTIONS_SHOW_HINTS) ? 0u : 1u
        };
        auto showHints = std::make_shared<ui::Selection>(settings);
        m_options.push_back(showHints);
        top += showHints->getRegion().height;
    }

    void SettingsControlsOther::addKeyboardOptions()
    {
        auto top = (-0.35f * Configuration::getGraphics().getViewCoordinates().height);
        // Uncomment the line below if the hint option is ever brough back
        // auto top = m_options.back()->getRegion().getBottom() + m_options.back()->getRegion().height * 0.5f;

        //
        // Locate the "press a key..." right above the keyboard input options
        m_selectKey.setPosition({ -(m_selectKey.getRegion().width / 2.0f), top });
        m_selectKey.hide();
        top += m_selectKey.getRegion().height * 1.25f;

        ui::KeyboardOption::Settings settingsUndo{
            0, top,
            "Undo", Configuration::get<std::string>(config::KEYBOARD_UNDO),
            Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_SETTINGS_ITEM_SIZE),
            [this]()
            {
                m_selectKey.show();
            },
            [this](std::string key)
            {
                m_selectKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_UNDO, key);
                }
            }
        };
        auto undo = std::make_shared<ui::KeyboardOption>(settingsUndo);
        m_options.push_back(undo);

        top += undo->getRegion().height;
        ui::KeyboardOption::Settings settingsReset{
            0, top,
            "Reset", Configuration::get<std::string>(config::KEYBOARD_RESET),
            Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_SETTINGS_ITEM_SIZE),
            [this]()
            {
                m_selectKey.show();
            },
            [this](std::string key)
            {
                m_selectKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_RESET, key);
                }
            }
        };
        auto reset = std::make_shared<ui::KeyboardOption>(settingsReset);
        m_options.push_back(reset);

        top += undo->getRegion().height;
        ui::KeyboardOption::Settings settingsTogglePhraseRendering{
            0, top,
            "Toggle Phrase Arrows", Configuration::get<std::string>(config::KEYBOARD_TOGGLE_PHRASE_RENDERING),
            Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_ACTIVE_OUTLINE),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_COLOR_SELECTION_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_SETTINGS_ITEM_SIZE),
            [this]()
            {
                m_selectKey.show();
            },
            [this](std::string key)
            {
                m_selectKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_TOGGLE_PHRASE_RENDERING, key);
                }
            }
        };
        auto togglePhraseRendering = std::make_shared<ui::KeyboardOption>(settingsTogglePhraseRendering);
        m_options.push_back(togglePhraseRendering);
    }

    void SettingsControlsOther::onKeyReleased(sf::Keyboard::Key key)
    {
        if (m_selectKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(key);
        }
    }

    void SettingsControlsOther::onMouseMoved(math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime)
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

    void SettingsControlsOther::onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime)
    {
        m_options[m_activeOption]->onMouseReleased(button, point, elapsedTime);
    }
} // namespace views
