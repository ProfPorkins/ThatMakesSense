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
    enum class AbilityType : std::uint16_t
    {
        // clang-format off
        None          = 0b0000000000000000,
        Climb         = 0b0000000000000001,
        Float          = 0b0000000000000010,
        Push          = 0b0000000000001000,
        Pull          = 0b0000000000010000,
        Chill         = 0b0000000000100000,
        Send          = 0b0000000001000000,

        // clang-format on
        SIZE // Must always be last to get the correct count
    };

    static const std::unordered_map<TextType, AbilityType> TextTypeToAbilityType{
        { TextType::Text_Climb, AbilityType::Climb },
        { TextType::Text_Float, AbilityType::Float },
        { TextType::Text_Push, AbilityType::Push },
        { TextType::Text_Pull, AbilityType::Pull },
        { TextType::Text_Chill, AbilityType::Chill },
        { TextType::Text_Send, AbilityType::Send }
    };

    // This assumes only one type is in the ability
    static const std::unordered_map<AbilityType, std::string> AbilityTypeToString{
        { AbilityType::None, "none" },
        { AbilityType::Climb, "climb" },
        { AbilityType::Float, "float" },
        { AbilityType::Push, "push" },
        { AbilityType::Pull, "pull" },
        { AbilityType::Chill, "chill" },
        { AbilityType::Send, "send" }
    };

    class Ability : public PolymorphicComparable<Component, Ability>
    {
      public:
        Ability(AbilityType type) :
            m_type(type)
        {
        }

        //
        // Pain in the rear, have to static_cast to the underlying type first to do the bitwise operators
        // and in two cases, cast back to the enum class type afterwards...yuck!
        auto get() { return m_type; }
        bool has(AbilityType type) { return static_cast<std::uint16_t>(m_type) & static_cast<std::uint16_t>(type); }
        void add(AbilityType type) { m_type = static_cast<AbilityType>(static_cast<std::uint16_t>(m_type) | static_cast<std::uint16_t>(type)); }
        void remove(AbilityType type) { m_type = static_cast<AbilityType>(static_cast<std::uint16_t>(m_type) & ~static_cast<std::uint16_t>(type)); }
        void reset() { m_type = AbilityType::None; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Ability>(), std::make_unique<Ability>(m_type) };
        }

        bool operator==(Ability& rhs)
        {
            return m_type == rhs.m_type;
        }

      private:
        AbilityType m_type;
    };
} // namespace components
