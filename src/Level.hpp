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

#include "components/Object.hpp"
#include "entities/Entity.hpp"
#include "misc/HexCoord.hpp"
#include "services/Scoring.hpp"

#include <cstdint>
#include <functional>
#include <gtest/gtest_prod.h>
#include <map>
#include <optional>
#include <string>
#include <vector>

class Level
{
  public:
    // This is used to keep the entities stored by rendering order, to ensure
    // well, they are rendered in the correct order.
    using RenderOrderStorage = std::multimap<components::ObjectType, entities::EntityPtr>;

    Level(std::string name, std::string hint, std::string uuid, std::string challenges, std::uint8_t layers, std::uint16_t width, std::uint16_t height, misc::HexCoord cameraStartPos, std::uint8_t cameraStartRange);

    void initialize(std::function<void(entities::EntityPtr)> addEntity);

    const auto& getUUID() { return m_uuid; }
    const auto& getName() { return m_name; }
    const auto& getHint() { return m_hint; }
    auto getWidth() const { return m_width; }
    auto getHeight() const { return m_height; }
    auto getCameraStartPos() { return m_cameraStartPos; }
    auto getCameraStartRange() { return m_cameraStartRange; }
    bool isValid(const misc::HexCoord& cell);

    const Scoring::LevelChallenges& getChallenges() { return m_challenges; }
    std::optional<Scoring::ChallengeGroup> matchChallenge(const Scoring::ChallengeGroup& result);

    void clear();
    void addEntity(entities::EntityPtr entity);
    void removeEntity(entities::Entity::IdType entityId);
    const entities::EntityMap& getEntities(const misc::HexCoord& cell) const;
    RenderOrderStorage getEntitiesByRender(const misc::HexCoord& cell);

    void moveEntity(entities::EntityPtr entity, misc::HexCoord previous);

  private:
    const std::string HINT_MOVEMENT{ "HINT: MOVEMENT" };
    const std::string HINT_PULL{ "HINT: PULL" };
    const std::string HINT_CAMERA{ "HINT: CAMERA" };
    std::string m_name;
    std::string m_hint;
    std::string m_uuid;
    std::uint16_t m_width;
    std::uint16_t m_height;
    misc::HexCoord m_cameraStartPos;
    std::uint8_t m_cameraStartRange;

    friend class Levels;
    FRIEND_TEST(SinglePhrase, PhraseColors1);
    FRIEND_TEST(SinglePhrase, PhraseColors2);
    FRIEND_TEST(SinglePhrase, PhraseColors3);
    FRIEND_TEST(SinglePhrase, PhraseColors4);
    FRIEND_TEST(SinglePhrase, PhraseColors5);
    FRIEND_TEST(SinglePhrase, PhraseAbilities1);
    FRIEND_TEST(SinglePhrase, PhraseAbilities2);
    FRIEND_TEST(SinglePhrase, PhraseAbilities3);
    FRIEND_TEST(SinglePhrase, PhraseAbilities4);
    FRIEND_TEST(SinglePhrase, PhraseColorsAbilities1);
    FRIEND_TEST(SinglePhrase, PhraseColorsAbilities2);
    FRIEND_TEST(SinglePhrase, PhraseColorsAbilities3);
    FRIEND_TEST(SinglePhrase, PhraseObjectsProperties1);
    FRIEND_TEST(SinglePhrase, PhraseObjectsProperties2);
    FRIEND_TEST(SinglePhrase, PhraseObjectsProperties3);
    FRIEND_TEST(SinglePhrase, PhraseObjectsProperties4);
    FRIEND_TEST(SinglePhrase, PhraseObjectsObjects1);
    FRIEND_TEST(SinglePhrase, PhraseObjectsObjects2);
    FRIEND_TEST(SinglePhrase, PhraseObjectsObjects3);
    FRIEND_TEST(SinglePhrase, ObjectIsGoal);

    std::vector<std::vector<std::string>> m_levelData;
    Scoring::LevelChallenges m_challenges;

    // Hex coordinate system is [q, r]
    // File and array storage system is [r, q]
    // hash table on entity id for all entities at this location
    std::vector<std::vector<entities::EntityMap>> m_entitiesByHash;
    std::vector<std::vector<RenderOrderStorage>> m_entitiesByRenderOrder;
    // Used to lookup any entity, regardless of position - needed for removing entities
    entities::EntityMap m_entitiesAll;

    void initialize(const std::vector<std::string>& levelData, std::function<void(entities::EntityPtr)> addEntity);
};
