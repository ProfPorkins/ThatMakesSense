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

#include "HelpControls.hpp"

#include "services/ControllerInput.hpp"

namespace views
{
    bool HelpControls::start()
    {
        MenuView::start();
        m_nextState = ViewState::HelpControls;

        if (!m_initialized)
        {
            using namespace std::string_literals;
            //
            // Creating a text item of the whole alphabet so we can always find the
            // tallest character in the font and use that for the height to separate menu items.
            sf::Text alphabet("ABCDEFGHIHKLMNOPQRTSUVWXYZ", *Content::get<sf::Font>(content::KEY_FONT_MENU), Configuration::get<std::uint8_t>(config::FONT_MENU_SIZE));
            alphabet.scale(Configuration::getGraphics().getScaleUI());

            //
            // Prepare the menu items
            ui::MenuItem::Settings settings{
                { true, 0, 0, "Movement - Keyboard"s,
                  Content::get<sf::Font>(content::KEY_FONT_MENU),
                  Configuration::get<sf::Color>(config::FONT_MENU_COLOR_FILL),
                  Configuration::get<sf::Color>(config::FONT_MENU_COLOR_OUTLINE),
                  alphabet.getCharacterSize() },
                Configuration::get<sf::Color>(config::FONT_MENU_COLOR_ACTIVE_FILL),
                Configuration::get<sf::Color>(config::FONT_MENU_COLOR_ACTIVE_OUTLINE),
                [this]()
                {
                    m_nextState = ViewState::HelpKeyboardMovement;
                }
            };
            auto menuMovementKeyboard = std::make_shared<ui::MenuItem>(settings);
            m_menuItems.push_back(menuMovementKeyboard);

            settings.label = "Movement - Controller"s;
            settings.handler = [this]()
            {
                switch (ControllerInput::instance().whichControllerVendorUsed())
                {
                    case ControllerInput::VENDOR_ID_SONY:
                        m_nextState = ViewState::HelpControllerMovementPS;
                        break;
                    default:
                        m_nextState = ViewState::HelpControllerMovementXBox;
                }
            };
            auto menuMovementController = std::make_shared<ui::MenuItem>(settings);
            m_menuItems.push_back(menuMovementController);

            settings.label = "Camera - Keyboard"s;
            settings.handler = [this]()
            {
                m_nextState = ViewState::HelpKeyboardCamera;
            };
            auto menuCameraKeyboard = std::make_shared<ui::MenuItem>(settings);
            m_menuItems.push_back(menuCameraKeyboard);

            settings.label = "Camera - Controller"s;
            settings.handler = [this]()
            {
                switch (ControllerInput::instance().whichControllerVendorUsed())
                {
                    case ControllerInput::VENDOR_ID_SONY:
                        m_nextState = ViewState::HelpControllerCameraPS;
                        break;
                    default:
                        m_nextState = ViewState::HelpControllerCameraXBox;
                }
            };
            auto menuCameraController = std::make_shared<ui::MenuItem>(settings);
            m_menuItems.push_back(menuCameraController);

            //
            // Go back through the menu items and center everything vertically and horizontally
            auto totalHeight = m_menuItems.size() * alphabet.getGlobalBounds().height * 1.5f;
            auto currentY = 0.0f - totalHeight / 2.0f;
            for (auto&& item : m_menuItems)
            {
                item->setRegion({ -item->getRegion().width / 2.0f,
                                  currentY,
                                  item->getRegion().width,
                                  alphabet.getGlobalBounds().height * 1.25f }); // TODO: This 1.25f should not be necessary!!
                currentY += alphabet.getGlobalBounds().height * 1.5f;
            }

            //
            // Select the first menu choice by default
            m_activeMenuItem = 0;
            m_menuItems[m_activeMenuItem]->setActive();

            m_initialized = true;
        }

        for (auto&& menuItem : m_menuItems)
        {
            menuItem->start();
        }

        return true;
    }

    void HelpControls::stop()
    {
        MenuView::stop();

        for (auto&& menuItem : m_menuItems)
        {
            menuItem->stop();
        }
    }

    ViewState HelpControls::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        MenuView::update(elapsedTime, now);

        return m_nextState;
    }

    void HelpControls::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        MenuView::render(renderTarget, elapsedTime);

        for (auto&& item : m_menuItems)
        {
            item->render(renderTarget);
        }
    }

    void HelpControls::navigateUp()
    {
        m_menuItems[m_activeMenuItem]->setInactive();
        m_activeMenuItem--;
        if (m_activeMenuItem < 0)
        {

            m_activeMenuItem = static_cast<std::int8_t>(m_menuItems.size() - 1);
        }
        m_menuItems[m_activeMenuItem]->setActive();
    }

    void HelpControls::navigateDown()
    {
        m_menuItems[m_activeMenuItem]->setInactive();
        m_activeMenuItem = (static_cast<std::size_t>(m_activeMenuItem) + 1) % m_menuItems.size();
        m_menuItems[m_activeMenuItem]->setActive();
    }

    void HelpControls::navigateBack()
    {
        m_nextState = ViewState::MainMenu;
    }

    void HelpControls::onMouseMoved(math::Point2f point, const std::chrono::microseconds)
    {
        for (decltype(m_menuItems.size()) item = 0; item < m_menuItems.size(); item++)
        {
            if (m_menuItems[item]->getRegion().contains(point))
            {
                if (!m_menuItems[item]->isActive())
                {
                    m_menuItems[m_activeMenuItem]->setInactive();
                    m_activeMenuItem = static_cast<std::int8_t>(item);
                    m_menuItems[m_activeMenuItem]->setActive();
                }
            }
        }
    }

    void HelpControls::onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime)
    {
        for (auto&& item : m_menuItems)
        {
            item->onMouseReleased(button, point, elapsedTime);
        }
    }
} // namespace views
