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

#include "RendererHexGridPhraseDirection.hpp"

#include "components/Camera.hpp"
#include "components/PhraseDirection.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"
#include "services/ControllerInput.hpp"
#include "services/KeyboardInput.hpp"
#include "services/MouseInput.hpp"

#include <chrono>
#include <ranges>

namespace systems
{
    RendererHexGridPhraseDirection::RendererHexGridPhraseDirection(std::shared_ptr<Level> level) :
        RendererHexGrid({ ctti::unnamed_type_id<components::PhraseDirection>() }, level)
    {
        m_texture = Content::instance().get<sf::Texture>(content::KEY_IMAGE_PHRASE_DIRECTION_1024);
        m_states.texture = &(*m_texture);
        m_color = Configuration::get<sf::Color>(config::IMAGE_PHRASE_DIRECTION_1024_COLOR);

        // Size everything for the max possible number of items, but will only draw what is actually visible
        // Max items is 5 direction arrows in each cell and 4 verts per each arrow
        m_cells.resize(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4 * 5);
        m_buffer.create(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4 * 5);

        m_directions.resize(level->getHeight());
        for (auto&& row : m_directions)
        {
            row.resize(level->getWidth());
        }

        m_keyboardHandlerId = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_TOGGLE_PHRASE_RENDERING),
            KeyboardInput::Modifier::None,
            false,
            std::chrono::microseconds::zero(),
            [this](std::chrono::microseconds)
            {
                m_renderAllArrows = !m_renderAllArrows;
            });

        m_controllerHandlerId = ControllerInput::instance().registerButtonDownHandler(
            ControllerInput::Button::Bottom,
            false,
            std::chrono::microseconds::zero(),
            [this](ControllerInput::Button, std::chrono::microseconds)
            {
                m_renderAllArrows = !m_renderAllArrows;
            });

        m_mouseMovedHandlerId = MouseInput::instance().registerMouseMovedHandler(
            [this](math::Point2f point, const std::chrono::microseconds)
            {
                mouseMoved(point);
            });
    }

    // --------------------------------------------------------------
    //
    // Because this system holds state, and I know it shouldn't, have
    // to override the ::clear method to clean that state up whenever
    // all entities are all of the sudden removed.
    //
    // --------------------------------------------------------------
    void RendererHexGridPhraseDirection::clear()
    {
        System::clear();
        // We'll just rebuild the array of directions
        auto rowSize = m_directions[0].size();
        for (auto&& row : m_directions)
        {
            row.clear();
            row.resize(rowSize);
        }
    }

    // --------------------------------------------------------------
    //
    // Have to deal with the Camera entity in a specific way.
    //
    // --------------------------------------------------------------
    bool RendererHexGridPhraseDirection::addEntity(entities::EntityPtr entity)
    {
        bool added{ false };
        if (RendererHexGrid::addEntity(entity))
        {
            added = true;
            // Yes, we want to add the entity, but have a special case for the Camera entity
            if (!entity->hasComponent<components::Camera>())
            {
                auto phraseInfo = entity->getComponent<components::PhraseDirection>();
                m_directions[phraseInfo->getPosition().r][phraseInfo->getPosition().q].insert({ phraseInfo->getId(), { phraseInfo->getDirection(), phraseInfo->getElement() } });
            }
            else
            {
                m_camera = entity;
            }
        }
        return added;
    }

    void RendererHexGridPhraseDirection::removeEntity(entities::Entity::IdType entityId)
    {
        if (m_entities.contains(entityId))
        {
            auto phraseInfo = m_entities[entityId]->getComponent<components::PhraseDirection>();
            m_directions[phraseInfo->getPosition().r][phraseInfo->getPosition().q].erase(phraseInfo->getId());
        }
    }

    void RendererHexGridPhraseDirection::shutdown()
    {
        KeyboardInput::instance().unregisterKeyDownHandler(m_keyboardHandlerId);
        ControllerInput::instance().unregisterButtonDownHandler(m_controllerHandlerId);
        MouseInput::instance().unregisterMouseMovedHandler(m_mouseMovedHandlerId);
    }

    // --------------------------------------------------------------
    //
    // Still interested in PhraseDirection entities, but also want
    // to grab ahold of the Camera entity when it comes by.
    //
    // --------------------------------------------------------------
    bool RendererHexGridPhraseDirection::isInterested(const entities::EntityPtr& entity)
    {
        return System::isInterested(entity) || entity->hasComponent<components::Camera>();
    }

    void RendererHexGridPhraseDirection::initUpdate([[maybe_unused]] std::chrono::microseconds elapsedTime, [[maybe_unused]] std::uint16_t startR, [[maybe_unused]] std::uint16_t endR, [[maybe_unused]] std::uint16_t startQ, [[maybe_unused]] std::uint16_t endQ, [[maybe_unused]] std::uint16_t numberR, [[maybe_unused]] std::uint16_t numberQ, [[maybe_unused]] const math::Dimension2f& coords)
    {
        m_howManyToDraw = 0;
    }

    void RendererHexGridPhraseDirection::perCell([[maybe_unused]] sf::RenderTarget& renderTarget, [[maybe_unused]] misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY)
    {
        // The test using m_howManyToDraw prevents a crash in the case too many direction arrows are being attempted to render
        if (m_howManyToDraw < m_cells.size() && (m_renderAllArrows || (m_mouseStartCell.has_value() && m_mouseHoverCells.contains(cell))))
        {
            // Because phrases can have a lot of overlapping duplicates, this is used to very significantly reduce
            // drawing the same direction arrows over and over, as in 10X (or more!) reduction in some cases.
            // Yes, using an old-style raw array because it should be faster to create than std::array...but I didn't actually measure it
            // Probably faster to create it as a class member and then set all 6 locations to false each time, but that is
            // more than really necessary.
            bool uniqueDirs[6]{ false };

            for (auto&& data : m_directions[cell.r][cell.q] | std::views::values)
            {
                if (!uniqueDirs[static_cast<std::uint8_t>(std::get<components::PhraseDirection::GridDirection>(data))])
                {
                    auto index = m_howManyToDraw;

                    float posOffsetX{ 0 };
                    float posOffsetY{ 0 };

                    float texStartX{ 0 };
                    float texEndX{ 0 };
                    float texStartY{ 0 };
                    float texEndY{ 0 };

                    switch (std::get<components::PhraseDirection::GridDirection>(data))
                    {
                        case misc::HexCoord::Direction::E:
                            posOffsetX = renderDimX / 2;
                            texStartX = 0;
                            texEndX = m_texture->getSize().x / 3.0f;
                            texStartY = 0;
                            texEndY = m_texture->getSize().y / 2.0f;
                            break;
                        case misc::HexCoord::Direction::NE:
                            posOffsetY = -(renderDimY * misc::HEX_VERTICAL_DISTANCE / 2);
                            texStartX = m_texture->getSize().x / 3.0f;
                            texEndX = (m_texture->getSize().x / 3.0f) * 2;
                            texStartY = 0;
                            texEndY = m_texture->getSize().y / 2.0f;
                            break;
                        case misc::HexCoord::Direction::NW:
                            posOffsetY = -(renderDimY * misc::HEX_VERTICAL_DISTANCE / 2);
                            texStartX = (m_texture->getSize().x / 3.0f) * 2;
                            texEndX = (m_texture->getSize().x / 3.0f) * 3;
                            texStartY = 0;
                            texEndY = m_texture->getSize().y / 2.0f;
                            break;
                        case misc::HexCoord::Direction::W:
                            posOffsetX = -(renderDimX / 2);
                            texStartX = 0;
                            texEndX = m_texture->getSize().x / 3.0f;
                            texStartY = m_texture->getSize().y / 2.0f;
                            texEndY = (m_texture->getSize().y / 2.0f) * 2;
                            break;
                        case misc::HexCoord::Direction::SE:
                            posOffsetY = (renderDimY * misc::HEX_VERTICAL_DISTANCE / 2);
                            texStartX = m_texture->getSize().x / 3.0f;
                            texEndX = (m_texture->getSize().x / 3.0f) * 2;
                            texStartY = m_texture->getSize().y / 2.0f;
                            texEndY = (m_texture->getSize().y / 2.0f) * 2;
                            break;
                        case misc::HexCoord::Direction::SW:
                            posOffsetY = (renderDimY * misc::HEX_VERTICAL_DISTANCE / 2);
                            texStartX = (m_texture->getSize().x / 3.0f) * 2;
                            texEndX = (m_texture->getSize().x / 3.0f) * 3;
                            texStartY = m_texture->getSize().y / 2.0f;
                            texEndY = (m_texture->getSize().y / 2.0f) * 2;
                            break;
                    }

                    m_cells[index + 0].position = sf::Vector2f(posX + posOffsetX, posY + posOffsetY);
                    m_cells[index + 1].position = sf::Vector2f(posX + renderDimX + posOffsetX, posY + posOffsetY);
                    m_cells[index + 2].position = sf::Vector2f(posX + renderDimX + posOffsetX, posY + renderDimY + posOffsetY);
                    m_cells[index + 3].position = sf::Vector2f(posX + posOffsetX, posY + renderDimY + posOffsetY);

                    m_cells[index + 0].texCoords = sf::Vector2f(texStartX, texStartY);
                    m_cells[index + 1].texCoords = sf::Vector2f(texEndX, texStartY);
                    m_cells[index + 2].texCoords = sf::Vector2f(texEndX, texEndY);
                    m_cells[index + 3].texCoords = sf::Vector2f(texStartX, texEndY);

                    m_cells[index + 0].color = sf::Color(m_color);
                    m_cells[index + 1].color = sf::Color(m_color);
                    m_cells[index + 2].color = sf::Color(m_color);
                    m_cells[index + 3].color = sf::Color(m_color);

                    m_howManyToDraw += 4;

                    uniqueDirs[static_cast<std::uint8_t>(std::get<components::PhraseDirection::GridDirection>(data))] = true;
                }
            }
        }
    }

    void RendererHexGridPhraseDirection::finalizeUpdate(sf::RenderTarget& renderTarget)
    {
        if (m_renderAllArrows || m_mouseStartCell.has_value())
        {
            m_buffer.update(m_cells.data());
            renderTarget.draw(m_buffer, 0, m_howManyToDraw, m_states);
        }
    }

    std::unordered_set<std::uint16_t> getPhraseStartIds(const components::PhraseDirection::DirectionGrid& directions, misc::HexCoord cell)
    {
        std::unordered_set<std::uint16_t> phraseIds;

        for (auto&& [id, data] : directions[cell.r][cell.q])
        {
            if (std::get<1>(data) == components::PhraseDirection::PhraseElement::Start)
            {
                phraseIds.insert(id);
            }
        }

        return phraseIds;
    }

    bool hasPhraseId(misc::HexCoord cell, const components::PhraseDirection::DirectionGrid& directions, const std::unordered_set<std::uint16_t>& phraseIds)
    {
        for (auto&& id : directions[cell.r][cell.q] | std::views::keys)
        {
            if (phraseIds.contains(id))
            {
                return true;
            }
        }

        return false;
    }

    // --------------------------------------------------------------
    //
    // When the mouse moves, we need to see if it now hovers over a
    // 'start' phrase direction arrow and if so, collect the neighboring
    // cells that are part of the phrase that starts at that location.
    //
    // --------------------------------------------------------------
    void RendererHexGridPhraseDirection::mouseMoved(math::Point2f point)
    {
        auto cameraPtr = m_camera->getComponent<components::Camera>();
        auto cell = misc::HexCoord::pointToHex(
            point,
            misc::computeCellSize(cameraPtr),
            cameraPtr->getCenter());

        if (cell.r >= 0 && cell.q >= 0 && cell.r < static_cast<misc::HexCoord::Type>(m_directions.size()) && cell.q < static_cast<misc::HexCoord::Type>(m_directions[cell.r].size()))
        {
            // Check to see if we need to recurse through and find the new set of cells
            // to render for mouse hover
            if (!m_mouseStartCell.has_value() || (m_mouseStartCell.has_value() && m_mouseStartCell.value() != cell))
            {
                m_mouseStartCell = cell;
                m_mouseHoverCells.clear();
                if (auto phraseIds = getPhraseStartIds(m_directions, cell); !phraseIds.empty())
                {
                    searchPhrases(cell, phraseIds, m_mouseHoverCells);
                }
            }
        }
        else
        {
            m_mouseStartCell = std::nullopt;
        }
    }

    // --------------------------------------------------------------
    //
    // Performs a recursive search to find all phrase directions that
    // are part of any of the set of phraseIds.
    //
    // --------------------------------------------------------------
    void RendererHexGridPhraseDirection::searchPhrases(const misc::HexCoord& cell, const std::unordered_set<std::uint16_t>& phraseIds, std::unordered_set<misc::HexCoord>& phraseCells)
    {
        // Base cases - if we go outside the grid
        if (!cell.isValid(static_cast<std::uint16_t>(m_directions[0].size()), static_cast<std::uint16_t>(m_directions.size())))
        {
            return;
        }

        // Already part of the search
        if (phraseCells.contains(cell))
        {
            return;
        }

        // If no matching phrase id
        if (!hasPhraseId(cell, m_directions, phraseIds))
        {
            return;
        }

        phraseCells.insert(cell);

        // Recursively call around
        searchPhrases(cell.NW(), phraseIds, phraseCells);
        searchPhrases(cell.NE(), phraseIds, phraseCells);
        searchPhrases(cell.W(), phraseIds, phraseCells);
        searchPhrases(cell.E(), phraseIds, phraseCells);
        searchPhrases(cell.SW(), phraseIds, phraseCells);
        searchPhrases(cell.SE(), phraseIds, phraseCells);
    }

} // namespace systems
