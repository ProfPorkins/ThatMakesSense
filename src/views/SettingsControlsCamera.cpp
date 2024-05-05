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

#include "SettingsControlsCamera.hpp"

#include "UIFramework/KeyboardOption.hpp"
#include "services/KeyboardInput.hpp"

#include <SFML/Graphics.hpp>
#include <algorithm>
#include <sstream>

namespace views
{
    bool SettingsControlsCamera::start()
    {
        MenuView::start();
        m_nextState = ViewState::SettingsControlsCamera;

        if (!m_initialized)
        {
            addKeyboardCameraOptions();
            addMouseCameraOptions();

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

    void SettingsControlsCamera::stop()
    {
        MenuView::stop();

        KeyboardInput::instance().unregisterAnyKeyReleasedHandler(m_keyboardHandlerId);

        for (auto&& option : m_options)
        {
            option->stop();
        }
    }

    ViewState SettingsControlsCamera::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        MenuView::update(elapsedTime, now);

        return m_nextState;
    }

    void SettingsControlsCamera::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
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

        m_selectCameraKey.render(renderTarget);
    }

    void SettingsControlsCamera::navigateUp()
    {
        if (!m_selectCameraKey.isVisible())
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
    void SettingsControlsCamera::navigateDown()
    {
        if (!m_selectCameraKey.isVisible())
        {
            m_options[m_activeOption]->setInactive();
            m_activeOption = (static_cast<std::size_t>(m_activeOption) + 1) % m_options.size();
            m_options[m_activeOption]->setActive();
        }
    }
    void SettingsControlsCamera::navigateLeft()
    {
        if (!m_selectCameraKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Left);
        }
    }
    void SettingsControlsCamera::navigateRight()
    {
        if (!m_selectCameraKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Right);
        }
    }
    void SettingsControlsCamera::navigateBack()
    {
        if (!m_selectCameraKey.isVisible())
        {
            m_nextState = ViewState::SettingsControls;
        }
        else
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Escape);
        }
    }
    void SettingsControlsCamera::navigateSelect()
    {
        if (!m_selectCameraKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(sf::Keyboard::Enter);
        }
    }

    void SettingsControlsCamera::addKeyboardCameraOptions()
    {
        const auto OFFSET_LEFT_RIGHT = 0.30f;

        auto top = (-0.35f * Configuration::getGraphics().getViewCoordinates().height);

        //
        // Start the top at the bottom of the last movement control
        ui::Text::Settings settingsHeaderCamera{
            true, 0, top,
            "-- Camera --",
            Content::get<sf::Font>(content::KEY_FONT_SETTINGS),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_TITLE_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_SETTINGS_TITLE_COLOR_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_SETTINGS_HEADING_SIZE)
        };
        auto headingCamera = std::make_shared<ui::Text>(settingsHeaderCamera);
        top += headingCamera->getRegion().height * 1.5f;
        m_headers.push_back(headingCamera);

        //
        // Locate the "press a key..." right above the keyboard input options
        m_selectCameraKey.setPosition({ -(m_selectCameraKey.getRegion().width / 2.0f), top });
        top += m_selectCameraKey.getRegion().height * 1.25f;
        m_selectCameraKey.hide();

        ui::KeyboardOption::Settings settingsPanUp{
            0.0f, top,
            "Pan-Up", Configuration::get<std::string>(config::KEYBOARD_CAMERA_UP),
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
                m_selectCameraKey.show();
            },
            [this](std::string key)
            {
                m_selectCameraKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_CAMERA_UP, key);
                }
            }
        };
        auto panUp = std::make_shared<ui::KeyboardOption>(settingsPanUp);

        ui::KeyboardOption::Settings settingsPanLeft{
            -(Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_LEFT_RIGHT,
            panUp->getRegion().getBottom(),
            "Pan-Left",
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_LEFT),
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
                m_selectCameraKey.show();
            },
            [this](std::string key)
            {
                m_selectCameraKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_CAMERA_LEFT, key);
                }
            }
        };
        auto panLeft = std::make_shared<ui::KeyboardOption>(settingsPanLeft);

        ui::KeyboardOption::Settings settingsPanRight{
            (Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_LEFT_RIGHT,
            panUp->getRegion().getBottom(),
            "Pan-Right",
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_RIGHT),
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
                m_selectCameraKey.show();
            },
            [this](std::string key)
            {
                m_selectCameraKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_CAMERA_RIGHT, key);
                }
            }
        };
        auto panRight = std::make_shared<ui::KeyboardOption>(settingsPanRight);

        ui::KeyboardOption::Settings settingsPanDown{
            0.0f, panLeft->getRegion().getBottom(),
            "Pan-Down",
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_DOWN),
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
                m_selectCameraKey.show();
            },
            [this](std::string key)
            {
                m_selectCameraKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_CAMERA_DOWN, key);
                }
            }
        };
        auto panDown = std::make_shared<ui::KeyboardOption>(settingsPanDown);

        ui::KeyboardOption::Settings settingsZoomIn{
            -(Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_LEFT_RIGHT,
            panDown->getRegion().getBottom(),
            "Zoom-In",
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_ZOOM_IN),
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
                m_selectCameraKey.show();
            },
            [this](std::string key)
            {
                m_selectCameraKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_CAMERA_ZOOM_IN, key);
                }
            }
        };
        auto zoomIn = std::make_shared<ui::KeyboardOption>(settingsZoomIn);

        ui::KeyboardOption::Settings settingsZoomOut{
            (Configuration::getGraphics().getViewCoordinates().width / 2.0f) * OFFSET_LEFT_RIGHT,
            panDown->getRegion().getBottom(),
            "Zoom-Out",
            Configuration::get<std::string>(config::KEYBOARD_CAMERA_ZOOM_OUT),
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
                m_selectCameraKey.show();
            },
            [this](std::string key)
            {
                m_selectCameraKey.hide();
                if (key != "esc")
                {
                    Configuration::set<std::string>(config::KEYBOARD_CAMERA_ZOOM_OUT, key);
                }
            }
        };
        auto zoomOut = std::make_shared<ui::KeyboardOption>(settingsZoomOut);

        m_options.push_back(panUp);
        m_options.push_back(panLeft);
        m_options.push_back(panRight);
        m_options.push_back(panDown);
        m_options.push_back(zoomIn);
        m_options.push_back(zoomOut);
    }

    void SettingsControlsCamera::addMouseCameraOptions()
    {
        // Mouse Options label starts right after the last keyboard option
        auto top = m_options.back()->getRegion().getBottom() + m_options.back()->getRegion().height * 0.5f;
        //
        // Doing the dirty deed and using 1/0 to represent true/false
        std::vector<std::tuple<std::string, std::uint8_t>> optionsYesNo = {
            { "Yes", static_cast<std::uint8_t>(1) },
            { "No", static_cast<std::uint8_t>(0) }
        };
        ui::Selection::Settings settings{
            { true, 0, top,
              "Invert Mouse/Controller Horizontal Pan",
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
                Configuration::set<bool>(config::MOUSE_CAMERA_INVERT_PAN_HORZ, value);
            },
            optionsYesNo,
            Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_HORZ) ? 0u : 1u
        };
        auto panHorz = std::make_shared<ui::Selection>(settings);
        m_options.push_back(panHorz);
        top += panHorz->getRegion().height;

        ui::Selection::Settings settingsPanVert{
            { true, 0, top,
              "Invert Mouse/Controller Vertical Pan",
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
                Configuration::set<bool>(config::MOUSE_CAMERA_INVERT_PAN_VERT, value);
            },
            optionsYesNo,
            Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_PAN_VERT) ? 0u : 1u
        };
        auto panVert = std::make_shared<ui::Selection>(settingsPanVert);
        m_options.push_back(panVert);
        top += panVert->getRegion().height;

        ui::Selection::Settings settingsZoom{
            { true, 0, top,
              "Invert Mouse/Controller Zoom",
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
                Configuration::set<bool>(config::MOUSE_CAMERA_INVERT_ZOOM, value);
            },
            optionsYesNo,
            Configuration::get<bool>(config::MOUSE_CAMERA_INVERT_ZOOM) ? 0u : 1u
        };
        auto zoom = std::make_shared<ui::Selection>(settingsZoom);
        m_options.push_back(zoom);
    }

    void SettingsControlsCamera::onKeyReleased(sf::Keyboard::Key key)
    {
        if (m_selectCameraKey.isVisible())
        {
            m_options[m_activeOption]->onKeyPressed(key);
        }
    }

    void SettingsControlsCamera::onMouseMoved(math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime)
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

    void SettingsControlsCamera::onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime)
    {
        m_options[m_activeOption]->onMouseReleased(button, point, elapsedTime);
    }
} // namespace views
