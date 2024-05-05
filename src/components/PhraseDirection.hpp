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
    class PhraseDirection : public PolymorphicComparable<Component, PhraseDirection>
    {
      public:
        enum class PhraseElement
        {
            Start,
            Middle,
            End
        };

        static constexpr std::uint8_t GridDirection = 0;
        static constexpr std::uint8_t GridElement = 1;
        using DirectionGrid = std::vector<std::vector<std::unordered_map<std::uint16_t, std::tuple<misc::HexCoord::Direction, components::PhraseDirection::PhraseElement>>>>;

        PhraseDirection(std::uint16_t id, misc::HexCoord position, misc::HexCoord::Direction direction, PhraseElement element) :
            m_id(id),
            m_position(position),
            m_direction(direction),
            m_element(element)
        {
        }

        std::uint16_t getId() { return m_id; }
        misc::HexCoord::Direction getDirection() { return m_direction; }
        misc::HexCoord getPosition() { return m_position; }
        PhraseElement getElement() { return m_element; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<PhraseDirection>(), std::make_unique<PhraseDirection>(m_id, m_position, m_direction, m_element) };
        }

        bool operator==(PhraseDirection& rhs)
        {
            return m_id == rhs.m_id &&
                   m_position == rhs.m_position &&
                   m_direction == rhs.m_direction &&
                   m_element == rhs.m_element;
        }

      private:
        std::uint16_t m_id;
        misc::HexCoord m_position;
        misc::HexCoord::Direction m_direction;
        PhraseElement m_element;
    };
} // namespace components
