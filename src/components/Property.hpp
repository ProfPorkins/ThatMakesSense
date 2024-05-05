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
#include "components/Object.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace components
{
    enum class PropertyType : std::uint16_t
    {
        // clang-format off
        None          = 0b0000000000000000,
        I             = 0b0000000000000001,
        Goal          = 0b0000000000000010,
        Steep         = 0b0000000000000100,
        Water         = 0b0000000000001000,
        Hot           = 0b0000000000010000,
        Stop          = 0b0000000000100000,
        Pushable      = 0b0000000001000000,
        Pullable      = 0b0000000010000000,
        // clang-format on

        SIZE // Must always be last to get the correct count
    };

    static const std::unordered_map<TextType, PropertyType> TextTypeToPropertyType{
        { TextType::Text_Goal, PropertyType::Goal },
        { TextType::Text_Steep, PropertyType::Steep },
        { TextType::Text_Water, PropertyType::Water },
        { TextType::Text_Hot, PropertyType::Hot },
        { TextType::Text_Stop, PropertyType::Stop },
        { TextType::Text_Push, PropertyType::Pushable },
        { TextType::Text_Pull, PropertyType::Pullable }
    };

    // This assumes only one type is in the property
    static const std::unordered_map<PropertyType, std::string> PropertyTypeToString{
        { PropertyType::None, "none" },
        { PropertyType::I, "i" },
        { PropertyType::Goal, "goal" },
        { PropertyType::Steep, "steep" },
        { PropertyType::Water, "water" },
        { PropertyType::Hot, "hot" },
        { PropertyType::Stop, "stop" },
        { PropertyType::Pushable, "pushable" },
        { PropertyType::Pullable, "pullable" }
    };

    class Property : public PolymorphicComparable<Component, Property>
    {
      public:
        Property(PropertyType type) :
            m_type(type)
        {
        }

        //
        // Pain in the rear, have to static_cast to the underlying type first to do the bitwise operators
        // and in two cases, cast back to the enum class type afterwards...yuck!
        auto get() { return m_type; }
        bool has(PropertyType type) { return static_cast<std::uint16_t>(m_type) & static_cast<std::uint16_t>(type); }
        void add(PropertyType type) { m_type = static_cast<PropertyType>(static_cast<std::uint16_t>(m_type) | static_cast<std::uint16_t>(type)); }
        void remove(PropertyType type) { m_type = static_cast<PropertyType>(static_cast<std::uint16_t>(m_type) & ~static_cast<std::uint16_t>(type)); }
        void reset() { m_type = PropertyType::None; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Property>(), std::make_unique<Property>(m_type) };
        }

        bool operator==(Property& rhs)
        {
            return m_type == rhs.m_type;
        }

      private:
        PropertyType m_type;
    };
} // namespace components
