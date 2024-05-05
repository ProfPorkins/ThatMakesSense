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

#include <cstdint>
#include <functional>
#include <optional>
#include <rapidjson/document.h>
#include <string>
#include <string_view>
#include <tuple>
#include <vector>

// Forward declaration, have to do this to prevent include dependency cycle
class Level;

// --------------------------------------------------------------
//
// Game/Level scoring service
//
// Note: This is a Singleton
//
// --------------------------------------------------------------
class Scoring
{
  public:
    using ChallengeItem = std::tuple<components::ObjectType, std::uint16_t>;
    using ChallengeGroup = std::vector<ChallengeItem>;
    using LevelChallenges = std::vector<ChallengeGroup>;

    Scoring(const Scoring&) = delete;
    Scoring(Scoring&&) = delete;
    Scoring& operator=(const Scoring&) = delete;
    Scoring& operator=(Scoring&&) = delete;

    static Scoring& instance();

    bool initialize(std::string_view configFilename);
    bool hasAnyScore();
    void recordScore(const std::shared_ptr<Level> level, const ChallengeGroup& score, std::function<void()> onComplete = nullptr);
    LevelChallenges getLevelChallenges(const std::string& uuid);
    std::optional<ChallengeGroup> getNextChallenge(const Scoring::LevelChallenges& challenges, const std::string& uuid);
    bool isChallengeMet(const std::string& uuid, const Scoring::ChallengeGroup& challenge);
    static std::string formatChallengeFriendly(const ChallengeGroup& challenge);
    static std::string formatChallengesJSON(const LevelChallenges& challenges);
    static LevelChallenges parseChallenges(std::string challengeString);

  private:
    Scoring() {}

    std::string serialize();
    void persist();
    void cleanPreRelease();

    std::string m_configFilename;
    bool m_initialized{ false };
    rapidjson::Document m_domScores;
};
