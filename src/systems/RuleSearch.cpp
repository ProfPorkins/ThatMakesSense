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

#include "RuleSearch.hpp"

#include "components/Ability.hpp"
#include "components/AnimatedSprite.hpp"
#include "components/Audio.hpp"
#include "components/Hint.hpp"
#include "components/InputControlled.hpp"
#include "components/Noun.hpp"
#include "components/Object.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"
#include "components/StaticSprite.hpp"
#include "components/Verb.hpp"
#include "entities/Factory.hpp"
#include "misc/math.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/ContentKey.hpp"
#include "services/ControllerInput.hpp"

#include <deque>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <ranges>
#include <unordered_map>
#include <utility> // std::move
#include <vector>

namespace systems
{
    static const auto isDirection = [](auto entity)
    {
        return entity->template hasComponent<components::PhraseDirection>();
    };

    static const auto isNotDirection = [](auto entity)
    {
        return !entity->template hasComponent<components::PhraseDirection>();
    };

    // --------------------------------------------------------------
    //
    // A simple hash computation, used in determining which are newly
    // discovered phrases.
    //
    // --------------------------------------------------------------
    std::uint32_t hashPhrase(const std::deque<systems::parser::Parser::PhrasePair>& phrase)
    {
        std::uint32_t hash{ 0 };

        for (std::size_t word = 0; word < phrase.size(); word++)
        {
            hash = ((31 * hash) + static_cast<std::uint32_t>(phrase[word].word));
        }

        return hash;
    }

    // --------------------------------------------------------------
    //
    // A simple hash computation, used to see if the current set of rules
    // is different from the previous set of rules.
    //
    // --------------------------------------------------------------
    std::uint32_t hashPhrases(const std::vector<std::deque<systems::parser::Parser::PhrasePair>>& phrases)
    {
        std::uint32_t hash{ 0 };

        for (auto&& phrase : phrases)
        {
            for (std::size_t word = 0; word < phrase.size(); word++)
            {
                hash = ((31 * hash) + static_cast<std::uint32_t>(phrase[word].word));
            }
        }

        return hash;
    }

    // --------------------------------------------------------------
    //
    // Debugging function used to report the discovered phrases
    //
    // --------------------------------------------------------------
    void reportPhrases(const std::vector<std::deque<systems::parser::Parser::PhrasePair>>& phrases)
    {
        std::cout << "--- Discovered Phrases ---" << std::endl;
        for (auto&& phrase : phrases)
        {
            for (std::size_t word = 0; word < phrase.size(); word++)
            {
                std::cout << components::TextTypeToString.at(phrase[word].word) << " ";
            }
            std::cout << std::endl;
        }
    }

    // --------------------------------------------------------------
    //
    // Debugging function used to report the discovered rules and
    // their structure.
    //
    // --------------------------------------------------------------
    void reportRules(const systems::parser::SemanticParser::SemanticRuleSet& rules)
    {
        std::cout << "--- Rule Structure ---" << std::endl;
        for (auto&& [noun, functions] : rules)
        {
            std::cout << components::NounTypeToString.at(noun) << std::endl;
            for (auto&& function : functions)
            {
                std::visit([](auto&& arg)
                           {
                               using T = std::decay_t<decltype(arg)>;
                               if constexpr (std::is_same_v<T, components::AbilityType>)
                               {
                                   std::cout << "    " << components::AbilityTypeToString.at(arg) << std::endl;
                               }
                               else if constexpr (std::is_same_v<T, components::PropertyType>)
                               {
                                   std::cout << "    " << components::PropertyTypeToString.at(arg) << std::endl;
                               }
                               else if constexpr (std::is_same_v<T, components::NounType>)
                               {
                                   std::cout << "    " << components::NounTypeToString.at(arg) << std::endl;
                               }
                           },
                           function);
            }
        }
    }

    void RuleSearch::update([[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        if (m_updateRules)
        {
            // Remove all previous phrase direction entities
            for (auto&& entity : m_entities | std::views::values | std::views::filter(isDirection))
            {
                m_removeEntity(entity->getId());
            }

            // NOTE: Staying in an update loop until the rules stop changing.  Not actually verified, but think
            //       this should repeat at most 1 time (2 loops).  I'll eventually need to test the performance
            //       of this to decide if this is okay, or if the updates need to be spread over two frames.
            bool done{ false };
            while (!done)
            {
                // Find the new phrase state
                systems::parser::PhraseSearch phraseSearch;
                auto phrases = phraseSearch.search(*m_level, m_gridPhraseDirection);
                // reportPhrases(phrases);

                // Check the phrases before they are parsed (and destructed) to see if there are any changes
                // in the current set versus the previous set.  If there are changes, we want to come back in
                // the next frame and do another update.
                auto phrasesHash = hashPhrases(phrases);
                done = (phrasesHash == m_previousPhrasesHash);
                m_previousPhrasesHash = phrasesHash;

                // Add the current phrase direction entities - and we only want to add them in
                // if it is the last time through, otherwise direction arrows will get added that
                // won't be removed next time through the loop; plus it is a waste of time to add
                // something that will get removed right away anyway.
                if (done)
                {
                    for (misc::HexCoord::Type r = 0; auto&& row : m_gridPhraseDirection)
                    {
                        for (misc::HexCoord::Type q = 0; auto&& cell : row)
                        {
                            for (auto&& [id, data] : cell)
                            {
                                auto entity = entities::createPhraseDirection(id, { q, r }, std::get<components::PhraseDirection::GridDirection>(data), std::get<components::PhraseDirection::GridElement>(data));
                                m_addEntity(entity);
                            }
                            q++;
                        }
                        r++;
                    }
                }

                // It only makes sense to update the state of things if the rules have changed.
                if (!done)
                {
                    notifyNewPhrases(phrases);
                    m_previousPhrases = phrases;

                    clean();
                    // Find the semantic rules
                    systems::parser::SemanticParser semanticParser;
                    systems::parser::SemanticParser::SemanticRuleSet allRules;
                    for (auto&& phrase : phrases)
                    {
                        auto phraseRules = semanticParser.parse(phrase);
                        for (auto&& [noun, functions] : phraseRules)
                        {
                            // Trying to be efficient by using move operations
                            if (!allRules.contains(noun))
                            {
                                allRules[noun] = std::move(functions);
                            }
                            else
                            {
                                std::move(functions.begin(), functions.end(), std::inserter(allRules[noun], allRules[noun].end()));
                            }
                        }
                    }
                    // reportRules(allRules);
                    applyRules(allRules);
                }
            }

            m_updateRules = false;
        }

        updateNotifications(elapsedTime);
    }

    void RuleSearch::updateNotifications(std::chrono::microseconds& elapsedTime)
    {
        m_timeSinceUndoHint += elapsedTime;
    }

    // --------------------------------------------------------------
    //
    // Given a rules traversal, can now go through the entities and
    // apply the relevant rules.
    //
    // --------------------------------------------------------------
    void RuleSearch::applyRules(const systems::parser::SemanticParser::SemanticRuleSet& rules)
    {
        static const auto isObject = [](entities::EntityPtr entity)
        {
            return entity->template hasComponent<components::Object>();
        };

        //
        // Step 1: Handle all the Noun (except for I) to Noun rules.  We want to get this done
        //         before applying properties and abilities so that we only have the final
        //         set of nouns before applying the properties/abilities.
        for (auto&& [noun, functions] : rules)
        {
            if (noun != components::NounType::I)
            {
                for (auto&& function : functions)
                {
                    std::visit(
                        [&](auto&& arg)
                        {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, components::NounType>)
                            {
                                for (auto&& entity : m_entities | std::views::values | std::views::filter(isObject))
                                {
                                    if (entity->template getComponent<components::Object>()->getType() == components::NounTypeToObjectType.at(noun))
                                    {
                                        entities::transformNoun(entity, arg);
                                        m_notifyUpdated(entity->getId());
                                    }
                                }
                            }
                        },
                        function);
                }
            }
        }

        //
        // Step 2: Handle all the Noun (except for I) based on Property/Ability rules
        entities::EntitySet currentGoalEntities;
        entities::EntitySet currentSendEntities;
        for (auto&& [noun, functions] : rules)
        {
            if (noun != components::NounType::I)
            {
                auto object = components::NounTypeToObjectType.at(noun);
                for (auto&& function : functions)
                {
                    std::visit(
                        [&](auto&& arg)
                        {
                            using T = std::decay_t<decltype(arg)>;
                            if constexpr (std::is_same_v<T, components::AbilityType>)
                            {
                                // I promise I tried to use a view filter, but couldn't get it to compile (even though the syntax was correct!)
                                for (auto&& entity : m_entities | std::views::values)
                                {
                                    if (entity->template hasComponent<components::Object>() && entity->template getComponent<components::Object>()->getType() == object)
                                    {
                                        if (arg == components::AbilityType::Send)
                                        {
                                            currentSendEntities.insert(entity->getId());
                                        }
                                        entity->template getComponent<components::Ability>()->add(arg);
                                        m_notifyUpdated(entity->getId());
                                    }
                                }
                            }
                            else if constexpr (std::is_same_v<T, components::PropertyType>)
                            {
                                // I promise I tried to use a view filter, but couldn't get it to compile (even though the syntax was correct!)
                                for (auto&& entity : m_entities | std::views::values)
                                {
                                    if (entity->template hasComponent<components::Object>() && entity->template getComponent<components::Object>()->getType() == object)
                                    {
                                        if (arg == components::PropertyType::Goal)
                                        {
                                            currentGoalEntities.insert(entity->getId());
                                        }
                                        entity->template getComponent<components::Property>()->add(arg);
                                    }
                                }
                            }
                        },
                        function);
                }
            }
        }

        //
        // Step 3: Handle the I (Noun) rules.

        // First step in this process is to collect all the "I" abilities so they
        // can be applied at once.
        auto abilitiesOfI = std::make_unique<components::Ability>(components::AbilityType::None);
        if (rules.contains(components::NounType::I))
        {
            for (auto&& functionType : rules.at(components::NounType::I))
            {
                std::visit(
                    [&](auto&& arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, components::AbilityType>)
                        {
                            abilitiesOfI->add(arg);
                        }
                    },
                    functionType);
            }
        }
        // Now, apply these abilities to all of the "I" nouns
        bool anyI{ false };
        entities::EntitySet currentIEntities;
        if (rules.contains(components::NounType::I))
        {
            for (auto&& function : rules.at(components::NounType::I))
            {
                std::visit(
                    [&](auto&& arg)
                    {
                        using T = std::decay_t<decltype(arg)>;
                        if constexpr (std::is_same_v<T, components::NounType>)
                        {
                            auto object = components::NounTypeToObjectType.at(arg);
                            // Find all the matching entities and apply
                            for (auto&& [id, entity] : m_entities)
                            {
                                if (entity->template hasComponent<components::Object>() && entity->template getComponent<components::Object>()->getType() == object)
                                {
                                    currentIEntities.insert(entity->getId());

                                    entity->addComponent(std::make_unique<components::InputControlled>());
                                    entity->addComponent(std::make_unique<components::Audio>(content::KEY_AUDIO_STEP));
                                    entity->template getComponent<components::Ability>()->add(abilitiesOfI->get());
                                    entity->template getComponent<components::Property>()->add(components::PropertyType::I);

                                    anyI = true;
                                }
                            }
                        }
                    },
                    function);
            }
        }

        notifyGoalChanges(currentGoalEntities);
        notifySendChanges(currentSendEntities);
        notifyIChanges(currentIEntities);

        // If there are no I entities, then give the user a hint about the undo system
        if (!anyI && m_timeSinceUndoHint >= MIN_UNDOHINT_DELAY)
        {
            auto hint = std::make_shared<entities::Entity>();
            std::string hintString{};
            if (ControllerInput::instance().hasControllerBeenUsed())
            {
                hintString = std::format("Undo {{ X }} or {{ square }}");
            }
            else
            {
                hintString = std::format("Undo Key is '{0}'", Configuration::get<std::string>(config::KEYBOARD_UNDO));
            }
            hint->addComponent(std::make_unique<components::Hint>(hintString, misc::msTous(std::chrono::milliseconds(10000))));
            m_addEntity(hint);

            m_timeSinceUndoHint = std::chrono::microseconds::zero();
        }
    }

    // --------------------------------------------------------------
    //
    // Scan through the entities and remove any abilities and properties
    // from them.
    //
    // --------------------------------------------------------------
    void RuleSearch::clean()
    {
        for (auto&& entity : m_entities | std::views::values | std::views::filter(isNotDirection))
        {
            if (entity->template hasComponent<components::Property>())
            {
                entity->template getComponent<components::Property>()->reset();
            }
            if (entity->template hasComponent<components::Ability>())
            {
                entity->template getComponent<components::Ability>()->reset();
            }
            if (entity->template hasComponent<components::InputControlled>())
            {
                entity->template removeComponent<components::InputControlled>();
                entity->template removeComponent<components::Audio>();
            }
        }
    }

    void RuleSearch::notifyNewPhrases(std::vector<std::deque<systems::parser::Parser::PhrasePair>>& current)
    {
        std::unordered_set<std::uint32_t> currentHashes;

        for (auto&& phrase : current)
        {
            auto hash = hashPhrase(phrase);
            if (!m_previousHashes.contains(hash))
            {
                for (std::size_t word = 0; word < phrase.size(); word++)
                {
                    m_notifyNewPhrase(phrase[word].cell);
                }
            }
            currentHashes.insert(hash);
        }

        m_previousHashes.swap(currentHashes);
    }

    // --------------------------------------------------------------
    //
    // When the I rule(s) change, the game model needs to be notified
    // of the new rules, along with any entities that have changed.
    //
    // --------------------------------------------------------------
    void RuleSearch::notifyIChanges(entities::EntitySet& current)
    {
        // Entities that are I
        for (auto&& test : current)
        {
            if (!m_previousIEntities.contains(test))
            {
                m_notifyUpdated(test);
            }
        }
        for (auto&& test : m_previousIEntities)
        {
            if (!current.contains(test))
            {
                m_notifyUpdated(test);
            }
        }

        // Rule changes
        if (current != m_previousIEntities)
        {
            // The reason for doing it in this order is to ensure that if the code that
            // accepts the entities hangs onto it for a while, it will continue to be valid.
            m_previousIEntities = std::move(current);
            m_notifyIChanged(m_previousIEntities);
        }
    }

    // --------------------------------------------------------------
    //
    // When the Goal rule(s) change, the game model needs to be notified
    // of the updated/new goal entities.
    //
    // --------------------------------------------------------------
    void RuleSearch::notifyGoalChanges(entities::EntitySet& current)
    {
        // Entities that are goals
        for (auto&& test : current)
        {
            if (!m_previousGoalEntities.contains(test))
            {
                m_notifyUpdated(test);
            }
        }
        for (auto&& test : m_previousGoalEntities)
        {
            if (!current.contains(test))
            {
                m_notifyUpdated(test);
            }
        }

        // Rule changes
        if (current != m_previousGoalEntities)
        {
            m_previousGoalEntities = std::move(current);
            m_notifyGoalChanged(m_previousGoalEntities);
        }
    }

    // --------------------------------------------------------------
    //
    // When the Send rule(s) change, the game model needs to be notified
    // of the updated/new send entities
    //
    // --------------------------------------------------------------
    void RuleSearch::notifySendChanges(entities::EntitySet& current)
    {
        // Entities that are send
        for (auto&& test : current)
        {
            if (!m_previousSendEntities.contains(test))
            {
                m_notifyUpdated(test);
            }
        }
        for (auto&& test : m_previousSendEntities)
        {
            if (!current.contains(test))
            {
                m_notifyUpdated(test);
            }
        }

        if (current != m_previousSendEntities)
        {
            m_previousSendEntities = std::move(current);
        }
    }
} // namespace systems
