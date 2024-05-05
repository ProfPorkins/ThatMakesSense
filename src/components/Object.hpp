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
#include "misc/misc.hpp"

#include <cstdint>
#include <unordered_map>

namespace components
{
    using namespace std::string_literals;

    // NOTE: The order of the values is also the rendering order
    enum class ObjectType : std::uint8_t
    {
        Background_White = 0, // These are ONLY intended as background tiles, never, ever, to be used as objects
        Background_Purple = 1,
        Background_Grey = 2,
        Background_Green = 3,
        Background_Blue = 4,
        Background_Red = 5,
        Background_Brown = 6,
        Background_Yellow = 7,

        Floor = 8,
        Wall = 9,
        Purple = 10,
        Grey = 11,
        Green = 12,
        Blue = 13,
        Red = 14,
        Brown = 15,
        Yellow = 16,
        Black = 17,

        I_Offset = Black - Background_Yellow,
        I_Wall = 18,   // the I_ aren't really objects, they are space-holders for the renderer so it
        I_Floor = 19,  // will render "I" objects on top of the other object.  I know this is pretty terrible
        I_Purple = 20, // for this enum to have that knowledge embedded, but it was an "easy" way to solve
        I_Grey = 21,   // the problem.
        I_Green = 22,
        I_Blue = 23,
        I_Red = 24,
        I_Brown = 25,
        I_Yellow = 26,
        I_Black = 27,

        Grass = 28,
        Flowers = 29,

        Text = 30, // This MUST be the last item before SIZE.  The reason is that all text renders on top of everything else
        SIZE = 31  // Not one of the enum items, but a count of how many there are, and it MUST be the last item
    };

    enum class TextType : std::uint8_t
    {
        Before_Verb,
        Text_Is,
        Text_Am,
        Text_Can,
        Text_And,
        After_Verb,

        // NOTE: If you add something here, go into Parser.cpp and update ::isProperty and ::isAbility as needed
        Before_PropAbility,
        Text_Goal,
        Text_Climb, // Ability
        Text_Float, // Ability
        Text_Chill, // Ability
        Text_Push,  // Property/Ability
        Text_Pull,  // Property/Ability
        Text_Stop,  // Property
        Text_Steep, // Property
        Text_Water, // Property
        Text_Hot,   // Property
        Text_Send,  // Property
        After_PropAbility,

        Before_Noun,
        Text_I,
        Text_Word,
        Text_Wall,
        Text_Floor,
        Text_Flowers,
        Text_Grass,
        Text_Purple,
        Text_Grey,
        Text_Green,
        Text_Blue,
        Text_Red,
        Text_Brown,
        Text_Yellow,
        Text_Black,
        After_Noun,
        None,
        SIZE
    };

    static const auto OBJECT_TYPE_STRING_WALL{ "wall"s };
    static const auto OBJECT_TYPE_STRING_FLOOR{ "floor"s };
    static const auto OBJECT_TYPE_STRING_FLOWERS{ "flowers"s };
    static const auto OBJECT_TYPE_STRING_GRASS{ "grass"s };
    static const auto OBJECT_TYPE_STRING_PURPLE{ "purple" };
    static const auto OBJECT_TYPE_STRING_GREY{ "grey" };
    static const auto OBJECT_TYPE_STRING_GREEN{ "green" };
    static const auto OBJECT_TYPE_STRING_BLUE{ "blue" };
    static const auto OBJECT_TYPE_STRING_RED{ "red" };
    static const auto OBJECT_TYPE_STRING_BROWN{ "brown" };
    static const auto OBJECT_TYPE_STRING_YELLOW{ "yellow" };
    static const auto OBJECT_TYPE_STRING_BLACK{ "black" };
    static const auto OBJECT_TYPE_STRING_TEXT{ "word" };

    static const std::unordered_map<ObjectType, std::string> ObjectTypeToString{
        { ObjectType::Wall, OBJECT_TYPE_STRING_WALL },
        { ObjectType::Floor, OBJECT_TYPE_STRING_FLOOR },
        { ObjectType::Flowers, OBJECT_TYPE_STRING_FLOWERS },
        { ObjectType::Grass, OBJECT_TYPE_STRING_GRASS },
        { ObjectType::Purple, OBJECT_TYPE_STRING_PURPLE },
        { ObjectType::Grey, OBJECT_TYPE_STRING_GREY },
        { ObjectType::Green, OBJECT_TYPE_STRING_GREEN },
        { ObjectType::Blue, OBJECT_TYPE_STRING_BLUE },
        { ObjectType::Red, OBJECT_TYPE_STRING_RED },
        { ObjectType::Brown, OBJECT_TYPE_STRING_BROWN },
        { ObjectType::Yellow, OBJECT_TYPE_STRING_YELLOW },
        { ObjectType::Black, OBJECT_TYPE_STRING_BLACK },
        { ObjectType::Text, OBJECT_TYPE_STRING_TEXT }
    };

    static const std::unordered_map<std::string, ObjectType> StringToObjectType{
        { OBJECT_TYPE_STRING_WALL, ObjectType::Wall },
        { OBJECT_TYPE_STRING_FLOOR, ObjectType::Floor },
        { OBJECT_TYPE_STRING_FLOWERS, ObjectType::Flowers },
        { OBJECT_TYPE_STRING_GRASS, ObjectType::Grass },
        { OBJECT_TYPE_STRING_PURPLE, ObjectType::Purple },
        { OBJECT_TYPE_STRING_GREY, ObjectType::Grey },
        { OBJECT_TYPE_STRING_GREEN, ObjectType::Green },
        { OBJECT_TYPE_STRING_BLUE, ObjectType::Blue },
        { OBJECT_TYPE_STRING_RED, ObjectType::Red },
        { OBJECT_TYPE_STRING_BROWN, ObjectType::Brown },
        { OBJECT_TYPE_STRING_YELLOW, ObjectType::Yellow },
        { OBJECT_TYPE_STRING_BLACK, ObjectType::Black },
        { OBJECT_TYPE_STRING_TEXT, ObjectType::Text }
    };

    static const std::unordered_map<TextType, std::string> TextTypeToString{
        { TextType::Text_Is, "is" },
        { TextType::Text_Am, "am" },
        { TextType::Text_Can, "can" },
        { TextType::Text_And, "and" },

        { TextType::Text_Goal, "goal" },
        { TextType::Text_Climb, "climb" },
        { TextType::Text_Float, "float" },
        { TextType::Text_Hot, "hot" },
        { TextType::Text_Stop, "stop" },
        { TextType::Text_Push, "push" },
        { TextType::Text_Pull, "pull" },
        { TextType::Text_I, "i" },

        { TextType::Text_Word, OBJECT_TYPE_STRING_TEXT },
        { TextType::Text_Wall, OBJECT_TYPE_STRING_WALL },
        { TextType::Text_Floor, OBJECT_TYPE_STRING_FLOOR },
        { TextType::Text_Flowers, OBJECT_TYPE_STRING_FLOWERS },
        { TextType::Text_Grass, OBJECT_TYPE_STRING_GRASS },
        { TextType::Text_Purple, OBJECT_TYPE_STRING_PURPLE },
        { TextType::Text_Grey, OBJECT_TYPE_STRING_GREY },
        { TextType::Text_Green, OBJECT_TYPE_STRING_GREEN },
        { TextType::Text_Blue, OBJECT_TYPE_STRING_BLUE },
        { TextType::Text_Red, OBJECT_TYPE_STRING_RED },
        { TextType::Text_Brown, OBJECT_TYPE_STRING_BROWN },
        { TextType::Text_Yellow, OBJECT_TYPE_STRING_YELLOW },
        { TextType::Text_Black, OBJECT_TYPE_STRING_BLACK }
    };

    class Object : public PolymorphicComparable<Component, Object>
    {
      public:
        static constexpr std::size_t TYPE_SIZE = static_cast<std::size_t>(misc::as_integer(ObjectType::SIZE)) + static_cast<std::size_t>(misc::as_integer(TextType::SIZE));

        Object(ObjectType object) :
            m_type(object),
            m_typeText(TextType::None)
        {
        }
        Object(ObjectType object, TextType text) :
            m_type(object),
            m_typeText(text)
        {
        }

        ObjectType getType() { return m_type; }
        TextType getText() { return m_typeText; }
        std::size_t renderSequence()
        {
            std::size_t bufferIndex = static_cast<std::size_t>(m_type);
            if (m_type == components::ObjectType::Text)
            {
                bufferIndex += static_cast<std::size_t>(m_typeText);
            }

            return bufferIndex;
        }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Object>(), std::make_unique<Object>(m_type, m_typeText) };
        }

        bool operator==(Object& rhs)
        {
            return m_type == rhs.m_type && m_typeText == rhs.m_typeText;
        }

        static bool isNoun(TextType textType)
        {
            return textType >= components::TextType::Before_Noun && textType <= components::TextType::After_Noun;
        }
        static bool isVerb(TextType textType)
        {
            return textType >= components::TextType::Before_Verb && textType <= components::TextType::After_Verb;
        }
        static bool isPropAbility(TextType textType)
        {
            return textType >= components::TextType::Before_PropAbility && textType <= components::TextType::After_PropAbility;
        };

      private:
        ObjectType m_type;
        TextType m_typeText;
    };
} // namespace components
