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

#include "Challenge.hpp"

#include <format>
#include <numeric> //std::accumulate

namespace systems
{
    bool Challenge::addEntity(entities::EntityPtr entity)
    {
        bool added{ false };
        if (System::addEntity(entity))
        {
            added = true;
            m_activeChallenge = entity;
        }

        return added;
    }

    void Challenge::levelComplete()
    {
        m_signalPuzzleComplete = true;
    }

    void Challenge::update([[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        if (!m_signalPuzzleComplete)
        {
            if (m_activeChallenge != nullptr)
            {
                auto challenge = m_activeChallenge->getComponent<components::Challenge>();

                switch (challenge->getState())
                {
                    case components::Challenge::State::In:
                        challenge->updateTimeInState(elapsedTime);
                        if (challenge->getTimeInState() >= components::Challenge::CHALLENGE_TRANSITION_TIME)
                        {
                            challenge->setState(components::Challenge::State::Steady);
                        }
                        break;
                    case components::Challenge::State::Steady:
                        // Only need to update this state if there are unmet challenges.  Otherwise we allow
                        // the "Make A Challenge" message to persist.
                        // ...and, if there is only 1 unmet challenge, just keep it persistent too
                        if (m_challenges.size() > 1)
                        {
                            challenge->updateTimeInState(elapsedTime);
                            if (challenge->getTimeInState() >= components::Challenge::CHALLENGE_STEADY_TIME)
                            {
                                m_notifyRemove(m_activeChallenge->getId());
                                m_activeChallenge = nullptr;
                            }
                        }
                        break;
                }
            }
            else
            {
                cycleChallenge();
            }
        }
        else
        {
            m_notifyRemove(m_activeChallenge->getId());
            m_activeChallenge = nullptr;

            // Detect when puzzle was solved on the last remaining challenge.  When this happens,
            // Want to provide a hint to the user all challenges have been met.
            if (!m_startedWithChallenges || (m_startedWithChallenges && !anyRemainingChallenges()))
            {
                while (!m_challenges.empty())
                {
                    m_challenges.pop();
                }

                auto challenge = std::make_shared<entities::Entity>();
                challenge->addComponent(std::make_unique<components::Challenge>("All Challenges Complete"));
                m_addEntity(challenge);
            }
            else if (m_startedWithChallenges)
            {
                // If we get here, it means there are challenges remaining, so let's rebuild the queue with them
                initializeChallenges();
            }
            m_signalPuzzleComplete = false;
        }
    }

    // --------------------------------------------------------------
    //
    // Update the next challenge hint to show the player
    //
    // --------------------------------------------------------------
    void Challenge::cycleChallenge()
    {
        static const auto countChallenges = [](const Scoring::ChallengeGroup& group)
        {
            auto total = std::accumulate(group.begin(), group.end(), 0, [](auto sum, auto& y)
                                         {
                                             return sum + std::get<1>(y);
                                         });
            return total;
        };

        auto challenge = std::make_shared<entities::Entity>();
        if (!m_challenges.empty())
        {
            auto group = m_challenges.front();
            m_challenges.pop();
            auto challengeText = std::format("Try to match {} {}", Scoring::formatChallengeFriendly(group), countChallenges(group) > 1 ? "goals" : "goal");
            challenge->addComponent(std::make_unique<components::Challenge>(challengeText));
            m_challenges.push(group);
        }
        else
        {
            challenge->addComponent(std::make_unique<components::Challenge>("Discover A Challenge"));
        }
        m_addEntity(challenge);
    }

    // --------------------------------------------------------------
    //
    // This builds the list of all unmet challenges.  They are placed
    // into a queue so they can be cycled through while the user is
    // playing the level.
    //
    // --------------------------------------------------------------
    void Challenge::initializeChallenges()
    {
        while (!m_challenges.empty())
        {
            m_challenges.pop();
        }
        for (auto challenge : m_level->getChallenges()) // yes, yes, ranges filter, I know
        {
            if (!Scoring::instance().isChallengeMet(m_level->getUUID(), challenge))
            {
                m_challenges.push(challenge);
            }
        }
    }

    // --------------------------------------------------------------
    //
    // Checks to see if there are any unmet challenges
    //
    // --------------------------------------------------------------
    bool Challenge::anyRemainingChallenges()
    {
        bool anyRemaining = false;

        for (auto challenge : m_level->getChallenges())
        {
            if (!Scoring::instance().isChallengeMet(m_level->getUUID(), challenge))
            {
                anyRemaining = true;
                break;
            }
        }

        return anyRemaining;
    }
} // namespace systems
