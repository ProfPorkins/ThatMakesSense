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

#include "RendererHexGrid.hpp"

#include "components/Camera.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"

#include <cmath>

namespace systems
{

    RendererHexGrid::RendererHexGrid(const std::initializer_list<ctti::unnamed_type_id_t>& list, std::shared_ptr<Level> level) :
        System(list),
        m_level(level)
    {
    }

    void RendererHexGrid::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, sf::RenderTarget& renderTarget, const entities::EntityPtr& camera)
    {
        auto details = misc::computeRenderingDetails(camera, m_level->getWidth(), m_level->getHeight());
        auto compCamera = camera->getComponent<components::Camera>();

        if (details.startR != m_previousStartR || details.startQ != m_previousStartQ || details.endR != m_previousEndR || details.endQ != m_previousEndQ || details.numberR != m_previousNumberR || details.numberQ != m_previousNumberQ || m_previousCenter != compCamera->getCenter())
        {
            m_cameraChange = true;
            m_previousStartR = details.startR;
            m_previousStartQ = details.startQ;
            m_previousEndR = details.endR;
            m_previousEndQ = details.endQ;
            m_previousNumberR = details.numberR;
            m_previousNumberQ = details.numberQ;
            m_previousCenter = compCamera->getCenter();
        }
        else
        {
            m_cameraChange = false;
        }

        // Call into the derived class so it can do something for this update call
        initUpdate(elapsedTime, details.startR, details.endR, details.startQ, details.endQ, details.numberR, details.numberQ, details.coords);

        //
        // Remember: 0, 0 is at the center of the window
        // The - 0.5 * delta[XY] is to get the upper left location for the hex cell rendering
        for (auto r = details.startR; r <= details.endR; r++)
        {
            float posY = details.startY + ((r - details.startR) - details.numberR / 2) * details.hexDimY - 0.5f * details.hexDimY;

            for (auto q = details.startQ; q <= details.endQ; q++)
            {
                float posX = details.startX + ((q - details.startQ) - details.numberQ / 2) * details.hexDimX - 0.5f * details.hexDimX;
                // Every other row adds 1/2 the width of the hex in order to correctly place the hex
                if (r % 2 == 1)
                {
                    posX += details.hexDimX / 2.0f;
                }

                // Call into the derived class so it can do something for this cell
                perCell(renderTarget, { static_cast<misc::HexCoord::Type>(q), static_cast<misc::HexCoord::Type>(r) }, posX, posY, details.renderDimX, details.renderDimY);
            }
        }

        // Call into the derived class to let it do any work it wants to at the end of the update
        finalizeUpdate(renderTarget);
    }

} // namespace systems