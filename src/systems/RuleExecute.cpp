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

#include "RuleExecute.hpp"

#include "components/Ability.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"

namespace systems
{
    void RuleExecute::update([[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        for (auto&& [id, entity] : m_entities)
        {
            checkAction(entity);
        }
    }

    // --------------------------------------------------------------
    //
    // An entity may have moved or rules have changed, this is where
    // we check to see if something should take place in terms of
    // being wiped out!
    //
    // --------------------------------------------------------------
    void RuleExecute::checkAction(entities::EntityPtr entity)
    {
        auto abilityEntity = entity->getComponent<components::Ability>();
        auto propertyEntity = entity->getComponent<components::Property>();
        auto position = entity->getComponent<components::Position>()->get();
        for (auto&& [id, entityInPlace] : m_level->getEntities(position))
        {
            if (entityInPlace->getId() != entity->getId())
            {
                if (entityInPlace->hasComponent<components::Property>())
                {
                    auto abilityInPlace = entityInPlace->getComponent<components::Ability>();
                    auto propertyInPlace = entityInPlace->getComponent<components::Property>();
                    if (propertyInPlace->has(components::PropertyType::Water) && !abilityEntity->has(components::AbilityType::Float))
                    {
                        m_notifyRemove(entity->getId(), RemoveReason::Water);
                        m_notifyRemove(entityInPlace->getId(), RemoveReason::Collateral);
                    }
                    if (propertyEntity->has(components::PropertyType::Water) && !abilityInPlace->has(components::AbilityType::Float))
                    {
                        m_notifyRemove(entityInPlace->getId(), RemoveReason::Water);
                        m_notifyRemove(entity->getId(), RemoveReason::Collateral);
                    }
                    if (propertyInPlace->has(components::PropertyType::Hot) && !abilityEntity->has(components::AbilityType::Chill))
                    {
                        m_notifyRemove(entity->getId(), RemoveReason::Hot);
                    }
                    if (propertyEntity->has(components::PropertyType::Hot) && !abilityInPlace->has(components::AbilityType::Chill))
                    {
                        m_notifyRemove(entityInPlace->getId(), RemoveReason::Hot);
                    }
                }
            }
        }
    }
} // namespace systems
