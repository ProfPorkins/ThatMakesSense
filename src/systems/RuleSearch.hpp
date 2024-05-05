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

#include "Level.hpp"
#include "System.hpp"
#include "components/Ability.hpp"
#include "components/Noun.hpp"
#include "components/Object.hpp"
#include "components/PhraseDirection.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"
#include "components/Verb.hpp"
#include "entities/Entity.hpp"
#include "systems/parser/Parser.hpp"
#include "systems/parser/PhraseSearch.hpp"
#include "systems/parser/SemanticParser.hpp"

#include <chrono>
#include <deque>
#include <functional>
#include <optional>
#include <tuple>
#include <unordered_set>
#include <variant>
#include <vector>

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system parses the game board looking for rules that should
    // be applied or un-applied to the entities.
    //
    // --------------------------------------------------------------
    class RuleSearch : public System
    {
      public:
        RuleSearch(std::shared_ptr<Level> level, std::function<void(entities::EntityPtr)> addEntity, std::function<void(entities::Entity::IdType)> removeEntity, std::function<void(entities::Entity::IdType)> notifyUpdated, std::function<void(const entities::EntitySet&)> notifyGoalChanged, std::function<void(const entities::EntitySet&)> notifyIChanged, std::function<void(misc::HexCoord position)> notifyNewPhrase) :
            System({}),
            m_level(level),
            m_addEntity(addEntity),
            m_removeEntity(removeEntity),
            m_notifyUpdated(notifyUpdated),
            m_notifyGoalChanged(notifyGoalChanged),
            m_notifyIChanged(notifyIChanged),
            m_notifyNewPhrase(notifyNewPhrase)
        {
        }

        void update(std::chrono::microseconds elapsedTime) override;
        void signalStateChange()
        {
            m_updateRules = true;
        }

      protected:
        // Don't need to track any entities here, because we only look at entities through the level
        // bool isInterested([[maybe_unused]] entities::Entity* entity) override { return false; }

      private:
        enum class Traversal : std::uint8_t
        {
            None,
            Visited,
            Group,
        };

        bool m_updateRules{ false };
        std::shared_ptr<Level> m_level;
        std::function<void(entities::EntityPtr)> m_addEntity;
        std::function<void(entities::Entity::IdType)> m_removeEntity;
        std::function<void(entities::Entity::IdType)> m_notifyUpdated;
        std::function<void(const entities::EntitySet&)> m_notifyGoalChanged;
        std::function<void(const std::unordered_set<entities::Entity::IdType>&)> m_notifyIChanged;
        std::function<void(misc::HexCoord position)> m_notifyNewPhrase;
        components::PhraseDirection::DirectionGrid m_gridPhraseDirection;
        std::uint32_t m_previousPhrasesHash{ 0 };
        std::vector<std::deque<systems::parser::Parser::PhrasePair>> m_previousPhrases;
        std::unordered_set<std::uint32_t> m_previousHashes;
        entities::EntitySet m_previousGoalEntities;
        entities::EntitySet m_previousSendEntities;
        entities::EntitySet m_previousIEntities;

        static constexpr auto MIN_UNDOHINT_DELAY = std::chrono::seconds{ 20 };
        std::chrono::microseconds m_timeSinceUndoHint{ MIN_UNDOHINT_DELAY };

        void updateNotifications(std::chrono::microseconds& elapsedTime);
        void applyRules(const systems::parser::SemanticParser::SemanticRuleSet& rules);
        void clean();
        void notifyNewPhrases(std::vector<std::deque<systems::parser::Parser::PhrasePair>>& current);
        void notifyIChanges(entities::EntitySet& currentEntities);
        void notifyGoalChanges(entities::EntitySet& currentEntities);
        void notifySendChanges(entities::EntitySet& currentEntities);
    };
} // namespace systems
