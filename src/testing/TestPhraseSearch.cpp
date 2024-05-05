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

#include "Level.hpp"
#include "components/Object.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"
#include "systems/parser/PhraseSearch.hpp"

#include <deque>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

// --------------------------------------------------------------
//
// Read the json config file, it is needed to load the content
// that is required by the level when it initializes.
//
// --------------------------------------------------------------
auto readConfiguration()
{
    const std::string CONFIG_SETTINGS_FILENAME = "client.settings.json";
    const std::string CONFIG_DEVELOPER_FILENAME = "client.developer.json";

    // Reference: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    // Using Jerry's answer, because it was benchmarked to be quite fast, even though the config files are small.
    std::ifstream inSettings(CONFIG_SETTINGS_FILENAME);
    std::stringstream bufferSettings;
    bufferSettings << inSettings.rdbuf();
    inSettings.close();

    std::stringstream bufferDeveloper;
    std::ifstream inDeveloper(CONFIG_DEVELOPER_FILENAME);
    if (inDeveloper)
    {
        bufferDeveloper << inDeveloper.rdbuf();
        inDeveloper.close();
    }

    return Configuration::instance().initialize(bufferSettings.str(), bufferDeveloper.str());
}

// --------------------------------------------------------------
//
// Yes, have to load all this content, because it is needed by
// the factory as entities are created during the level initialization.
//
// --------------------------------------------------------------
void loadContent()
{
    //
    // Textures
    std::vector<std::pair<std::string, config::config_path>> textures{
        { content::KEY_IMAGE_ANIMATED_ENTITY_WALL, config::IMAGE_ENTITY_ANIMATED_WALL },
        { content::KEY_IMAGE_ANIMATED_ENTITY_FLOOR, config::IMAGE_ENTITY_ANIMATED_FLOOR },
        { content::KEY_IMAGE_ANIMATED_ENTITY_GRASS, config::IMAGE_ENTITY_ANIMATED_GRASS },
        { content::KEY_IMAGE_ANIMATED_ENTITY_FLOWERS, config::IMAGE_ENTITY_ANIMATED_FLOWERS },

        { content::KEY_TEXT_ANIMATED_IS, config::IMAGE_ENTITY_TEXT_IS },
        { content::KEY_TEXT_ANIMATED_AM, config::IMAGE_ENTITY_TEXT_AM },
        { content::KEY_TEXT_ANIMATED_CAN, config::IMAGE_ENTITY_TEXT_CAN },
        { content::KEY_TEXT_ANIMATED_AND, config::IMAGE_ENTITY_TEXT_AND },

        { content::KEY_TEXT_ANIMATED_GOAL, config::IMAGE_ENTITY_TEXT_GOAL },
        { content::KEY_TEXT_ANIMATED_CLIMB, config::IMAGE_ENTITY_TEXT_CLIMB },
        { content::KEY_TEXT_ANIMATED_FLOAT, config::IMAGE_ENTITY_TEXT_FLOAT },
        { content::KEY_TEXT_ANIMATED_CHILL, config::IMAGE_ENTITY_TEXT_CHILL },
        { content::KEY_TEXT_ANIMATED_PUSH, config::IMAGE_ENTITY_TEXT_PUSH },
        { content::KEY_TEXT_ANIMATED_PULL, config::IMAGE_ENTITY_TEXT_PULL },
        { content::KEY_TEXT_ANIMATED_STOP, config::IMAGE_ENTITY_TEXT_STOP },
        { content::KEY_TEXT_ANIMATED_STEEP, config::IMAGE_ENTITY_TEXT_STEEP },
        { content::KEY_TEXT_ANIMATED_WATER, config::IMAGE_ENTITY_TEXT_WATER },
        { content::KEY_TEXT_ANIMATED_HOT, config::IMAGE_ENTITY_TEXT_HOT },

        { content::KEY_TEXT_ANIMATED_I, config::IMAGE_ENTITY_TEXT_I },
        { content::KEY_TEXT_ANIMATED_GOAL, config::IMAGE_ENTITY_TEXT_GOAL },
        { content::KEY_TEXT_ANIMATED_WORD, config::IMAGE_ENTITY_TEXT_WORD },
        { content::KEY_TEXT_ANIMATED_WALL, config::IMAGE_ENTITY_TEXT_WALL },
        { content::KEY_TEXT_ANIMATED_FLOOR, config::IMAGE_ENTITY_TEXT_FLOOR },
        { content::KEY_TEXT_ANIMATED_FLOWERS, config::IMAGE_ENTITY_TEXT_FLOWERS },
        { content::KEY_TEXT_ANIMATED_GRASS, config::IMAGE_ENTITY_TEXT_GRASS },
        { content::KEY_TEXT_ANIMATED_PURPLE, config::IMAGE_ENTITY_TEXT_PURPLE },
        { content::KEY_TEXT_ANIMATED_GREY, config::IMAGE_ENTITY_TEXT_GREY },
        { content::KEY_TEXT_ANIMATED_GREEN, config::IMAGE_ENTITY_TEXT_GREEN },
        { content::KEY_TEXT_ANIMATED_BLUE, config::IMAGE_ENTITY_TEXT_BLUE },
        { content::KEY_TEXT_ANIMATED_RED, config::IMAGE_ENTITY_TEXT_RED },
        { content::KEY_TEXT_ANIMATED_BROWN, config::IMAGE_ENTITY_TEXT_BROWN },
        { content::KEY_TEXT_ANIMATED_YELLOW, config::IMAGE_ENTITY_TEXT_YELLOW },
    };

    for (auto&& [keyContent, keyConfig] : textures)
    {
        if (!Content::has<sf::Texture>(keyContent))
        {
            Content::load<sf::Texture>(keyContent, Configuration::get<std::string>(keyConfig), nullptr, nullptr);
        }
    }

    Content::load<Levels>(content::KEY_LEVELS, "levels-unittests.puzzles", nullptr, nullptr);

    // Busy wait for the content to finish loading.  It's okay, it happens super fast first time level is started
    while (Content::instance().anyPending())
        ;
}

bool containsPhrase(std::vector<std::deque<systems::parser::Parser::PhrasePair>>& phrases, std::deque<components::TextType>& phrase)
{
    for (auto&& possible : phrases)
    {
        // If we find out differently, we'll change our mind
        bool hasAllWords{ true };

        if (possible.size() == phrase.size())
        {
            for (std::size_t word = 0; word < phrase.size(); word++)
            {
                if (possible[word].word != phrase[word])
                {
                    hasAllWords = false;
                }
            }
        }
        else
        {
            hasAllWords = false;
        }

        if (hasAllWords)
        {
            return true;
        }
    }

    return false;
}

TEST(HexCoord, GetDirection)
{
    using namespace misc;

    // Odd row
    EXPECT_EQ(HexCoord::getDirection({ 3, 3 }, { 2, 3 }), HexCoord::Direction::W);
    EXPECT_EQ(HexCoord::getDirection({ 3, 3 }, { 4, 3 }), HexCoord::Direction::E);
    EXPECT_EQ(HexCoord::getDirection({ 3, 3 }, { 3, 2 }), HexCoord::Direction::NW);
    EXPECT_EQ(HexCoord::getDirection({ 3, 3 }, { 4, 2 }), HexCoord::Direction::NE);
    EXPECT_EQ(HexCoord::getDirection({ 3, 3 }, { 3, 4 }), HexCoord::Direction::SW);
    EXPECT_EQ(HexCoord::getDirection({ 3, 3 }, { 4, 4 }), HexCoord::Direction::SE);

    // Even row
    EXPECT_EQ(HexCoord::getDirection({ 2, 2 }, { 1, 2 }), HexCoord::Direction::W);
    EXPECT_EQ(HexCoord::getDirection({ 2, 2 }, { 3, 2 }), HexCoord::Direction::E);
    EXPECT_EQ(HexCoord::getDirection({ 2, 2 }, { 1, 1 }), HexCoord::Direction::NW);
    EXPECT_EQ(HexCoord::getDirection({ 2, 2 }, { 2, 1 }), HexCoord::Direction::NE);
    EXPECT_EQ(HexCoord::getDirection({ 2, 2 }, { 1, 3 }), HexCoord::Direction::SW);
    EXPECT_EQ(HexCoord::getDirection({ 2, 2 }, { 2, 3 }), HexCoord::Direction::SE);
}

TEST(SinglePhrase, PhraseColors1)
{
    using namespace std::string_literals;

    readConfiguration();
    loadContent();

    auto l = Content::getLevels().get("PhraseColors1"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 3);

    using namespace components;
    if (phrases.size() == 3)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Am, TextType::Text_Purple };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
    }
}

TEST(SinglePhrase, PhraseColors2)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseColors2"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 3);

    using namespace components;
    if (phrases.size() == 3)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Am, TextType::Text_Purple };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
    }
}

TEST(SinglePhrase, PhraseColors3)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseColors3"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 3);

    using namespace components;
    if (phrases.size() == 3)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Am, TextType::Text_Purple };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
    }
}

TEST(SinglePhrase, PhraseColors4)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseColors4"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 6);

    using namespace components;
    if (phrases.size() == 6)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Am, TextType::Text_Purple };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red };
        std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Grey };
        std::deque<TextType> phrase5{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Yellow };
        std::deque<TextType> phrase6{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Blue };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
        EXPECT_EQ(containsPhrase(phrases, phrase4), true);
        EXPECT_EQ(containsPhrase(phrases, phrase5), true);
        EXPECT_EQ(containsPhrase(phrases, phrase6), true);
    }
}

TEST(SinglePhrase, PhraseColors5)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseColors5"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 6);

    using namespace components;
    if (phrases.size() == 6)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Am, TextType::Text_Purple };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red };
        std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Grey };
        std::deque<TextType> phrase5{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Yellow };
        std::deque<TextType> phrase6{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Blue };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
        EXPECT_EQ(containsPhrase(phrases, phrase4), true);
        EXPECT_EQ(containsPhrase(phrases, phrase5), true);
        EXPECT_EQ(containsPhrase(phrases, phrase6), true);
    }
}

TEST(SinglePhrase, PhraseAbilities1)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseAbilities1"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 3);

    using namespace components;
    if (phrases.size() == 3)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Can, TextType::Text_Pull };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
    }
}

TEST(SinglePhrase, PhraseAbilities2)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseAbilities2"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 3);

    using namespace components;
    if (phrases.size() == 3)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Can, TextType::Text_Pull };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
    }
}

TEST(SinglePhrase, PhraseAbilities3)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseAbilities3"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 4);

    using namespace components;
    if (phrases.size() == 4)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Can, TextType::Text_Pull };
        std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Float };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
        EXPECT_EQ(containsPhrase(phrases, phrase4), true);
    }
}

TEST(SinglePhrase, PhraseAbilities4)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseAbilities4"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 4);

    using namespace components;
    if (phrases.size() == 4)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Can, TextType::Text_Pull };
        std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Can, TextType::Text_Float };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
        EXPECT_EQ(containsPhrase(phrases, phrase4), true);
    }
}

TEST(SinglePhrase, PhraseColorsAbilities1)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseColorsAbilities1"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 2);

    using namespace components;
    if (phrases.size() == 2)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb, TextType::Text_And, TextType::Text_Am, TextType::Text_Blue };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
    }
}

TEST(SinglePhrase, PhraseColorsAbilities2)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseColorsAbilities2"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 6);

    using namespace components;
    if (phrases.size() == 6)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push };
        std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Can, TextType::Text_Pull };
        std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb, TextType::Text_And, TextType::Text_Am, TextType::Text_Blue };
        std::deque<TextType> phrase5{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb, TextType::Text_And, TextType::Text_Am, TextType::Text_Purple };
        std::deque<TextType> phrase6{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb, TextType::Text_And, TextType::Text_Am, TextType::Text_Red };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
        EXPECT_EQ(containsPhrase(phrases, phrase4), true);
        EXPECT_EQ(containsPhrase(phrases, phrase5), true);
        EXPECT_EQ(containsPhrase(phrases, phrase6), true);
    }
}

TEST(SinglePhrase, PhraseColorsAbilities3)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseColorsAbilities3"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 2);

    using namespace components;
    if (phrases.size() == 2)
    {
        std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Am, TextType::Text_Blue };
        std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am, TextType::Text_Blue, TextType::Text_And, TextType::Text_Can, TextType::Text_Climb };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
    }
}

TEST(SinglePhrase, PhraseObjectsProperties1)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseObjectsProperties1"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 1);

    using namespace components;
    if (phrases.size() == 1)
    {
        std::deque<TextType> phrase1{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
    }
}

TEST(SinglePhrase, PhraseObjectsProperties2)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseObjectsProperties2"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 2);

    using namespace components;
    if (phrases.size() == 2)
    {
        std::deque<TextType> phrase1{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop };
        std::deque<TextType> phrase2{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop, TextType::Text_And, TextType::Text_Hot };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
    }
}

TEST(SinglePhrase, PhraseObjectsProperties3)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseObjectsProperties3"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 2);

    using namespace components;
    if (phrases.size() == 2)
    {
        std::deque<TextType> phrase1{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop };
        std::deque<TextType> phrase2{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop, TextType::Text_And, TextType::Text_Is, TextType::Text_Hot };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
    }
}

TEST(SinglePhrase, PhraseObjectsProperties4)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseObjectsProperties4"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 3);

    using namespace components;
    if (phrases.size() == 3)
    {
        std::deque<TextType> phrase1{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Hot };
        std::deque<TextType> phrase2{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop };
        std::deque<TextType> phrase3{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Steep };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
    }
}

TEST(SinglePhrase, PhraseObjectsObjects1)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseObjectsObjects1"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 1);

    using namespace components;
    if (phrases.size() == 1)
    {
        std::deque<TextType> phrase1{ TextType::Text_Purple, TextType::Text_Is, TextType::Text_Grey };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
    }
}

TEST(SinglePhrase, PhraseObjectsObjects2)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseObjectsObjects2"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 3);

    using namespace components;
    if (phrases.size() == 3)
    {
        std::deque<TextType> phrase1{ TextType::Text_Blue, TextType::Text_Is, TextType::Text_Purple };
        std::deque<TextType> phrase2{ TextType::Text_Blue, TextType::Text_Is, TextType::Text_Green };
        std::deque<TextType> phrase3{ TextType::Text_Blue, TextType::Text_Is, TextType::Text_Grey };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
        EXPECT_EQ(containsPhrase(phrases, phrase3), true);
    }
}

TEST(SinglePhrase, PhraseObjectsObjects3)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseObjectsObjects3"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 2);

    using namespace components;
    if (phrases.size() == 2)
    {
        std::deque<TextType> phrase1{ TextType::Text_Blue, TextType::Text_Is, TextType::Text_Grey };
        std::deque<TextType> phrase2{ TextType::Text_Purple, TextType::Text_And, TextType::Text_Blue, TextType::Text_Is, TextType::Text_Grey };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
        EXPECT_EQ(containsPhrase(phrases, phrase2), true);
    }
}

TEST(SinglePhrase, PhraseBlueIsGoal)
{
    using namespace std::string_literals;

    auto l = Content::getLevels().get("PhraseBlueIsGoal"s);
    l->initialize([&](entities::EntityPtr entity)
                  {
                      // Yes, we add the entity here.  In the game code the GameModel calls this
                      // method to add the entities, but in the unit testing, we go ahead and
                      // and add it here.
                      l->addEntity(entity);
                  });

    systems::parser::PhraseSearch search;
    components::PhraseDirection::DirectionGrid gridDirection;

    auto phrases = search.search(*l, gridDirection);
    EXPECT_EQ(phrases.size(), 1);

    using namespace components;
    if (phrases.size() == 1)
    {
        std::deque<TextType> phrase1{ TextType::Text_Blue, TextType::Text_Is, TextType::Text_Goal };

        EXPECT_EQ(containsPhrase(phrases, phrase1), true);
    }
}