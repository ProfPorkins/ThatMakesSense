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

#include "Puzzles.hpp"

#include "GameModel.hpp"
#include "services/Audio.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"
#include "services/MouseInput.hpp"
#include "services/Scoring.hpp"

#include <format>

namespace views
{

    bool Puzzles::start()
    {
        MenuView::start();
        m_nextState = ViewState::Puzzles;

        initializeSprites();
        // This has to be regenerated every time, because the scoring details can change during
        // the time the program is active.
        {
            using namespace std::string_literals;

            m_menuItems.clear();
            m_statusChallenges.clear();
            m_textChallenges.clear();

            //
            // Creating a text item of the whole alphabet so we can always find the
            // tallest character in the font and use that for the height to separate menu items.
            sf::Text alphabetItem("ABCDEFGHIHKLMNOPQRTSUVWXYZ", *Content::get<sf::Font>(content::KEY_FONT_CHALLENGES), Configuration::get<std::uint8_t>(config::FONT_CHALLENGES_ITEM_SIZE));
            alphabetItem.scale(Configuration::getGraphics().getScaleUI());

            auto coords = Configuration::getGraphics().getViewCoordinates();
            auto top = -(coords.height / 2.0f) + (coords.height / 2.0f) * OFFSET_TOP;

            // Position the headers
            m_headerPuzzles.setPosition({ -coords.width / 2.0f + OFFSET_HEADER_PUZZLES * coords.width, top });
            m_headerChallenges.setPosition({ -coords.width / 2.0f + OFFSET_HEADER_CHALLENGES * coords.width, top });
            m_headerDetail.setPosition({ coords.width * OFFSET_HEADER_DETAIL, top });

            // Update top based on header size
            top += m_headerPuzzles.getRegion().height * 2.5f;

            m_boxDetail.setPosition({ OFFSET_DETAILBOX_LEFT * coords.width, top });
            m_boxDetail.setSize({ coords.width * (0.5f - (OFFSET_DETAILBOX_LEFT + OFFSET_DETAILBOX_RIGHT)), 55 });
            m_boxDetail.setFillColor(sf::Color::Transparent);
            m_boxDetail.setOutlineColor(Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_FILL));
            m_boxDetail.setOutlineThickness(1.0f);

            if (!m_initialized)
            {
                // Figure out how many levels can be rendered, along with how many to render at the start.
                // Once the visible count is initialized, don't redo it on subsequent calls to ::start, because
                // we want to leave it how it was when the user left the screen.
                //
                // Taking std::abs because top is negative (should be) due to 0, 0 being the center
                // of the screen and the header info is rendered into the top half (where y coords are negative).
                std::size_t howManyPossible = static_cast<std::size_t>((coords.height - std::abs(top) - OFFSET_BOTTOM) / (alphabetItem.getGlobalBounds().height * 1.5f));
                m_visibleCount = static_cast<std::uint8_t>(std::min(howManyPossible, Content::getLevels().size()));
            }

            constructLevelItems(coords, top, alphabetItem);

            //
            // In case the number of levels has changed, make sure the active
            // selection isn't out of bound.
            m_activeLevel = std::min(m_activeLevel, static_cast<std::int8_t>(m_menuItems.size() - 1));
            m_menuItems[m_activeLevel]->setActive();

            m_initialized = true;
        }

        registerInputHanlders();
        repositionLevelItems();

        return true;
    }

    void Puzzles::stop()
    {
        MenuView::stop();

        for (auto&& menuItem : m_menuItems)
        {
            menuItem->stop();
        }

        MouseInput::instance().unregisterMouseWheelHandler(m_mouseWheelHandlerId);
    }

    ViewState Puzzles::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        MenuView::update(elapsedTime, now);

        return m_nextState;
    }

    void Puzzles::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        MenuView::render(renderTarget, elapsedTime);

        m_headerPuzzles.render(renderTarget);
        m_headerChallenges.render(renderTarget);
        m_headerDetail.render(renderTarget);
        renderTarget.draw(m_boxDetail);

        for (auto puzzle = m_visibleStart; puzzle < m_visibleStart + m_visibleCount; puzzle++)
        {
            m_menuItems[puzzle]->render(renderTarget);
        }

        // Render the summary checkboxes that show the status of the challenges
        for (auto puzzle = m_visibleStart; puzzle < m_visibleStart + m_visibleCount; puzzle++)
        {
            auto top = m_menuItems[puzzle]->getRegion().getCenter().y;
            auto left = m_headerChallenges.getRegion().left;
            for (auto&& [preDefined, status] : m_statusChallenges[puzzle])
            {
                if (status)
                {
                    if (preDefined)
                    {
                        m_boxFilled.setPosition({ left, top });
                        renderTarget.draw(m_boxFilled);
                    }
                    else
                    {
                        m_boxFilledBC.setPosition({ left, top });
                        renderTarget.draw(m_boxFilledBC);
                    }
                }
                else
                {
                    m_boxEmpty.setPosition({ left, top });
                    renderTarget.draw(m_boxEmpty);
                }

                left += (m_boxEmpty.getGlobalBounds().width + m_boxEmpty.getGlobalBounds().width * 0.15f);
            }
        }

        // Render the full detail for the actively selected level
        {
            auto left = m_boxDetail.getGlobalBounds().left + m_boxDetail.getGlobalBounds().width - (1 + m_boxEmpty.getGlobalBounds().width * 2);
            for (std::uint8_t challenge = 0; auto&& item : m_textChallenges[m_activeLevel])
            {
                item->render(renderTarget);

                auto top = item->getRegion().getCenter().y;
                if (std::get<1>(m_statusChallenges[m_activeLevel][challenge]))
                {
                    if (std::get<0>(m_statusChallenges[m_activeLevel][challenge]))
                    {
                        m_boxFilled.setPosition({ left, top });
                        renderTarget.draw(m_boxFilled);
                    }
                    else
                    {
                        m_boxFilledBC.setPosition({ left, top });
                        renderTarget.draw(m_boxFilledBC);
                    }
                }
                else
                {
                    m_boxEmpty.setPosition({ left, top });
                    renderTarget.draw(m_boxEmpty);
                }

                challenge++;
            }
        }
    }

    void Puzzles::navigateUp()
    {
        m_menuItems[m_activeLevel]->setInactive();
        m_activeLevel--;
        if (m_activeLevel < 0)
        {
            m_activeLevel = static_cast<std::int8_t>(m_menuItems.size() - 1);
        }
        m_menuItems[m_activeLevel]->setActive();

        if (m_activeLevel < m_visibleStart)
        {
            m_visibleStart = m_activeLevel;
            repositionLevelItems();
        }
        else if (m_activeLevel > (m_visibleStart + m_visibleCount))
        {
            m_visibleStart = m_activeLevel - m_visibleCount + 1;
            repositionLevelItems();
        }
    }
    void Puzzles::navigateDown()
    {
        m_menuItems[m_activeLevel]->setInactive();
        m_activeLevel = (static_cast<std::size_t>(m_activeLevel) + 1) % m_menuItems.size();
        m_menuItems[m_activeLevel]->setActive();

        if (m_activeLevel >= (m_visibleStart + m_visibleCount))
        {
            m_visibleStart = m_activeLevel - m_visibleCount + 1;
            repositionLevelItems();
        }
        else if (m_activeLevel < m_visibleStart)
        {
            m_visibleStart = m_activeLevel;
            repositionLevelItems();
        }
    }

    void Puzzles::navigateBack()
    {
        m_nextState = ViewState::MainMenu;
    }

    void Puzzles::onMouseMoved(math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        for (std::uint8_t item = m_visibleStart; item < m_visibleStart + m_visibleCount; item++)
        {
            if (m_menuItems[item]->getRegion().contains(point))
            {
                if (!m_menuItems[item]->isActive())
                {
                    m_menuItems[m_activeLevel]->setInactive();
                    m_activeLevel = static_cast<std::int8_t>(item);
                    m_menuItems[m_activeLevel]->setActive();
                }
            }
        }
    }

    void Puzzles::onMouseReleased(sf::Mouse::Button button, math::Point2f point, const std::chrono::microseconds elapsedTime)
    {
        for (auto&& item : m_menuItems)
        {
            item->onMouseReleased(button, point, elapsedTime);
        }
    }

    void Puzzles::onMouseWheel(sf::Event::MouseWheelScrollEvent event)
    {
        if (event.delta > 0)
        {
            m_visibleStart = static_cast<std::uint8_t>(std::max(m_visibleStart - 1, 0));
            // If the active item is no longer visible, select a new one
            if (!(m_activeLevel >= m_visibleStart && m_activeLevel < (m_visibleStart + m_visibleCount)))
            {
                m_menuItems[m_activeLevel]->setInactive();
                m_activeLevel--;
                m_menuItems[m_activeLevel]->setActive();
            }
            repositionLevelItems();
        }
        else if (event.delta < 0)
        {
            m_visibleStart = std::min(static_cast<std::uint8_t>(m_visibleStart + 1), static_cast<std::uint8_t>(m_menuItems.size() - m_visibleCount));
            // If the active item is no longer visible, select a new one
            if (!(m_activeLevel >= m_visibleStart && m_activeLevel < (m_visibleStart + m_visibleCount)))
            {
                m_menuItems[m_activeLevel]->setInactive();
                m_activeLevel++;
                m_menuItems[m_activeLevel]->setActive();
            }
            repositionLevelItems();
        }
    }

    void Puzzles::constructLevelItems(math::Dimension2f& coords, float& top, sf::Text& alphabetItem)
    {
        auto& levels = Content::getLevels();

        for (std::uint8_t l = 0; l < levels.size(); l++)
        {
            auto level = levels.get(l);

            ui::MenuItem::Settings settingsLevel{
                { false,
                  -coords.width / 2.0f + OFFSET_PUZZLES_LEFT * coords.width, top,
                  std::format("{0:2d}. {1}", l + 1, level->getName()),
                  Content::get<sf::Font>(content::KEY_FONT_CHALLENGES),
                  Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_FILL),
                  Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_OUTLINE),
                  alphabetItem.getCharacterSize() },
                Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_ACTIVE_FILL),
                Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_ACTIVE_OUTLINE),
                [this, level]()
                {
                    GameModel::selectLevel(level);
                    m_nextState = ViewState::GamePlay;
                }
            };
            m_menuItems.push_back(std::make_shared<ui::MenuItem>(settingsLevel));
            top += alphabetItem.getGlobalBounds().height * 1.5f;

            // Build the array of challenges and their status
            m_statusChallenges.push_back({});
            m_textChallenges.push_back({});
            auto topChallenge = m_boxDetail.getGlobalBounds().top + alphabetItem.getGlobalBounds().height;

            // Build the info for the pre-defined challenges
            for (auto&& challenge : level->getChallenges())
            {
                m_statusChallenges[l].push_back({ true, Scoring::instance().isChallengeMet(level->getUUID(), challenge) });

                ui::Text::Settings settingsChallenge{
                    false,
                    m_boxDetail.getPosition().x + 1, topChallenge,
                    Scoring::formatChallengeFriendly(challenge),
                    Content::get<sf::Font>(content::KEY_FONT_CHALLENGES),
                    Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_ACTIVE_FILL),
                    Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_ACTIVE_OUTLINE),
                    Configuration::get<std::uint8_t>(config::FONT_CHALLENGES_ITEM_SIZE)
                };
                m_textChallenges[l].push_back(std::make_shared<ui::Text>(settingsChallenge));

                topChallenge += alphabetItem.getGlobalBounds().height * 1.5f;
            }

            // Add in the info for challenges met that weren't pre-defined
            auto scoring = Scoring::instance().getLevelChallenges(level->getUUID());
            for (auto&& challenge : scoring)
            {
                // Have to check in with the level to see if this is pre-defined
                if (!level->matchChallenge(challenge).has_value())
                {
                    m_statusChallenges[l].push_back({ false, Scoring::instance().isChallengeMet(level->getUUID(), challenge) });

                    ui::Text::Settings settingsChallenge{
                        false,
                        m_boxDetail.getPosition().x + 1, topChallenge,
                        Scoring::formatChallengeFriendly(challenge),
                        Content::get<sf::Font>(content::KEY_FONT_CHALLENGES),
                        Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_ACTIVE_FILL),
                        Configuration::get<sf::Color>(config::FONT_CHALLENGES_ITEM_COLOR_ACTIVE_OUTLINE),
                        Configuration::get<std::uint8_t>(config::FONT_CHALLENGES_ITEM_SIZE)
                    };
                    m_textChallenges[l].push_back(std::make_shared<ui::Text>(settingsChallenge));

                    topChallenge += alphabetItem.getGlobalBounds().height * 1.5f;
                }
            }
        }

        //
        // Go through the menu items and set their regions, because for whatever reason
        // they don't get set automatically.
        for (auto&& item : m_menuItems)
        {
            item->setRegion({ item->getRegion().left,
                              item->getRegion().top,
                              item->getRegion().width,
                              alphabetItem.getGlobalBounds().height * 1.25f }); // TODO: This 1.25f should not be necessary!!
        }
    }

    void Puzzles::repositionLevelItems()
    {
        // Need to stop all the visible menu items so they no longer receive or
        // respond to events (like the mouse).
        for (std::uint8_t puzzle = m_visibleStart; puzzle < m_visibleStart + m_visibleCount; puzzle++)
        {
            m_menuItems[puzzle]->stop();
        }

        auto coords = Configuration::getGraphics().getViewCoordinates();
        auto top = -(coords.height / 2.0f) + (coords.height / 2.0f) * OFFSET_TOP;
        top += m_headerPuzzles.getRegion().height * 2.5f;

        for (std::uint8_t puzzle = m_visibleStart; puzzle < m_visibleStart + m_visibleCount; puzzle++)
        {
            auto menuItem = m_menuItems[puzzle];
            auto region = menuItem->getRegion();
            menuItem->setPosition({ region.left, top });
            top += menuItem->getRegion().height * 1.25f; // The 1.25f doesn't make any sense at all!

            // Go ahead and re-enable events for this item
            menuItem->start();
        }
    }

    void Puzzles::initializeSprites()
    {
        auto texEmpty = Content::get<sf::Texture>(content::KEY_IMAGE_SCORING_CHECKMARK_EMPTY);
        m_boxEmpty.setTexture(*texEmpty);
        m_boxEmpty.setOrigin({ texEmpty->getSize().x / 2.0f, texEmpty->getSize().y / 2.0f });
        m_boxEmpty.setScale(math::getViewScale({ 2.0f, 2.0f }, m_boxEmpty.getTexture()));

        auto texFilled = Content::get<sf::Texture>(content::KEY_IMAGE_SCORING_CHECKMARK_FILLED);
        m_boxFilled.setTexture(*texFilled);
        m_boxFilled.setOrigin({ texFilled->getSize().x / 2.0f, texFilled->getSize().y / 2.0f });
        m_boxFilled.setScale(math::getViewScale({ 2.0f, 2.0f }, m_boxFilled.getTexture()));

        auto texFilledBC = Content::get<sf::Texture>(content::KEY_IMAGE_SCORING_CHECKMARK_BC_FILLED);
        m_boxFilledBC.setTexture(*texFilledBC);
        m_boxFilledBC.setOrigin({ texFilledBC->getSize().x / 2.0f, texFilledBC->getSize().y / 2.0f });
        m_boxFilledBC.setScale(math::getViewScale({ 2.0f, 2.0f }, m_boxFilledBC.getTexture()));
    }

    void Puzzles::registerInputHanlders()
    {
        for (auto&& menuItem : m_menuItems)
        {
            menuItem->start();
        }

        // Mouse wheel handler to scroll the list of puzzles
        m_mouseWheelHandlerId = MouseInput::instance().registerMouseWheelHandler(
            [this](sf::Event::MouseWheelScrollEvent event, const std::chrono::microseconds)
            {
                onMouseWheel(event);
            });
    }
} // namespace views
