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
#include "systems/parser/PhraseSearch.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>
#include <optional>
#include <unordered_set>

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system is used to render the phrase direction markers
    //
    // --------------------------------------------------------------
    class RendererHexGridPhraseDirection : public RendererHexGrid
    {
      public:
        RendererHexGridPhraseDirection(std::shared_ptr<Level> level);

        void clear() override;
        bool addEntity(entities::EntityPtr entity) override;
        void removeEntity(entities::Entity::IdType entityId) override;
        void shutdown() override;

      protected:
        bool isInterested(const entities::EntityPtr& entity) override;
        void initUpdate(std::chrono::microseconds elapsedTime, std::uint16_t startR, std::uint16_t endR, std::uint16_t startQ, std::uint16_t endQ, std::uint16_t numberR, std::uint16_t numberQ, const math::Dimension2f& coords) override;
        void perCell(sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY) override;
        void finalizeUpdate(sf::RenderTarget& renderTarget) override;

      private:
        bool m_renderAllArrows{ false };
        std::shared_ptr<sf::Sprite> m_sprite;
        components::PhraseDirection::DirectionGrid m_directions;
        entities::EntityPtr m_camera{ nullptr };
        std::optional<misc::HexCoord> m_mouseStartCell{ std::nullopt };
        std::unordered_set<misc::HexCoord> m_mouseHoverCells;
        std::uint32_t m_keyboardHandlerId{ 0 };
        std::uint32_t m_controllerHandlerId{ 0 };
        std::uint32_t m_mouseMovedHandlerId{ 0 };

        std::shared_ptr<sf::Texture> m_texture;
        sf::Color m_color;
        sf::VertexBuffer m_buffer{ sf::PrimitiveType::Quads, sf::VertexBuffer::Usage::Dynamic };
        std::vector<sf::Vertex> m_cells;
        std::size_t m_howManyToDraw{ 0 };
        sf::RenderStates m_states;

        void mouseMoved(math::Point2f point);
        void searchPhrases(const misc::HexCoord& cell, const std::unordered_set<std::uint16_t>& phraseIds, std::unordered_set<misc::HexCoord>& phraseCells);
    };
} // namespace systems
