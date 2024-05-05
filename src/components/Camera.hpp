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

#include "Component.hpp"
#include "misc/HexCoord.hpp"

namespace components
{
    class Camera : public PolymorphicComparable<Component, Camera>
    {
      public:
        Camera(misc::HexCoord center, std::uint8_t range) :
            m_center(center),
            m_range(range)
        {
        }

        const auto& getCenter() const { return m_center; }
        void setCenter(const misc::HexCoord& center) { m_center = center; }

        auto getRange() const { return m_range; }
        void setRange(std::uint8_t range) { m_range = range; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Camera>(), std::make_unique<Camera>(m_center, m_range) };
        }

      private:
        misc::HexCoord m_center;
        std::uint8_t m_range; // Number of visible cells in width/height
    };
} // namespace components
