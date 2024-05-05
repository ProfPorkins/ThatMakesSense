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
#include "components/Object.hpp"
#include "misc/math.hpp"

#include <SFML/Graphics.hpp>
#include <array>
#include <unordered_map>
#include <vector>

namespace systems
{

    // --------------------------------------------------------------
    //
    // This system is used to render AnimatedSprites in the hex grid
    //
    // --------------------------------------------------------------
    class RendererHexGridAnimatedSprites : public RendererHexGrid
    {
      public:
        RendererHexGridAnimatedSprites(std::shared_ptr<Level> level);

        bool addEntity(entities::EntityPtr entity) override;
        void updatedEntity(entities::EntityPtr entity) override;

      protected:
        void initUpdate(std::chrono::microseconds elapsedTime, std::uint16_t startR, std::uint16_t endR, std::uint16_t startQ, std::uint16_t endQ, std::uint16_t numberR, std::uint16_t numberQ, const math::Dimension2f& coords) override;
        void perCell(sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY) override;
        void finalizeUpdate(sf::RenderTarget& renderTarget) override;

      private:
        // One vertex buffer (and related) for each Object type, as they correspond to different textures
        // NOTE: Could consider putting all textures at the same rendering order into a single texture
        //       to reduce the number of collections and draw calls.
        std::array<sf::VertexBuffer, components::Object::TYPE_SIZE> m_buffer;
        std::array<std::vector<sf::Vertex>, components::Object::TYPE_SIZE> m_cells;
        std::array<std::size_t, components::Object::TYPE_SIZE> m_howManyToDraw{ 0 };
        std::array<sf::RenderStates, components::Object::TYPE_SIZE> m_states;
        std::array<const sf::Texture*, components::Object::TYPE_SIZE> m_texture;

        sf::Color lookupComplementaryColor(const misc::HexCoord& cell);
    };
} // namespace systems
