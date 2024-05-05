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

#include "RendererHexGridStaticSprites.hpp"

#include "components/Position.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

namespace systems
{
    RendererHexGridStaticSprites::RendererHexGridStaticSprites(std::shared_ptr<Level> level) :
        RendererHexGrid({ ctti::unnamed_type_id<components::Object>(),
                          ctti::unnamed_type_id<components::StaticSprite>() },
                        level)
    {
        // Prepare the various buffers for rendering
        for (std::uint8_t type = 0; type < m_buffer.size(); type++)
        {
            m_cells[type].resize(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4);
            m_buffer[type].create(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4);
            m_buffer[type].setPrimitiveType(sf::PrimitiveType::Quads);
            m_buffer[type].setUsage(sf::VertexBuffer::Usage::Dynamic);
        }
    }

    bool RendererHexGridStaticSprites::addEntity(entities::EntityPtr entity)
    {
        bool added{ false };
        if (added = RendererHexGrid::addEntity(entity); added)
        {
            std::size_t index = entity->getComponent<components::Object>()->renderSequence();
            m_texture[index] = entity->getComponent<components::StaticSprite>()->getSprite()->getTexture();
        }

        return added;
    }

    void RendererHexGridStaticSprites::updatedEntity(entities::EntityPtr entity)
    {
        if (m_entities.contains(entity->getId()) && System::isInterested(entity))
        {
            // It may have had its texture changed, so need to grab it, it might also have had its static sprite removed
            std::size_t index = entity->getComponent<components::Object>()->renderSequence();
            m_texture[index] = entity->getComponent<components::StaticSprite>()->getSprite()->getTexture();
        }
        else
        {
            RendererHexGrid::removeEntity(entity->getId());
        }
    }

    //
    // ---------------------- Rendering ----------------------
    //

    void RendererHexGridStaticSprites::initUpdate([[maybe_unused]] std::chrono::microseconds elapsedTime, [[maybe_unused]] std::uint16_t startR, [[maybe_unused]] std::uint16_t endR, [[maybe_unused]] std::uint16_t startQ, [[maybe_unused]] std::uint16_t endQ, [[maybe_unused]] std::uint16_t numberR, [[maybe_unused]] std::uint16_t numberQ, [[maybe_unused]] const math::Dimension2f& coords)
    {
        for (auto&& howManyToDraw : m_howManyToDraw)
        {
            howManyToDraw = 0;
        }
    }

    void RendererHexGridStaticSprites::perCell([[maybe_unused]] sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY)
    {
        for (auto&& [order, entity] : m_level->getEntitiesByRender(cell))
        {
            if (entity->hasComponent<components::StaticSprite>())
            {
                auto sprite = entity->getComponent<components::StaticSprite>();
                std::size_t bufferIndex = entity->getComponent<components::Object>()->renderSequence();

                auto vertex = m_howManyToDraw[bufferIndex];

                m_cells[bufferIndex][vertex + 0].position = sf::Vector2f(posX, posY);
                m_cells[bufferIndex][vertex + 1].position = sf::Vector2f(posX + renderDimX, posY);
                m_cells[bufferIndex][vertex + 2].position = sf::Vector2f(posX + renderDimX, posY + renderDimY);
                m_cells[bufferIndex][vertex + 3].position = sf::Vector2f(posX, posY + renderDimY);

                auto textureRect = entity->getComponent<components::StaticSprite>()->getCurrentSpriteRect();
                m_cells[bufferIndex][vertex + 0].texCoords = sf::Vector2f(static_cast<float>(textureRect.left), static_cast<float>(textureRect.top));
                m_cells[bufferIndex][vertex + 1].texCoords = sf::Vector2f(static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top));
                m_cells[bufferIndex][vertex + 2].texCoords = sf::Vector2f(static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top + textureRect.height));
                m_cells[bufferIndex][vertex + 3].texCoords = sf::Vector2f(static_cast<float>(textureRect.left), static_cast<float>(textureRect.top + textureRect.height));

                m_cells[bufferIndex][vertex + 0].color = sprite->getSpriteColor();
                m_cells[bufferIndex][vertex + 1].color = sprite->getSpriteColor();
                m_cells[bufferIndex][vertex + 2].color = sprite->getSpriteColor();
                m_cells[bufferIndex][vertex + 3].color = sprite->getSpriteColor();

                m_howManyToDraw[bufferIndex] += 4;
            }
        }
    }

    void RendererHexGridStaticSprites::finalizeUpdate(sf::RenderTarget& renderTarget)
    {
        for (std::uint8_t type = 0; type < m_buffer.size(); type++)
        {
            m_states[type].texture = m_texture[type];
            m_buffer[type].update(m_cells[type].data());

            renderTarget.draw(m_buffer[type], 0, m_howManyToDraw[type], m_states[type]);
        }
    }

} // namespace systems
