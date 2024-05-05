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

#include "Completion.hpp"

#include "components/Object.hpp"

#include <algorithm>
#include <cassert>
#include <map>
#include <ranges>

namespace systems
{
    // --------------------------------------------------------------
    //
    // The winning condition is met if any entity that has the I
    // property is in the same location as any entity that has the
    // Goal property.
    //
    // --------------------------------------------------------------
    void Completion::update([[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        const auto hasGoal = [this](auto colocated)
        {
            if (m_markedForRemoval(colocated->getId()))
            {
                return false;
            }
            return colocated->template hasComponent<components::Property>() &&
                   colocated->template getComponent<components::Property>()->has(components::PropertyType::Goal);
        };

        // Let's go through all the I entities, get the entities at those positions
        // in the level and see if there are an Goal entities in the same position.
        std::unordered_set<entities::Entity::IdType> alreadyCounted;
        bool completed{ false };
        std::map<components::ObjectType, std::uint16_t> score;

        for (auto&& entity : m_entities | std::views::values)
        {
            auto position = entity->getComponent<components::Position>()->get();

            for (auto&& goal : m_level->getEntities(position) | std::views::values | std::views::filter(hasGoal))
            {
                // We can have the condition that an I object can also be an Goal object.  We want to count that
                // but not double count it if there are two (or more) I & Goal objects in the same position.  To handle this,
                // once a Goal entity has been counted, we don't look at it again.
                if (!alreadyCounted.contains(goal->getId()))
                {
                    completed = true;

                    assert(goal->hasComponent<components::Object>());
                    auto object = goal->getComponent<components::Object>()->getType();
                    if (!score.contains(object))
                    {
                        score.insert({ object, static_cast<std::uint16_t>(0) });
                    }
                    score[object]++;
                    alreadyCounted.insert(goal->getId());
                }
            }
        }

        if (completed)
        {
            // Convert the score to a ChallengeGroup for notification back to the GameModel and for Scoring
            Scoring::ChallengeGroup group;
            std::transform(score.begin(), score.end(), std::back_inserter(group), [](auto& item)
                           {
                               return item;
                           });

            m_notifyCompletion(group);
        }
    }

    // --------------------------------------------------------------
    //
    // We'll just track all the I entities, then check their positions
    // in the level for any entities that have the Goal property.
    //
    // --------------------------------------------------------------
    bool Completion::isInterested(const entities::EntityPtr& entity)
    {
        return System::isInterested(entity) && entity->getComponent<components::Property>()->has(components::PropertyType::I);
    }

} // namespace systems
