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
    enum class NounType : std::uint8_t
    {
        I, // This is an implied object, it is the player, but there is no such thing as the "I" object.
           // The reason for the enum type is because it is a grammar element that needs to be represented
        Word,
        Wall,
        Floor,
        Flowers,
        Grass,
        Purple,
        Grey,
        Green,
        Blue,
        Red,
        Brown,
        Yellow,
        Black,
        SIZE // Must always be last to get the cor
    };

    static const std::unordered_map<TextType, NounType> TextTypeToNounType{
        { TextType::Text_I, NounType::I },
        { TextType::Text_Word, NounType::Word },
        { TextType::Text_Wall, NounType::Wall },
        { TextType::Text_Floor, NounType::Floor },
        { TextType::Text_Flowers, NounType::Flowers },
        { TextType::Text_Grass, NounType::Grass },
        { TextType::Text_Purple, NounType::Purple },
        { TextType::Text_Grey, NounType::Grey },
        { TextType::Text_Green, NounType::Green },
        { TextType::Text_Blue, NounType::Blue },
        { TextType::Text_Red, NounType::Red },
        { TextType::Text_Brown, NounType::Brown },
        { TextType::Text_Yellow, NounType::Yellow },
        { TextType::Text_Black, NounType::Black }
    };

    static const std::unordered_map<NounType, ObjectType> NounTypeToObjectType{
        { NounType::Word, ObjectType::Text },
        { NounType::Wall, ObjectType::Wall },
        { NounType::Floor, ObjectType::Floor },
        { NounType::Flowers, ObjectType::Flowers },
        { NounType::Grass, ObjectType::Grass },
        { NounType::Purple, ObjectType::Purple },
        { NounType::Grey, ObjectType::Grey },
        { NounType::Green, ObjectType::Green },
        { NounType::Blue, ObjectType::Blue },
        { NounType::Red, ObjectType::Red },
        { NounType::Brown, ObjectType::Brown },
        { NounType::Yellow, ObjectType::Yellow },
        { NounType::Black, ObjectType::Black }
    };

    static const std::unordered_map<ObjectType, NounType> ObjectTypeToNounType{
        { ObjectType::Text, NounType::Word },
        { ObjectType::Wall, NounType::Wall },
        { ObjectType::Floor, NounType::Floor },
        { ObjectType::Flowers, NounType::Flowers },
        { ObjectType::Grass, NounType::Grass },
        { ObjectType::Purple, NounType::Purple },
        { ObjectType::Grey, NounType::Grey },
        { ObjectType::Green, NounType::Green },
        { ObjectType::Blue, NounType::Blue },
        { ObjectType::Red, NounType::Red },
        { ObjectType::Brown, NounType::Brown },
        { ObjectType::Yellow, NounType::Yellow },
        { ObjectType::Black, NounType::Black }
    };

    static const std::unordered_map<NounType, std::string> NounTypeToString{
        { NounType::I, "i" },
        { NounType::Word, OBJECT_TYPE_STRING_TEXT },
        { NounType::Wall, OBJECT_TYPE_STRING_WALL },
        { NounType::Floor, OBJECT_TYPE_STRING_FLOOR },
        { NounType::Flowers, OBJECT_TYPE_STRING_FLOWERS },
        { NounType::Grass, OBJECT_TYPE_STRING_GRASS },
        { NounType::Purple, OBJECT_TYPE_STRING_PURPLE },
        { NounType::Grey, OBJECT_TYPE_STRING_GREY },
        { NounType::Green, OBJECT_TYPE_STRING_GREEN },
        { NounType::Blue, OBJECT_TYPE_STRING_BLUE },
        { NounType::Red, OBJECT_TYPE_STRING_RED },
        { NounType::Brown, OBJECT_TYPE_STRING_BROWN },
        { NounType::Yellow, OBJECT_TYPE_STRING_YELLOW },
        { NounType::Black, OBJECT_TYPE_STRING_BLACK }
    };

    class Noun : public PolymorphicComparable<Component, Noun>
    {
      public:
        Noun(NounType type) :
            m_type(type)
        {
        }

        NounType get() { return m_type; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Noun>(), std::make_unique<Noun>(m_type) };
        }

        bool operator==(Noun& rhs)
        {
            return m_type == rhs.m_type;
        }

      private:
        NounType m_type;
    };
} // namespace components
