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

#include "RendererHexGridHighlight.hpp"

#include "components/Noun.hpp"
#include "components/Object.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"

#include <initializer_list>
#include <ranges>

namespace systems
{
    RendererHexGridHighlight::RendererHexGridHighlight(std::shared_ptr<Level> level, std::function<void(entities::EntityPtr)> addEntity) :
        RendererHexGridHighlight({ ctti::unnamed_type_id<components::Object>(),
                                   ctti::unnamed_type_id<components::Position>(),
                                   ctti::unnamed_type_id<components::Property>() },
                                 level,
                                 addEntity)
    {
    }

    RendererHexGridHighlight::RendererHexGridHighlight(const std::initializer_list<ctti::unnamed_type_id_t>& list, std::shared_ptr<Level> level, std::function<void(entities::EntityPtr)> addEntity) :
        RendererHexGrid(list, level),
        m_addEntity(addEntity)
    {
        // Create an array that is the same number of cells as the level, used to keep
        // a count of entities in each of them.
        m_gridTypeCount.resize(level->getHeight());
        for (auto&& row : m_gridTypeCount)
        {
            row.resize(level->getWidth(), static_cast<std::uint16_t>(0));
        }

        // Prepare the various buffers for rendering
        m_cells.resize(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4);
        m_buffer.create(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4);
        m_buffer.setPrimitiveType(sf::PrimitiveType::Quads);
        m_buffer.setUsage(sf::VertexBuffer::Usage::Dynamic);
    }

    void RendererHexGridHighlight::clear()
    {
        System::clear();
        for (auto&& row : m_gridTypeCount)
        {
            std::ranges::fill(row, static_cast<std::uint16_t>(0));
        }

        // Yes, we just cleared out all the entities, but we need to re-notify everyone about this entity so that
        // the AnimatedSprite system keeps updating it.
        m_addEntity(m_highlight);
    }
    bool RendererHexGridHighlight::addEntity(entities::EntityPtr entity)
    {
        bool added{ false };
        if (System::addEntity(entity))
        {
            auto position = entity->getComponent<components::Position>()->get();
            m_gridTypeCount[position.r][position.q]++;
            // We also remember this entity's current position, because it might change
            // in the future, we need to know that so its count can be correctly subtracted in the future
            m_idToCoord[entity->getId()] = position;
        }

        return added;
    }
    void RendererHexGridHighlight::removeEntity(entities::Entity::IdType entityId)
    {
        if (m_idToCoord.contains(entityId))
        {
            // We remove by the coord we are tracking, in addition to updating the grid position count
            auto cell = m_idToCoord[entityId];
            m_gridTypeCount[cell.r][cell.q]--;
            m_idToCoord.erase(entityId);
        }

        System::removeEntity(entityId);
    }
    void RendererHexGridHighlight::updatedEntity(entities::EntityPtr entity)
    {
        // If we previously knew about this entity, then let's update
        if (m_idToCoord.contains(entity->getId()))
        {
            // If we are no longer interested, then remove it
            if (!isInterested(entity))
            {
                auto cell = m_idToCoord[entity->getId()];
                m_gridTypeCount[cell.r][cell.q]--;
                m_idToCoord.erase(entity->getId());
            }
            else
            {
                // If the entity coordinate changed, we have to update some things
                auto position = entity->getComponent<components::Position>()->get();
                if (position != m_idToCoord[entity->getId()])
                {
                    auto cell = m_idToCoord[entity->getId()];
                    // Old position count is subtracted
                    m_gridTypeCount[cell.r][cell.q]--;
                    // New position count is incremented
                    m_gridTypeCount[position.r][position.q]++;
                    // Replace with the current position
                    m_idToCoord[entity->getId()] = position;
                }
            }
        }
        else // If we didn't know about this entity, but are now interested, add it to our tracking
        {
            addEntity(entity);
        }
    }

    //
    // ---------------------- Rendering ----------------------
    //

    void RendererHexGridHighlight::initUpdate([[maybe_unused]] std::chrono::microseconds elapsedTime, [[maybe_unused]] std::uint16_t startR, [[maybe_unused]] std::uint16_t endR, [[maybe_unused]] std::uint16_t startQ, [[maybe_unused]] std::uint16_t endQ, [[maybe_unused]] std::uint16_t numberR, [[maybe_unused]] std::uint16_t numberQ, [[maybe_unused]] const math::Dimension2f& coords)
    {
        m_howManyToDraw = 0;
    }

    void RendererHexGridHighlight::perCell([[maybe_unused]] sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY)
    {
        // See if we are tracking anyone in this cell
        if (m_gridTypeCount[cell.r][cell.q] > 0)
        {
            auto sprite = m_highlight->getComponent<components::AnimatedSprite>();

            auto vertex = m_howManyToDraw;

            m_cells[vertex + 0].position = sf::Vector2f(posX, posY);
            m_cells[vertex + 1].position = sf::Vector2f(posX + renderDimX, posY);
            m_cells[vertex + 2].position = sf::Vector2f(posX + renderDimX, posY + renderDimY);
            m_cells[vertex + 3].position = sf::Vector2f(posX, posY + renderDimY);

            auto textureRect = m_highlight->getComponent<components::AnimatedSprite>()->getCurrentSpriteRect();
            m_cells[vertex + 0].texCoords = sf::Vector2f(static_cast<float>(textureRect.left), static_cast<float>(textureRect.top));
            m_cells[vertex + 1].texCoords = sf::Vector2f(static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top));
            m_cells[vertex + 2].texCoords = sf::Vector2f(static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top + textureRect.height));
            m_cells[vertex + 3].texCoords = sf::Vector2f(static_cast<float>(textureRect.left), static_cast<float>(textureRect.top + textureRect.height));

            m_cells[vertex + 0].color = sprite->getSpriteColor();
            m_cells[vertex + 1].color = sprite->getSpriteColor();
            m_cells[vertex + 2].color = sprite->getSpriteColor();
            m_cells[vertex + 3].color = sprite->getSpriteColor();

            m_howManyToDraw += 4;
        }
    }

    void RendererHexGridHighlight::finalizeUpdate(sf::RenderTarget& renderTarget)
    {
        m_state.texture = m_texture;
        m_buffer.update(m_cells.data());
        renderTarget.draw(m_buffer, 0, m_howManyToDraw, m_state);
    }

} // namespace systems
