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

#include "RendererHexGridAnimatedSprites.hpp"

#include "components/Position.hpp"
#include "components/Property.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

namespace systems
{
    RendererHexGridAnimatedSprites::RendererHexGridAnimatedSprites(std::shared_ptr<Level> level) :
        RendererHexGrid({ ctti::unnamed_type_id<components::Object>(),
                          ctti::unnamed_type_id<components::AnimatedSprite>() },
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

    bool RendererHexGridAnimatedSprites::addEntity(entities::EntityPtr entity)
    {
        bool added{ false };
        if (added = RendererHexGrid::addEntity(entity); added)
        {
            std::size_t index = entity->getComponent<components::Object>()->renderSequence();
            m_texture[index] = entity->getComponent<components::AnimatedSprite>()->getSprite()->getTexture();
            // Also have to add the same texture into the "I" texture buffers
            if (index < static_cast<std::size_t>(components::ObjectType::I_Wall))
            {
                index += static_cast<std::size_t>(components::ObjectType::I_Offset);
                m_texture[index] = entity->getComponent<components::AnimatedSprite>()->getSprite()->getTexture();
            }
        }

        return added;
    }

    void RendererHexGridAnimatedSprites::updatedEntity(entities::EntityPtr entity)
    {
        if (m_entities.contains(entity->getId()) && System::isInterested(entity))
        {
            // It may have had its texture changed, so need to grab it, might also have had the animated sprite removed
            std::size_t index = entity->getComponent<components::Object>()->renderSequence();
            m_texture[index] = entity->getComponent<components::AnimatedSprite>()->getSprite()->getTexture();
            // Also have to add the same texture into the "I" texture buffers
            if (index < static_cast<std::size_t>(components::ObjectType::I_Wall))
            {
                index += static_cast<std::size_t>(components::ObjectType::I_Offset);
                m_texture[index] = entity->getComponent<components::AnimatedSprite>()->getSprite()->getTexture();
            }
        }
        else
        {
            RendererHexGrid::removeEntity(entity->getId());
        }
    }

    //
    // ---------------------- Rendering ----------------------
    //

    void RendererHexGridAnimatedSprites::initUpdate([[maybe_unused]] std::chrono::microseconds elapsedTime, [[maybe_unused]] std::uint16_t startR, [[maybe_unused]] std::uint16_t endR, [[maybe_unused]] std::uint16_t startQ, [[maybe_unused]] std::uint16_t endQ, [[maybe_unused]] std::uint16_t numberR, [[maybe_unused]] std::uint16_t numberQ, [[maybe_unused]] const math::Dimension2f& coords)
    {
        for (auto&& howManyToDraw : m_howManyToDraw)
        {
            howManyToDraw = 0;
        }
    }

    void RendererHexGridAnimatedSprites::perCell([[maybe_unused]] sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY)
    {
        for (auto&& [order, entity] : m_level->getEntitiesByRender(cell))
        {
            if (entity->hasComponent<components::AnimatedSprite>())
            {
                auto sprite = entity->getComponent<components::AnimatedSprite>();
                std::size_t bufferIndex = entity->getComponent<components::Object>()->renderSequence();
                // Ensure any "color" entity that also has an "I" property is rendered on top of all other color entities
                if (entity->hasComponent<components::Property>() && entity->getComponent<components::Property>()->has(components::PropertyType::I))
                {
                    if (bufferIndex < static_cast<std::size_t>(components::ObjectType::I_Wall))
                    {
                        bufferIndex += static_cast<std::size_t>(components::ObjectType::I_Offset);
                    }
                }

                auto vertex = m_howManyToDraw[bufferIndex];

                m_cells[bufferIndex][vertex + 0].position = sf::Vector2f(posX, posY);
                m_cells[bufferIndex][vertex + 1].position = sf::Vector2f(posX + renderDimX, posY);
                m_cells[bufferIndex][vertex + 2].position = sf::Vector2f(posX + renderDimX, posY + renderDimY);
                m_cells[bufferIndex][vertex + 3].position = sf::Vector2f(posX, posY + renderDimY);

                auto textureRect = entity->getComponent<components::AnimatedSprite>()->getCurrentSpriteRect();
                m_cells[bufferIndex][vertex + 0].texCoords = sf::Vector2f(static_cast<float>(textureRect.left), static_cast<float>(textureRect.top));
                m_cells[bufferIndex][vertex + 1].texCoords = sf::Vector2f(static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top));
                m_cells[bufferIndex][vertex + 2].texCoords = sf::Vector2f(static_cast<float>(textureRect.left + textureRect.width), static_cast<float>(textureRect.top + textureRect.height));
                m_cells[bufferIndex][vertex + 3].texCoords = sf::Vector2f(static_cast<float>(textureRect.left), static_cast<float>(textureRect.top + textureRect.height));

                // For text types, determine the complimentary color based on the background entity
                auto color = sprite->getSpriteColor();
                if (entity->hasComponent<components::Object>() && entity->getComponent<components::Object>()->getType() == components::ObjectType::Text)
                {
                    color = lookupComplementaryColor(cell);
                }

                m_cells[bufferIndex][vertex + 0].color = color;
                m_cells[bufferIndex][vertex + 1].color = color;
                m_cells[bufferIndex][vertex + 2].color = color;
                m_cells[bufferIndex][vertex + 3].color = color;

                m_howManyToDraw[bufferIndex] += 4;
            }
        }
    }

    void RendererHexGridAnimatedSprites::finalizeUpdate(sf::RenderTarget& renderTarget)
    {
        for (std::uint8_t type = 0; type < m_buffer.size(); type++)
        {
            // Don't waste any time drawing nothing...this improved the
            // in-puzzle rendering by 25% to 30%
            if (m_howManyToDraw[type] > 0)
            {
                m_states[type].texture = m_texture[type];
                m_buffer[type].update(m_cells[type].data());

                renderTarget.draw(m_buffer[type], 0, m_howManyToDraw[type], m_states[type]);
            }
        }
    }

    // --------------------------------------------------------------
    //
    // Reference for picking complementary colors: https://giggster.com/guide/complementary-colors/
    //
    // --------------------------------------------------------------
    sf::Color RendererHexGridAnimatedSprites::lookupComplementaryColor(const misc::HexCoord& cell)
    {
        // Figure out what we'll consider as the background cell color
        sf::Color complementaryColor = sf::Color::Black;
        for (auto&& [order, entity] : m_level->getEntitiesByRender(cell))
        {
            if (entity->hasComponent<components::Object>())
            {
                switch (entity->getComponent<components::Object>()->getType())
                {
                    case components::ObjectType::Wall:
                        complementaryColor = sf::Color(255, 128, 0);
                        break;
                    case components::ObjectType::Floor:
                        complementaryColor = sf::Color::Black;
                        break;
                    case components::ObjectType::Purple:
                    case components::ObjectType::Background_Purple:
                        complementaryColor = sf::Color(141, 145, 13);
                        break;
                    case components::ObjectType::Grey:
                    case components::ObjectType::Background_Grey:
                        complementaryColor = sf::Color::White;
                        break;
                    case components::ObjectType::Green:
                    case components::ObjectType::Background_Green:
                        complementaryColor = sf::Color(0, 255, 255);
                        break;
                    case components::ObjectType::Blue:
                    case components::ObjectType::Background_Blue:
                        complementaryColor = sf::Color(255, 128, 0);
                        break;
                    case components::ObjectType::Red:
                    case components::ObjectType::Background_Red:
                        complementaryColor = sf::Color(0, 255, 0);
                        break;
                    case components::ObjectType::Brown:
                    case components::ObjectType::Background_Brown:
                        complementaryColor = sf::Color(255, 255, 0);
                        break;
                    case components::ObjectType::Yellow:
                    case components::ObjectType::Background_Yellow:
                        complementaryColor = sf::Color(0, 0, 0);
                        break;
                    case components::ObjectType::Black:
                        complementaryColor = sf::Color(255, 255, 255);
                        break;
                    case components::ObjectType::Background_White:
                        complementaryColor = sf::Color(0, 0, 0);
                        break;
                    default: // Do Nothing
                        break;
                }
            }
        }

        return complementaryColor;
    }

} // namespace systems
