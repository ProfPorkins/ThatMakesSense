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
#include "components/Position.hpp"

#include <chrono>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system executes the rule abilities of Hot and Water that are
    // currently applied to the entities.  The movement system handles
    // all the rules/abilities associated with movement, of course.
    //
    // --------------------------------------------------------------
    class RuleExecute : public System
    {
      public:
        enum class RemoveReason
        {
            Hot,
            Water,
            Collateral
        };

        // We are only tracking entities with abilities, because those are the only ones that
        // can affect things by the time this sytem executes.
        RuleExecute(std::shared_ptr<Level> level, std::function<void(entities::Entity::IdType, RemoveReason)> notifyRemove) :
            System({ ctti::unnamed_type_id<components::Position>(),
                     ctti::unnamed_type_id<components::Ability>() }),
            m_level(level),
            m_notifyRemove(notifyRemove)
        {
        }

        void update(std::chrono::microseconds elapsedTime) override;

      private:
        std::shared_ptr<Level> m_level;
        std::function<void(entities::Entity::IdType, RemoveReason)> m_notifyRemove;

        void checkAction(entities::EntityPtr entity);
    };
} // namespace systems
