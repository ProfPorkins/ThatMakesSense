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
#include "components/Position.hpp"
#include "components/Property.hpp"
#include "services/Scoring.hpp"

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system checks for to see if the winning condition has been
    // satisfied for a level.
    //
    // --------------------------------------------------------------
    class Completion : public System
    {
      public:
        Completion(std::shared_ptr<Level> level, std::function<void(const Scoring::ChallengeGroup&)> notifyCompletion, std::function<bool(const entities::Entity::IdType)> markedForRemoval) :
            System({ ctti::unnamed_type_id<components::Position>(),
                     ctti::unnamed_type_id<components::Property>() }),
            m_level(level),
            m_notifyCompletion(notifyCompletion),
            m_markedForRemoval(markedForRemoval)
        {
        }

        void update(std::chrono::microseconds elapsedTime) override;

      protected:
        bool isInterested(const entities::EntityPtr& entity) override;

      private:
        std::shared_ptr<Level> m_level;
        std::function<void(const Scoring::ChallengeGroup&)> m_notifyCompletion;
        std::function<bool(const entities::Entity::IdType)> m_markedForRemoval;
    };
} // namespace systems
