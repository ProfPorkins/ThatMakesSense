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

#include "Level.hpp"
#include "System.hpp"
#include "misc/math.hpp"

#include <SFML/Graphics.hpp>
#include <cstdint>

namespace systems
{
    class RendererHexGrid : public System
    {
      public:
        RendererHexGrid(const std::initializer_list<ctti::unnamed_type_id_t>& list, std::shared_ptr<Level> level);

        virtual void update(std::chrono::microseconds elapsedTime, sf::RenderTarget& renderTarget, const entities::EntityPtr& camera);

      protected:
        std::shared_ptr<Level> m_level;

        virtual void initUpdate([[maybe_unused]] std::chrono::microseconds elapsedTime, [[maybe_unused]] std::uint16_t startR, [[maybe_unused]] std::uint16_t endR, [[maybe_unused]] std::uint16_t startQ, [[maybe_unused]] std::uint16_t endQ, [[maybe_unused]] std::uint16_t numberR, [[maybe_unused]] std::uint16_t numberQ, [[maybe_unused]] const math::Dimension2f& coords){};
        virtual void perCell(sf::RenderTarget& renderTarget, misc::HexCoord cell, float posX, float posY, float renderDimX, float renderDimY) = 0;
        virtual void finalizeUpdate([[maybe_unused]] sf::RenderTarget& renderTarget){};

        bool didCameraChange() { return m_cameraChange; }

      private:
        using System::update; // disables compiler warning from clang

        bool m_cameraChange{ true };
        std::uint16_t m_previousStartR{ 0 };
        std::uint16_t m_previousStartQ{ 0 };
        std::uint16_t m_previousEndR{ 0 };
        std::uint16_t m_previousEndQ{ 0 };
        std::uint16_t m_previousNumberR{ 0 };
        std::uint16_t m_previousNumberQ{ 0 };
        misc::HexCoord m_previousCenter{ 0, 0 };
    };
} // namespace systems
