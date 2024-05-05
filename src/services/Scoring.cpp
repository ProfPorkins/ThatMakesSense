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

#include "Scoring.hpp"

#include "Level.hpp"
#include "Levels.hpp"
#include "entities/Factory.hpp"
#include "misc/misc.hpp"
#include "services/Content.hpp"
#include "services/ThreadPool.hpp"
#include "services/concurrency/Task.hpp"

#include <algorithm>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>
#include <numeric>
#include <ranges>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <regex>
#include <set>
#include <sstream>
#include <vector>

// -----------------------------------------------------------------
//
// Using the Meyer's Singleton technique...this is thread safe
//
// -----------------------------------------------------------------
Scoring& Scoring::instance()
{
    static Scoring instance;
    return instance;
}

// --------------------------------------------------------------
//
// Having this service manage the scores configuration file itself
// so that it can save scoring data as it is received, rather than
// waiting until the program exits to do so.  This way if the program
// ends non-gracefully, the scoring info is persisted regardless.
//
// --------------------------------------------------------------
bool Scoring::initialize(std::string_view configFilename)
{
    m_configFilename = configFilename;

    // If the file doesn't exist, create a default one
    if (!std::filesystem::exists(m_configFilename))
    {
        std::ofstream ofFile(m_configFilename);
        ofFile << "{}";
        ofFile.close();
    }

    // Reference: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    // Using Jerry's answer, because it was benchmarked to be quite fast, even though the config files are small.
    std::ifstream inScores(m_configFilename);
    std::stringstream jsonScores;
    jsonScores << inScores.rdbuf();
    inScores.close();

    m_domScores.Parse(jsonScores.str().data());

    cleanPreRelease();

    // We need to know if there was a parse error, because later on, if things
    // aren't initialized, we'll silently ignore future requests.
    m_initialized = !m_domScores.HasParseError();

    return m_initialized;
}

// --------------------------------------------------------------
//
// Returns true if any score for any level has been recorded.  The
// purpose of this is to allow the game code to do something different
// than normal when no scores have yet been recorded.
//
// --------------------------------------------------------------
bool Scoring::hasAnyScore()
{

    return m_domScores.MemberCount() > 0;
}

// --------------------------------------------------------------
//
// Update which challenges have been met.
//
// --------------------------------------------------------------
void Scoring::recordScore(std::shared_ptr<Level> level, const ChallengeGroup& score, std::function<void()> onComplete)
{
    // This is used to compare challenges so they can be ordered for
    // determining duplicates...so they can be removed.
    static const auto lessChallengeGroup = [](auto& a, auto& b)
    {
        if (a.size() < b.size())
        {
            return true;
        }
        else if (a.size() > b.size())
        {
            return false;
        }

        for (std::size_t i = 0; i < a.size(); i++)
        {
            if (std::get<0>(a[i]) < std::get<0>(b[i]))
            {
                return true;
            }
            else if (std::get<0>(a[i]) == std::get<0>(b[i]) && std::get<1>(a[i]) < std::get<1>(b[i]))
            {
                return true;
            }
        }

        return false;
    };

    auto work = [this, level, score]()
    {
        // Create/Get the level entry in the DOM
        rapidjson::Value levelKey(rapidjson::kStringType);
        levelKey.SetString(level->getUUID().c_str(), static_cast<rapidjson::SizeType>(level->getUUID().size()), m_domScores.GetAllocator());
        if (!m_domScores.HasMember(level->getUUID().c_str()))
        {
            m_domScores.AddMember(levelKey, rapidjson::Value(rapidjson::kStringType), m_domScores.GetAllocator());
        }
        auto levelData = m_domScores.FindMember(level->getUUID().c_str());
        auto challenges = parseChallenges(levelData->value.GetString());
        challenges.push_back(score);

        // Remove duplicates
        std::sort(challenges.begin(), challenges.end(), lessChallengeGroup);
        auto last = std::unique(challenges.begin(), challenges.end());
        challenges.erase(last, challenges.end());

        auto formatted = formatChallengesJSON(challenges);
        rapidjson::Value jsonScore(rapidjson::kStringType);
        jsonScore.SetString(formatted.c_str(), static_cast<rapidjson::SizeType>(formatted.size()), m_domScores.GetAllocator());
        levelData->value = jsonScore;

        // Finally, go ahead and update the configuration file right now
        persist();
    };

    auto task = ThreadPool::instance().createIOTask(work, onComplete);
    ThreadPool::instance().enqueueTask(task);
}

// --------------------------------------------------------------
//
// Returns the scoring detail for the specified level.  If there is
// no entry, then an empty container is returned.
//
// --------------------------------------------------------------
Scoring::LevelChallenges Scoring::getLevelChallenges(const std::string& uuid)
{
    LevelChallenges detail;

    if (m_domScores.HasMember(uuid.c_str()))
    {
        auto levelData = m_domScores.FindMember(uuid.c_str());
        detail = parseChallenges(levelData->value.GetString());
    }

    return detail;
}

// --------------------------------------------------------------
//
// Returns the next challenge for the level, that has not yet been
// completed.
//
// --------------------------------------------------------------
std::optional<Scoring::ChallengeGroup> Scoring::getNextChallenge(const Scoring::LevelChallenges& challenges, const std::string& uuid)
{
    static const auto inChallenges = [](const ChallengeGroup& challenge, const LevelChallenges& challenges)
    {
        for (auto&& compare : challenges)
        {
            if (compare == challenge)
            {
                return true;
            }
        }
        return false;
    };

    // Let's get what has already been met
    auto recorded = getLevelChallenges(uuid);

    // Go through the identified challenges and return the first one not in the recorded challenges
    for (auto&& possible : challenges)
    {
        if (!inChallenges(possible, recorded))
        {
            return possible;
        }
    }

    return std::nullopt;
}

// --------------------------------------------------------------
//
// Returns true if the challenge has been completed, false otherwise.
//
// --------------------------------------------------------------
bool Scoring::isChallengeMet(const std::string& uuid, const Scoring::ChallengeGroup& challenge)
{
    for (auto&& completed : getLevelChallenges(uuid))
    {
        if (completed == challenge)
        {
            return true;
        }
    }

    return false;
}

// --------------------------------------------------------------
//
// Formats a challenge group into a human friendly format, intended
// for display to the user.
//
// --------------------------------------------------------------
std::string Scoring::formatChallengeFriendly(const ChallengeGroup& challenge)
{
    std::string message = "";

    for (bool first = true; auto&& [type, howMany] : challenge)
    {
        auto entityName = components::ObjectTypeToString.at(type);
        message += std::format("{0}{1} {2}", first ? "" : " & ", howMany, entityName);
        first = false;
    }

    return message;
}

// --------------------------------------------------------------
//
// Takes the score result from a level and formats it for serialization
// to the scoring JSON file.  This is also the same format used
// to identify the level challenges in the level files.
//
// --------------------------------------------------------------
std::string Scoring::formatChallengesJSON(const LevelChallenges& challenges)
{
    std::string result = "";

    for (auto&& group : challenges)
    {
        result += "[";
        for (bool first = true; auto&& [type, howMany] : group)
        {
            result += std::format("{0}{1}:{2}",
                                  first ? "" : ", ",
                                  howMany,
                                  static_cast<std::uint8_t>(entities::ObjectTypeToEntityCode.at(type)));
            first = false;
        }

        result += "] ";
    }

    return result;
}

// --------------------------------------------------------------
//
// Parses the "challenges" for the level into the logical data
// structure for easier use.
//
// Tool used for developing the regex: https://regex101.com/
// SO Reference for specifics on the regex: https://stackoverflow.com/questions/6208367/regex-to-match-stuff-between-parentheses
//
// --------------------------------------------------------------
Scoring::LevelChallenges Scoring::parseChallenges(std::string challengeString)
{
    Scoring::LevelChallenges challenges;

    try
    {
        std::regex regexBracket("\\[([^\\]]+)\\]");

        auto itr = std::sregex_iterator(challengeString.begin(), challengeString.end(), regexBracket);
        for (; itr != std::sregex_iterator(); itr++)
        {
            std::string challengeGroup = (*itr).str();
            misc::trim(challengeGroup);
            if (!challengeGroup.empty())
            {
                // Remove the open/close brackets
                challengeGroup.erase(0, 1);
                challengeGroup.erase(challengeGroup.size() - 1, 1);

                Scoring::ChallengeGroup group;
                // Now, extract challenge items
                for (auto&& item : misc::split(challengeGroup, ','))
                {
                    // Finally, convert these numbers into the in-code representations of the data
                    auto parts = misc::split(item, ':');
                    // First part is how many
                    std::uint16_t howMany = static_cast<std::uint16_t>(std::stoi(parts[0]));
                    // Second part is the entity code (from the factory)
                    auto entityCode = static_cast<entities::EntityCode>(std::stoi(parts[1]));
                    auto objectType = entities::EntityCodeToObjectType.at(entityCode);

                    group.push_back({ objectType, howMany });
                }

                challenges.push_back(group);
            }
        }
    }
    catch (std::exception&)
    {
        std::cout << "\tIncorrectly formed challenges\n";
    }

    return challenges;
}

// --------------------------------------------------------------
//
// Serialize the scoring info to a std::string for persistence.
//
// --------------------------------------------------------------
std::string Scoring::serialize()
{
    //
    // Use pretty writing to make a nicely formatted string for serialization (human readable)
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>> writer(buffer);
    m_domScores.Accept(writer);

    return buffer.GetString();
}

// --------------------------------------------------------------
//
// Save the current in-memory DOM to the scoring configuraton file
//
// --------------------------------------------------------------
void Scoring::persist()
{
    auto json = serialize();

    std::ofstream ofFile(m_configFilename);
    ofFile << json;
    ofFile.close();
}

// --------------------------------------------------------------
//
// This is done to clear out any scoring leftover from playing the
// game during early access and before achievements were enabled.
//
// --------------------------------------------------------------
void Scoring::cleanPreRelease()
{
    // Step 1: Collect a set of all known level UUIDs
    std::set<std::string> knownUUIDs;
    for (std::uint8_t level = 0; level < Content::getLevels().size(); level++)
    {
        auto uuid = Content::getLevels().get(level)->getUUID();
        knownUUIDs.insert(uuid);
    }
    // Step 2: Collect the set of UUIDs that have scores recorded
    std::set<std::string> scoreUUIDs;
    for (auto& score : m_domScores.GetObject())
    {
        scoreUUIDs.insert(score.name.GetString());
    }
    // Step 2: Remove any score entries that are not in the set of known UUIDs
    bool anyRemoved{ false };
    for (auto& score : scoreUUIDs)
    {
        if (!knownUUIDs.contains(score))
        {
            m_domScores.RemoveMember(score.c_str());
            anyRemoved = true;
        }
    }
    if (anyRemoved)
    {
        persist();
    }
}
