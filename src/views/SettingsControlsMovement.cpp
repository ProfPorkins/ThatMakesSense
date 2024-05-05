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

#include "SettingsControlsMovement.hpp"

#include "UIFramework/KeyboardOption.hpp"
#include "services/KeyboardInput.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <sstream>

namespace views
{
    bool SettingsControlsMovement::start()
    {
        MenuView::start();
        m_nextState = ViewState::SettingsControlsMovement;

        if (!m_initialized)
        {
            addKeyboardMovementOptions();

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

    void SettingsControlsMovement::stop()
    {
        MenuView::stop();

        KeyboardInput::instance().unregisterAnyKeyReleasedHandler(m_keyboardHandlerId);
    }

    ViewState SettingsControlsMovement::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        MenuView::update(elapsedTime, now);

        return m_nextState;
    }

    void SettingsControlsMovement::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        MenuView::render(renderTarget, elapsedTime);

        for (auto&& option : m_options)
        {
            option->render(renderTarget);
        }

        for (auto&& header : m_headers)
        {
            header->render(renderTarget);
        }

        m_selectMovementKey.render(renderTarget);
    }

    void SettingsControlsMovement::navigateUp()
    {
        if (!m_selectMovementKey.isVisible())
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

    void SettingsControlsMovement::navigateDown()
    {
        if (!m_selectMovementKey.isVisible())
        {
            m_options[m_activeOption]->setInactive();
            m_activeOption = (static_cast<std::size_t>(m_activeOption) + 1) % m_options.size();
            m_options[m_activeOption]->setActive();
        }
    }

    void SettingsControlsMovement::navigateBack()
    {
        if (!m_selectMovementKey.isVisible())
        {
            m_nextState = ViewState::SettingsControls;
        }
        else
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Escape);
        }
    }
    void SettingsControlsMovement::navigateSelect()
    {
        if (!m_selectMovementKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Enter);
        }
    }

    // --------------------------------------------------------------
    //
    // Give the user the ability to change the keyboard inputs.
    //
    // --------------------------------------------------------------
    void SettingsControlsMovement::addKeyboardMovementOptions()
    {
        const auto OFFSET_EAST_WEST = 0.35f;
        const auto OFFSET_NORTH_SOUTH = 0.20f;

        auto top = (-0.35f * Configuration::getGraphics().getViewCoordinates().height);

        ui::Text::Settings settingsHeaderMovement{
            true, 0, top, "-- Movement --", Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_TITLE_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_TITLE_COLOR_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_SETTINGS_HEADING_SIZE)
        };
        auto headingMovement = std::make_shared<ui::Text>(settingsHeaderMovement);
        top += headingMovement->getRegion().height * 1.5f;
        m_headers.push_back(headingMovement);

        //
        // Locate the "press a key..." right above the keyboard input options
        m_selectMovementKey.setPosition({ -(m_selectMovementKey.getRegion().width / 2.0f), top });
        m_selectMovementKey.hide();
        top += m_selectMovementKey.getRegion().height * 1.25f;

        ui::KeyboardOption::Settings settingsNW{
            -(Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_NORTH_SOUTH,
            top,
            "Up-Left", Configuration::get<std::string>(config::KEYBOARD_UP_LEFT),
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
                m_selectMovementKey.show();
            },
            [this](std::string key)
            {
                m_selectMovementKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_UP_LEFT, key);
                }
            }
        };
        auto moveNW = std::make_shared<ui::KeyboardOption>(settingsNW);

        ui::KeyboardOption::Settings settingsNE{
            (Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_NORTH_SOUTH,
            top,
            "Up-Right", Configuration::get<std::string>(config::KEYBOARD_UP_RIGHT),
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
                m_selectMovementKey.show();
            },
            [this](std::string key)
            {
                m_selectMovementKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_UP_RIGHT, key);
                }
            }
        };
        auto moveNE = std::make_shared<ui::KeyboardOption>(settingsNE);

        ui::KeyboardOption::Settings settingsW{
            -(Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_EAST_WEST,
            std::max(moveNW->getRegion().getBottom(), moveNE->getRegion().getBottom()),
            "Left", Configuration::get<std::string>(config::KEYBOARD_LEFT),
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
                m_selectMovementKey.show();
            },
            [this](std::string key)
            {
                m_selectMovementKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_LEFT, key);
                }
            }
        };
        auto moveW = std::make_shared<ui::KeyboardOption>(settingsW);

        ui::KeyboardOption::Settings settingsE{
            (Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_EAST_WEST,
            std::max(moveNW->getRegion().getBottom(), moveNE->getRegion().getBottom()),
            "Right", Configuration::get<std::string>(config::KEYBOARD_RIGHT),
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
                m_selectMovementKey.show();
            },
            [this](std::string key)
            {
                m_selectMovementKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_RIGHT, key);
                }
            }
        };
        auto moveE = std::make_shared<ui::KeyboardOption>(settingsE);

        ui::KeyboardOption::Settings settingsSW{
            -(Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_NORTH_SOUTH,
            std::max(moveW->getRegion().getBottom(), moveE->getRegion().getBottom()),
            "Down-Left", Configuration::get<std::string>(config::KEYBOARD_DOWN_LEFT),
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
                m_selectMovementKey.show();
            },
            [this](std::string key)
            {
                m_selectMovementKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_DOWN_LEFT, key);
                }
            }
        };
        auto moveSW = std::make_shared<ui::KeyboardOption>(settingsSW);

        ui::KeyboardOption::Settings settingsSE{
            (Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_NORTH_SOUTH,
            std::max(moveW->getRegion().getBottom(), moveE->getRegion().getBottom()),
            "Down-Right", Configuration::get<std::string>(config::KEYBOARD_DOWN_RIGHT),
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
                m_selectMovementKey.show();
            },
            [this](std::string key)
            {
                m_selectMovementKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_DOWN_RIGHT, key);
                }
            }
        };
        auto moveSE = std::make_shared<ui::KeyboardOption>(settingsSE);

        top = moveSE->getRegion().height + headingMovement->getRegion().height;
        ui::Text::Settings settingsPull{
            true,
            0, top,
            "Pull : [shift/left-trigger] plus [direction]",
            Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_TITLE_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_TITLE_COLOR_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_SETTINGS_HEADING_SIZE)
        };
        auto pullLabel = std::make_shared<ui::Text>(settingsPull);

        // Add them in this order, because it impacts the keyboard control order as the
        // user navigates the options.
        m_options.push_back(moveNW);
        m_options.push_back(moveNE);
        m_options.push_back(moveW);
        m_options.push_back(moveE);
        m_options.push_back(moveSW);
        m_options.push_back(moveSE);
        m_headers.push_back(pullLabel);
    }

    void SettingsControlsMovement::onKeyReleased(sf::Keyboard::Key key)
    {
        if (m_selectMovementKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(key);
        }
    }

    void SettingsControlsMovement::onMouseMoved(math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime)
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

    void SettingsControlsMovement::onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime)
    {
        m_options[m_activeOption]->onMouseReleased(button, point, elapsedTime);
    }
} // namespace views
