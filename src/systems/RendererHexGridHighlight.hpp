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

#pragma once

#include "RendererHexGrid.hpp"
#include "components/AnimatedSprite.hpp"
#include "entities/Entity.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <functional>
#include <unordered_map>
#include <vector>

namespace systems
{

    // --------------------------------------------------------------
    //
    // This is a base system used to render the a highlight animation
    //
    // --------------------------------------------------------------
    class RendererHexGridHighlight : public RendererHexGrid
    {
      public:
        RendererHexGridHighlight(std::shared_ptr<Level> level, std::function<void(entities::EntityPtr)> addEntity);
        RendererHexGridHighlight(const std::initializer_list<ctti::unnamed_type_id_t>& list, std::shared_ptr<Level> level, std::function<void(entities::EntityPtr)> addEntity);

        void clear() override;
        bool addEntity(entities::EntityPtr entity) override;
        void removeEntity(entities::Entity::IdType entityId) override;
        void updatedEntity(entities::EntityPtr entity) override;

      protected:
        void initUpdate(std::chrono::microseconds elapsedTime, std::uint16_t startR, std::uint16_t endR, std::uint16_t startQ, std::uint16_t endQ, std::uint16_t numberR, std::uint16_t numberQ, const math::Dimension2f& coords) override;
        void perCell(sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY) override;
        void finalizeUpdate(sf::RenderTarget& renderTarget) override;

        std::function<void(entities::EntityPtr)> m_addEntity;
        entities::EntityPtr m_highlight;
        const sf::Texture* m_texture{ nullptr };

      private:
        std::vector<std::vector<std::uint16_t>> m_gridTypeCount;
        std::unordered_map<entities::Entity::IdType, misc::HexCoord> m_idToCoord;

        sf::VertexBuffer m_buffer;
        std::vector<sf::Vertex> m_cells;
        std::size_t m_howManyToDraw{ 0 };
        sf::RenderStates m_state;
    };
} // namespace systems
