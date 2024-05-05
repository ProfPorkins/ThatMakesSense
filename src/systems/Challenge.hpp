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
#include "components/Challenge.hpp"
#include "entities/Entity.hpp"
#include "services/Scoring.hpp"

#include <chrono>
#include <functional>
#include <queue>

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system updates the state of the active challenge hint
    //
    // --------------------------------------------------------------
    class Challenge : public System
    {
      public:
        Challenge(std::shared_ptr<Level> level, std::function<void(entities::EntityPtr)> addEntity, std::function<void(entities::Entity::IdType id)> removeEntity) :
            System({ ctti::unnamed_type_id<components::Challenge>() }),
            m_level(level),
            m_addEntity(addEntity),
            m_notifyRemove(removeEntity),
            m_startedWithChallenges(anyRemainingChallenges())
        {
            initializeChallenges();
        }

        bool addEntity(entities::EntityPtr entity) override;
        void update(std::chrono::microseconds elapsedTime) override;
        void levelComplete();

      private:
        std::shared_ptr<Level> m_level;
        std::function<void(entities::EntityPtr)> m_addEntity;
        std::function<void(entities::Entity::IdType removeMeId)> m_notifyRemove;
        std::queue<Scoring::ChallengeGroup> m_challenges;
        entities::EntityPtr m_activeChallenge{ nullptr };
        bool m_startedWithChallenges{ false };
        bool m_signalPuzzleComplete{ false };

        void initializeChallenges();
        void cycleChallenge();
        bool anyRemainingChallenges();
    };
} // namespace systems
