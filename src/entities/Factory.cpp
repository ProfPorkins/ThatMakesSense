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

#include "Factory.hpp"

#include "components/AnimatedSprite.hpp"
#include "components/Audio.hpp"
#include "components/Camera.hpp"
#include "components/Component.hpp"
#include "components/InputControlled.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"
#include "components/StaticSprite.hpp"
#include "misc/math.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <SFML/Graphics.hpp>
#include <cassert>
#include <string>

namespace entities
{
    std::shared_ptr<Entity> createEntity(misc::HexCoord position, std::function<void(entities::EntityPtr)> apply)
    {
        auto entity = std::make_shared<entities::Entity>();

        entity->addComponent(std::make_unique<components::Position>(position));
        apply(entity);

        return entity;
    };

    std::shared_ptr<Entity> createCamera(misc::HexCoord center, std::uint8_t range)
    {
        auto entity = std::make_shared<entities::Entity>();

        entity->addComponent(std::make_unique<components::Camera>(center, range));

        return entity;
    }

    std::unique_ptr<components::AnimatedSprite> createAnimatedSprite(std::string keyLevel, std::string keyDOM, std::string keyContent)
    {
        using namespace config;

        const config_path TEXTURE = { DOM_CONTENT, DOM_LEVELS, keyLevel, keyDOM, DOM_FILENAME_ANIMATED };
        const config_path SPRITE_COUNT = { DOM_CONTENT, DOM_LEVELS, keyLevel, keyDOM, DOM_SPRITE_COUNT };
        const config_path SPRITE_TIME = { DOM_CONTENT, DOM_LEVELS, keyLevel, keyDOM, DOM_SPRITE_TIME };
        const config_path SPRITE_COLOR = { DOM_CONTENT, DOM_LEVELS, keyLevel, keyDOM, DOM_SPRITE_COLOR };

        auto spriteCount = Configuration::get<std::uint8_t>(SPRITE_COUNT);
        auto spriteTime = misc::msTous(Configuration::get<std::chrono::milliseconds>(SPRITE_TIME));
        auto texture = Content::get<sf::Texture>(keyContent);
        auto colors = misc::split(Configuration::get<std::string>(SPRITE_COLOR), ',');
        sf::Color spriteColor(static_cast<uint8_t>(std::stoi(colors[0])), static_cast<uint8_t>(std::stoi(colors[1])), static_cast<uint8_t>(std::stoi(colors[2])));

        auto sprite = std::make_unique<components::AnimatedSprite>(texture, spriteCount, spriteTime, spriteColor);
        sprite->getSprite()->setScale(math::getViewScale({ static_cast<float>(sprite->getSpriteCount()), 1.0f }, sprite->getSprite()->getTexture()));

        return sprite;
    }

    std::unique_ptr<components::StaticSprite> createStaticSprite(std::string keyLevel, std::string keyDOM, std::string keyContent)
    {
        using namespace config;

        const config_path TEXTURE = { DOM_CONTENT, DOM_LEVELS, keyLevel, keyDOM, DOM_FILENAME_STATIC };
        const config_path SPRITE_COLOR = { DOM_CONTENT, DOM_LEVELS, keyLevel, keyDOM, DOM_SPRITE_COLOR };

        auto texture = Content::get<sf::Texture>(keyContent);
        auto colors = misc::split(Configuration::get<std::string>(SPRITE_COLOR), ',');
        sf::Color spriteColor(static_cast<uint8_t>(std::stoi(colors[0])), static_cast<uint8_t>(std::stoi(colors[1])), static_cast<uint8_t>(std::stoi(colors[2])));

        auto sprite = std::make_unique<components::StaticSprite>(texture, spriteColor);
        sprite->getSprite()->setScale(math::getViewScale({ 1.0f, 1.0f }, sprite->getSprite()->getTexture()));

        return sprite;
    }

    std::shared_ptr<Entity> createObject(misc::HexCoord position, components::ObjectType type, std::string word, std::string keyContent)
    {
        auto entity = std::make_shared<entities::Entity>();

        entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, word, keyContent));
        entity->addComponent(std::make_unique<components::Position>(position));
        entity->addComponent(std::make_unique<components::Object>(type));
        entity->addComponent(std::make_unique<components::Property>(components::PropertyType::None));
        entity->addComponent(std::make_unique<components::Ability>(components::AbilityType::None));

        return entity;
    }

    // --------------------------------------------------------------
    //
    // This is called "create" because text types can never be changed,
    // only created.
    //
    // --------------------------------------------------------------
    entities::EntityPtr createText(misc::HexCoord position, std::string word, components::TextType typeText, std::string keyContent)
    {
        auto entity = std::make_shared<entities::Entity>();

        entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, word, keyContent));
        entity->addComponent(std::make_unique<components::Position>(position));
        entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Text, typeText));
        entity->addComponent(std::make_unique<components::Property>(components::PropertyType::None));
        entity->addComponent(std::make_unique<components::Ability>(components::AbilityType::None));

        return entity;
    }

    // --------------------------------------------------------------
    //
    // This is called "create" because text types can never be changed,
    // only created.
    //
    // --------------------------------------------------------------
    entities::EntityPtr createText(misc::HexCoord position, std::string word, components::TextType typeText, components::NounType typeNoun, std::string keyContent)
    {
        auto entity = createText(position, word, typeText, keyContent);

        entity->addComponent(std::make_unique<components::Noun>(typeNoun));

        return entity;
    }

    // --------------------------------------------------------------
    //
    // This is called "create" because text types can never be changed,
    // only created.
    //
    // --------------------------------------------------------------
    entities::EntityPtr createText(misc::HexCoord position, std::string word, components::TextType typeText, components::VerbType typeVerb, std::string keyContent)
    {
        auto entity = createText(position, word, typeText, keyContent);

        entity->addComponent(std::make_unique<components::Verb>(typeVerb));

        return entity;
    }

    // --------------------------------------------------------------
    //
    // This is called "create" because text types can never be changed,
    // only created.
    //
    // --------------------------------------------------------------
    entities::EntityPtr createText(misc::HexCoord position, std::string word, components::TextType typeText, components::AbilityType typeAbility, std::string keyContent)
    {
        auto entity = createText(position, word, typeText, keyContent);

        entity->addComponent(std::make_unique<components::Ability>(typeAbility));

        return entity;
    }

    entities::EntityPtr createPhraseDirection(std::uint16_t id, misc::HexCoord position, misc::HexCoord::Direction direction, components::PhraseDirection::PhraseElement element)
    {
        auto entity = std::make_shared<entities::Entity>();

        entity->addComponent(std::make_unique<components::PhraseDirection>(id, position, direction, element));

        return entity;
    }

    // --------------------------------------------------------------
    //
    // Used to transform an existing (noun) entity into another noun.
    //
    // --------------------------------------------------------------
    void transformNoun(entities::EntityPtr entity, components::NounType type)
    {
        using namespace std::string_literals;

        // tear down the existing noun components and attributes
        entity->removeComponent<components::AnimatedSprite>();
        entity->removeComponent<components::StaticSprite>();
        entity->removeComponent<components::Object>();
        entity->removeComponent<components::Noun>();
        entity->removeComponent<components::InputControlled>();

        entity->getComponent<components::Property>()->reset();
        entity->getComponent<components::Ability>()->reset();

        switch (type)
        {
            case components::NounType::Word:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "text-word"s, content::KEY_TEXT_ANIMATED_WORD));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Text, components::TextType::Text_Word));
                entity->addComponent(std::make_unique<components::Noun>(components::NounType::Word));
            }
            break;
            case components::NounType::Wall:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "wall"s, content::KEY_IMAGE_ANIMATED_ENTITY_WALL));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Wall));
            }
            break;
            case components::NounType::Floor: // In theory, shouldn't happen
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "floor"s, content::KEY_IMAGE_ANIMATED_ENTITY_FLOOR));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Floor));
            }
            break;
            case components::NounType::Flowers: // In theory, shouldn't happen
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "flowers"s, content::KEY_IMAGE_ANIMATED_ENTITY_FLOWERS));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Flowers));
            }
            break;
            case components::NounType::Grass: // In theory, shouldn't happen
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "grass"s, content::KEY_IMAGE_ANIMATED_ENTITY_GRASS));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Grass));
            }
            break;
            case components::NounType::Purple:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "purple"s, content::KEY_IMAGE_ANIMATED_ENTITY_PURPLE));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Purple));
            }
            break;
            case components::NounType::Grey:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "grey"s, content::KEY_IMAGE_ANIMATED_ENTITY_GREY));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Grey));
            }
            break;
            case components::NounType::Green:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "green"s, content::KEY_IMAGE_ANIMATED_ENTITY_GREEN));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Green));
            }
            break;
            case components::NounType::Blue:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "blue"s, content::KEY_IMAGE_ANIMATED_ENTITY_BLUE));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Blue));
            }
            break;
            case components::NounType::Red:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "red"s, content::KEY_IMAGE_ANIMATED_ENTITY_RED));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Red));
            }
            break;
            case components::NounType::Brown:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "brown"s, content::KEY_IMAGE_ANIMATED_ENTITY_BROWN));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Brown));
            }
            break;
            case components::NounType::Yellow:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "yellow"s, content::KEY_IMAGE_ANIMATED_ENTITY_YELLOW));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Yellow));
            }
            break;
            case components::NounType::Black:
            {
                entity->addComponent(createAnimatedSprite(config::DOM_IMAGES_OBJECTS, "black"s, content::KEY_IMAGE_ANIMATED_ENTITY_BLACK));
                entity->addComponent(std::make_unique<components::Object>(components::ObjectType::Black));
            }
            break;

            case components::NounType::I: // Should never happen
                assert(false);
                break;
            case components::NounType::SIZE: // Should never happen
                assert(false);
                break;
        }
    }

    // --------------------------------------------------------------
    //
    // Build a command map that can used to create the appropriate entity
    // based on the code read from the file.
    //
    // --------------------------------------------------------------
    EntityFactoryCommandMap buildCodeToEntityCommandMap()
    {
        using namespace std::string_literals;
        std::unordered_map<entities::EntityCode, std::function<entities::EntityPtr(misc::HexCoord position)>> map;
        //
        // Object types
        map[entities::EntityCode::Background_Wall] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Wall, "wall"s, content::KEY_IMAGE_ANIMATED_ENTITY_WALL);
        };
        map[entities::EntityCode::Background_Floor] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Floor, "floor"s, content::KEY_IMAGE_ANIMATED_ENTITY_FLOOR);
        };
        map[entities::EntityCode::Background_Grass] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Grass, "grass"s, content::KEY_IMAGE_ANIMATED_ENTITY_GRASS);
        };
        map[entities::EntityCode::Background_Flowers] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Flowers, "flowers"s, content::KEY_IMAGE_ANIMATED_ENTITY_FLOWERS);
        };
        map[entities::EntityCode::Object_Purple] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Purple, "purple"s, content::KEY_IMAGE_ANIMATED_ENTITY_PURPLE);
        };
        map[entities::EntityCode::Background_Purple] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_Purple, "background_purple", content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_PURPLE);
        };
        map[entities::EntityCode::Object_Grey] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Grey, "grey"s, content::KEY_IMAGE_ANIMATED_ENTITY_GREY);
        };
        map[entities::EntityCode::Background_Grey] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_Grey, "background_grey", content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_GREY);
        };
        map[entities::EntityCode::Object_Green] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Green, "green"s, content::KEY_IMAGE_ANIMATED_ENTITY_GREEN);
        };
        map[entities::EntityCode::Background_Green] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_Green, "background_green", content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_GREEN);
        };
        map[entities::EntityCode::Object_Blue] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Blue, "blue"s, content::KEY_IMAGE_ANIMATED_ENTITY_BLUE);
        };
        map[entities::EntityCode::Background_Blue] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_Blue, "background_blue", content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_BLUE);
        };
        map[entities::EntityCode::Object_Red] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Red, "red"s, content::KEY_IMAGE_ANIMATED_ENTITY_RED);
        };
        map[entities::EntityCode::Background_Red] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_Red, "background_red", content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_RED);
        };
        map[entities::EntityCode::Object_Brown] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Brown, "brown"s, content::KEY_IMAGE_ANIMATED_ENTITY_BROWN);
        };
        map[entities::EntityCode::Background_Brown] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_Brown, "background_brown", content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_BROWN);
        };
        map[entities::EntityCode::Object_Yellow] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Yellow, "yellow"s, content::KEY_IMAGE_ANIMATED_ENTITY_YELLOW);
        };
        map[entities::EntityCode::Background_Yellow] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_Yellow, "background_yellow", content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_YELLOW);
        };
        map[entities::EntityCode::Background_White] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Background_White, "background_white"s, content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_WHITE);
        };
        map[entities::EntityCode::Object_Black] = [](misc::HexCoord position)
        {
            return entities::createObject(position, components::ObjectType::Black, "black"s, content::KEY_IMAGE_ANIMATED_ENTITY_BLACK);
        };

        // Text types
        map[entities::EntityCode::Text_I] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-i", components::TextType::Text_I, components::NounType::I, content::KEY_TEXT_ANIMATED_I);
        };
        map[entities::EntityCode::Text_Word] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-word", components::TextType::Text_Word, components::NounType::Word, content::KEY_TEXT_ANIMATED_WORD);
        };
        map[entities::EntityCode::Text_Wall] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-wall", components::TextType::Text_Wall, components::NounType::Wall, content::KEY_TEXT_ANIMATED_WALL);
        };
        map[entities::EntityCode::Text_Floor] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-floor", components::TextType::Text_Floor, components::NounType::Floor, content::KEY_TEXT_ANIMATED_FLOOR);
        };
        map[entities::EntityCode::Text_Flowers] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-flowers", components::TextType::Text_Flowers, components::NounType::Flowers, content::KEY_TEXT_ANIMATED_FLOWERS);
        };
        map[entities::EntityCode::Text_Grass] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-grass", components::TextType::Text_Grass, components::NounType::Grass, content::KEY_TEXT_ANIMATED_GRASS);
        };
        map[entities::EntityCode::Text_Purple] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-purple", components::TextType::Text_Purple, components::NounType::Purple, content::KEY_TEXT_ANIMATED_PURPLE);
        };
        map[entities::EntityCode::Text_Grey] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-grey", components::TextType::Text_Grey, components::NounType::Grey, content::KEY_TEXT_ANIMATED_GREY);
        };
        map[entities::EntityCode::Text_Green] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-green", components::TextType::Text_Green, components::NounType::Green, content::KEY_TEXT_ANIMATED_GREEN);
        };
        map[entities::EntityCode::Text_Blue] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-blue", components::TextType::Text_Blue, components::NounType::Blue, content::KEY_TEXT_ANIMATED_BLUE);
        };
        map[entities::EntityCode::Text_Red] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-red", components::TextType::Text_Red, components::NounType::Red, content::KEY_TEXT_ANIMATED_RED);
        };
        map[entities::EntityCode::Text_Brown] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-brown", components::TextType::Text_Brown, components::NounType::Brown, content::KEY_TEXT_ANIMATED_BROWN);
        };
        map[entities::EntityCode::Text_Yellow] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-yellow", components::TextType::Text_Yellow, components::NounType::Yellow, content::KEY_TEXT_ANIMATED_YELLOW);
        };
        map[entities::EntityCode::Text_Black] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-black", components::TextType::Text_Black, components::NounType::Black, content::KEY_TEXT_ANIMATED_BLACK);
        };

        map[entities::EntityCode::Text_Is] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-is", components::TextType::Text_Is, components::VerbType::Is, content::KEY_TEXT_ANIMATED_IS);
        };
        map[entities::EntityCode::Text_Am] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-am", components::TextType::Text_Am, components::VerbType::Am, content::KEY_TEXT_ANIMATED_AM);
        };
        map[entities::EntityCode::Text_Can] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-can", components::TextType::Text_Can, components::VerbType::Can, content::KEY_TEXT_ANIMATED_CAN);
        };
        map[entities::EntityCode::Text_And] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-and", components::TextType::Text_And, components::VerbType::And, content::KEY_TEXT_ANIMATED_AND);
        };

        map[entities::EntityCode::Text_Goal] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-goal", components::TextType::Text_Goal, components::AbilityType::None, content::KEY_TEXT_ANIMATED_GOAL);
        };
        map[entities::EntityCode::Text_Climb] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-climb", components::TextType::Text_Climb, components::AbilityType::Climb, content::KEY_TEXT_ANIMATED_CLIMB);
        };
        map[entities::EntityCode::Text_Float] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-float", components::TextType::Text_Float, components::AbilityType::Float, content::KEY_TEXT_ANIMATED_FLOAT);
        };
        map[entities::EntityCode::Text_Hot] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-hot", components::TextType::Text_Hot, components::AbilityType::None, content::KEY_TEXT_ANIMATED_HOT);
        };
        map[entities::EntityCode::Text_Send] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-send", components::TextType::Text_Send, components::AbilityType::None, content::KEY_TEXT_ANIMATED_SEND);
        };
        map[entities::EntityCode::Text_Stop] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-stop", components::TextType::Text_Stop, components::AbilityType::None, content::KEY_TEXT_ANIMATED_STOP);
        };
        map[entities::EntityCode::Text_Push] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-push", components::TextType::Text_Push, components::AbilityType::Push, content::KEY_TEXT_ANIMATED_PUSH);
        };
        map[entities::EntityCode::Text_Pull] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-pull", components::TextType::Text_Pull, components::AbilityType::Pull, content::KEY_TEXT_ANIMATED_PULL);
        };
        map[entities::EntityCode::Text_Water] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-water", components::TextType::Text_Water, components::AbilityType::None, content::KEY_TEXT_ANIMATED_WATER);
        };
        map[entities::EntityCode::Text_Chill] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-chill", components::TextType::Text_Chill, components::AbilityType::Chill, content::KEY_TEXT_ANIMATED_CHILL);
        };
        map[entities::EntityCode::Text_Steep] = [](misc::HexCoord position)
        {
            return entities::createText(position, "text-steep", components::TextType::Text_Steep, components::AbilityType::None, content::KEY_TEXT_ANIMATED_STEEP);
        };

        return map;
    }
} // namespace entities
