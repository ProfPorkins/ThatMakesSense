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

#include "Entity.hpp"
#include "components/Ability.hpp"
#include "components/AnimatedSprite.hpp"
#include "components/Noun.hpp"
#include "components/Object.hpp"
#include "components/PhraseDirection.hpp"
#include "components/Property.hpp"
#include "components/Verb.hpp"
#include "misc/HexCoord.hpp"

#include <memory>
#include <unordered_map>

namespace entities
{
    enum class EntityCode : std::uint8_t
    {
        // Game objects
        Object_Purple = 3,
        Object_Grey = 4,
        Object_Green = 5,
        Object_Blue = 6,
        Object_Red = 7,
        Object_Brown = 8,
        Object_Yellow = 9,
        Object_Black = 12,

        // Background/environmental tiles only
        Background_Wall = 1,
        Background_Floor = 2,
        Background_Grass = 10,
        Background_Flowers = 11,
        Background_White = 13,
        Background_Purple = 14,
        Background_Grey = 15,
        Background_Green = 16,
        Background_Blue = 17,
        Background_Red = 18,
        Background_Brown = 19,
        Background_Yellow = 20,

        // Object text names
        Text_I = 40,
        Text_Word = 41,
        Text_Wall = 42,
        Text_Floor = 43,
        Text_Purple = 44,
        Text_Grey = 45,
        Text_Green = 46,
        Text_Blue = 47,
        Text_Red = 48,
        Text_Brown = 49,
        Text_Yellow = 50,
        Text_Grass = 51,
        Text_Flowers = 52,
        Text_Black = 53,
        Text_White = 54,

        // Verb text names
        Text_Is = 60,
        Text_Am = 61,
        Text_Can = 62,
        Text_And = 63,

        // Abilities/Properties text names
        Text_Goal = 76,
        Text_Climb = 70,
        Text_Float = 71,
        Text_Hot = 72,
        Text_Send = 80,
        Text_Stop = 73,
        Text_Push = 74,
        Text_Pull = 75,
        Text_Water = 77,
        Text_Chill = 78,
        Text_Steep = 79
    };

    static const std::unordered_map<EntityCode, components::ObjectType> EntityCodeToObjectType{
        { EntityCode::Background_Wall, components::ObjectType::Wall },
        { EntityCode::Background_Floor, components::ObjectType::Floor },
        { EntityCode::Background_Grass, components::ObjectType::Grass },
        { EntityCode::Background_Flowers, components::ObjectType::Flowers },
        { EntityCode::Object_Purple, components::ObjectType::Purple },
        { EntityCode::Object_Grey, components::ObjectType::Grey },
        { EntityCode::Object_Green, components::ObjectType::Green },
        { EntityCode::Object_Blue, components::ObjectType::Blue },
        { EntityCode::Object_Red, components::ObjectType::Red },
        { EntityCode::Object_Brown, components::ObjectType::Brown },
        { EntityCode::Object_Yellow, components::ObjectType::Yellow },
        { EntityCode::Object_Black, components::ObjectType::Black },
        { EntityCode::Text_Word, components::ObjectType::Text }
    };

    static const std::unordered_map<components::ObjectType, EntityCode> ObjectTypeToEntityCode{
        { components::ObjectType::Wall, EntityCode::Background_Wall },
        { components::ObjectType::Floor, EntityCode::Background_Floor },
        { components::ObjectType::Grass, EntityCode::Background_Grass },
        { components::ObjectType::Flowers, EntityCode::Background_Flowers },
        { components::ObjectType::Purple, EntityCode::Object_Purple },
        { components::ObjectType::Grey, EntityCode::Object_Grey },
        { components::ObjectType::Green, EntityCode::Object_Green },
        { components::ObjectType::Blue, EntityCode::Object_Blue },
        { components::ObjectType::Red, EntityCode::Object_Red },
        { components::ObjectType::Brown, EntityCode::Object_Brown },
        { components::ObjectType::Yellow, EntityCode::Object_Yellow },
        { components::ObjectType::Black, EntityCode::Object_Black },
        { components::ObjectType::Text, EntityCode::Text_Word }
    };

    std::shared_ptr<Entity> createEntity(misc::HexCoord position, std::function<void(entities::EntityPtr)> apply);
    std::shared_ptr<Entity> createCamera(misc::HexCoord center, std::uint8_t range);

    std::unique_ptr<components::AnimatedSprite> createAnimatedSprite(std::string keyLevel, std::string keyDOM, std::string keyContent);
    std::shared_ptr<Entity> createObject(misc::HexCoord position, components::ObjectType type, std::string word, std::string keyContent);
    entities::EntityPtr createText(misc::HexCoord position, std::string word, components::TextType typeText, components::NounType typeNoun, std::string keyContent);
    entities::EntityPtr createText(misc::HexCoord position, std::string word, components::TextType typeText, components::VerbType typeVerb, std::string keyContent);
    entities::EntityPtr createPhraseDirection(std::uint16_t id, misc::HexCoord position, misc::HexCoord::Direction direction, components::PhraseDirection::PhraseElement element);
    void transformNoun(entities::EntityPtr entity, components::NounType type);

    using EntityFactoryCommandMap = std::unordered_map<entities::EntityCode, std::function<entities::EntityPtr(misc::HexCoord position)>>;
    EntityFactoryCommandMap buildCodeToEntityCommandMap();
} // namespace entities
