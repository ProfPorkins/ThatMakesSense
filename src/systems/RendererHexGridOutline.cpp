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

#include "RendererHexGridOutline.hpp"

#include "components/Camera.hpp"
#include "services//Configuration.hpp"
#include "services//ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

namespace systems
{
    RendererHexGridOutline::RendererHexGridOutline(std::shared_ptr<Level> level) :
        RendererHexGrid({ /* Doesn't specify any interest because it only cares about coordinates */ }, level)
    {
        m_texture = Content::instance().get<sf::Texture>(content::KEY_IMAGE_HEX_OUTLINE_256);
        m_states.texture = &(*m_texture);

        m_color = Configuration::get<sf::Color>(config::IMAGE_HEX_OUTLINE_256_COLOR);

        // Size everything for the max possible number of items, but will only draw what is actually visible
        m_cells.resize(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4);
        m_buffer.create(static_cast<std::size_t>(level->getHeight()) * static_cast<std::size_t>(level->getWidth()) * 4);
    }

    void RendererHexGridOutline::initUpdate([[maybe_unused]] std::chrono::microseconds elapsedTime, [[maybe_unused]] std::uint16_t startR, [[maybe_unused]] std::uint16_t endR, [[maybe_unused]] std::uint16_t startQ, [[maybe_unused]] std::uint16_t endQ, [[maybe_unused]] std::uint16_t numberR, [[maybe_unused]] std::uint16_t numberQ, [[maybe_unused]] const math::Dimension2f& coords)
    {
        if (didCameraChange())
        {
            m_howManyToDraw = 0;
        }
    }

    void RendererHexGridOutline::perCell([[maybe_unused]] sf::RenderTarget& renderTarget, [[maybe_unused]] misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY)
    {
        // If there are no entities at this location, we don't draw anything.  This allows for
        // levels to have other than square shapes.
        if (didCameraChange() && m_level->getEntities(cell).size() > 0)
        {
            auto index = m_howManyToDraw;
            m_cells[index + 0].position = sf::Vector2f(posX, posY);
            m_cells[index + 1].position = sf::Vector2f(posX + renderDimX, posY);
            m_cells[index + 2].position = sf::Vector2f(posX + renderDimX, posY + renderDimY);
            m_cells[index + 3].position = sf::Vector2f(posX, posY + renderDimY);

            m_cells[index + 0].texCoords = sf::Vector2f(0, 0);
            m_cells[index + 1].texCoords = sf::Vector2f(static_cast<float>(m_texture->getSize().x), 0);
            m_cells[index + 2].texCoords = sf::Vector2f(static_cast<float>(m_texture->getSize().x), static_cast<float>(m_texture->getSize().y));
            m_cells[index + 3].texCoords = sf::Vector2f(0, static_cast<float>(m_texture->getSize().y));

            m_cells[index + 0].color = sf::Color(m_color);
            m_cells[index + 1].color = sf::Color(m_color);
            m_cells[index + 2].color = sf::Color(m_color);
            m_cells[index + 3].color = sf::Color(m_color);

            m_howManyToDraw += 4;
        }
    }

    void RendererHexGridOutline::finalizeUpdate(sf::RenderTarget& renderTarget)
    {
        // TODO: Only want to update if something about the visible cells has changed
        if (didCameraChange())
        {
            m_buffer.update(m_cells.data());
        }

        renderTarget.draw(m_buffer, 0, m_howManyToDraw, m_states);
    }
} // namespace systems