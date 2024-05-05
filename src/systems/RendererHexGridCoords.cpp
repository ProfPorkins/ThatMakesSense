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

#include "RendererHexGridCoords.hpp"

#include "components/Camera.hpp"
#include "misc/math.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <format>

namespace systems
{
    RendererHexGridCoords::RendererHexGridCoords(std::shared_ptr<Level> level) :
        RendererHexGrid({ /* Doesn't specify any interest because it only cares about coordinates */ }, level)
    {
        ui::Text::Settings settings{ false, 0, 0, "", Content::get<sf::Font>(content::KEY_FONT_DEVELOPER_HEX_COORDS), sf::Color::Black, std::nullopt, Configuration::get<std::uint8_t>(config::DEVELOPER_HEX_COORDS_FONT_SIZE) };
        m_textCoords = std::make_unique<ui::Text>(settings);
    }

    void RendererHexGridCoords::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, sf::RenderTarget& renderTarget, const entities::EntityPtr& camera)
    {
        if (Configuration::get<bool>(config::DEVELOPER_HEX_COORDS_RENDER))
        {
            RendererHexGrid::update(elapsedTime, renderTarget, camera);
        }
    }

    void RendererHexGridCoords::perCell(sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY)
    {
        m_textCoords->setText(std::format("{0},{1}", cell.q, cell.r));
        // Center in the cell
        m_textCoords->setPosition({ posX + renderDimX / 2 - m_textCoords->getRegion().width / 2.0f, posY + renderDimY / 2 - m_textCoords->getRegion().height / 2.0f });
        m_textCoords->render(renderTarget);
    }

} // namespace systems