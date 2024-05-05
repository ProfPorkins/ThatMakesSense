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

#include "Levels.hpp"

#include "misc/misc.hpp"
#include "misc/sha512.hpp"
#include "services//Configuration.hpp"
#include "services/ConfigurationPath.hpp"

#include <algorithm>
#include <cassert>
#include <filesystem>
#include <format>
#include <fstream>
#include <iostream>

std::shared_ptr<Level> Levels::get(std::string name) const
{
    auto level = std::find_if(m_levels.begin(), m_levels.end(),
                              [&name](const auto& level)
                              {
                                  return name == level->getName();
                              });

    if (level != m_levels.end())
    {
        return *level;
    }

    return nullptr;
}

std::shared_ptr<Level> Levels::getNextLevel(const std::string& uuid) const
{
    auto level = std::find_if(m_levels.begin(), m_levels.end(),
                              [&uuid](const auto& level)
                              {
                                  return uuid == level->getUUID();
                              });

    if (level != m_levels.end())
    {
        // Have to increment the iterator to get to the next level, and still
        // test for end, because it might have been the last level.
        level++;
        if (level != m_levels.end())
        {
            return *level;
        }
    }

    return nullptr;
}

// --------------------------------------------------------------
//
// The file may contain a single level or multiple levels.  This
// method handles determine if there is a level left to read, and
// if so, hands off the reading of each level to the Level class.
//
// --------------------------------------------------------------
bool Levels::load(std::filesystem::path filePath)
{
    static const std::string EXPECTED_FILE_HASH = "9d88f9667c2425f958e7e0a6cf985742cdccb67aa206bb4311a317f0c54f46da2e5a648d7e82bee1d0aca5d51ef2999ddcbca95d4cb7d37ce83d944591ed325c";

    std::ifstream isLevels(filePath);
    std::string fileString;

    m_levels.clear();
    bool success{ true };
    bool done{ false };

    while (!done)
    {
        // First line is the in-game display name of the level
        std::string line;
        std::getline(isLevels, line);
        misc::trim(line);

        if (line.length() > 0)
        {
            fileString += line;
            std::cout << std::format("Level Name: {0}\n", line);
            if (!readLevel(isLevels, line, fileString))
            {
                success = false;
                done = true;
            }
        }
        else
        {
            done = true;
        }
    }

    if (!success)
    {
        std::cout << "Failure in loading the levels!\n";
    }

    // Check to see if we should validate the file hash.  I've also thrown in
    // checking for a read failure, this way at least something is created.
    if (!success || Configuration::get<bool>(config::DEVELOPER_VALIDATE_LEVEL_HASH))
    {
        auto computedHash = sha512(fileString);
        if (computedHash != EXPECTED_FILE_HASH)
        {
            std::cout << "Levels file failed hash validation\n";

            m_levels.clear();
            createSimpleLevel();
        }
    }

    return true; // Because of the above, we can always say success
}

// --------------------------------------------------------------
//
// This function simply reads the string data for the levels into memory
//
// --------------------------------------------------------------
std::vector<std::string> readLevelBlock(std::ifstream& isLevels, std::shared_ptr<Level> level, std::uint16_t width, std::string& fileString)
{
    std::vector<std::string> data;

    for (std::uint8_t row = 0; row < level->getHeight(); row++)
    {
        std::string line;
        std::getline(isLevels, line);
        fileString += line;
        // * 2 because two characters per entry
        assert(line.size() == width * 2);
        if (line.size() != width * 2)
        {
            std::cout << std::format("Incorrect layer line width at row {0} of {1}\n", row, line.size());
        }
        data.push_back(line);
    }

    return data;
}

// --------------------------------------------------------------
//
// This method knows how to read a level from the file, parse it
// to create a Level insteance.
//
// MUTHOR of assumptions:  No errors in the level data!!
//   * Now there is some error checking :)
//
// --------------------------------------------------------------
bool Levels::readLevel(std::ifstream& isLevels, std::string name, std::string& fileString)
{
    static const auto CAMERA_Q_INDEX = 0;
    static const auto CAMERA_R_INDEX = 1;
    static const auto CAMERA_RANGE_INDEX = 2;

    static const auto LEVEL_LAYERS_INDEX = 0;
    static const auto LEVEL_LAYER_WIDTH_INDEX = 1;
    static const auto LEVEL_LAYER_HEIGHT_INDEX = 2;

    // Second line is the level hint, if any
    std::string lineHint;
    std::getline(isLevels, lineHint);
    misc::trim(lineHint);
    fileString += lineHint;
    std::cout << std::format("\tHint: {0}\n", lineHint);

    // Third line is the UUID
    std::string lineUUID;
    std::getline(isLevels, lineUUID);
    misc::trim(lineUUID);
    fileString += lineUUID;
    std::cout << std::format("\tUUID: {0}\n", lineUUID);

    // Fourth line contains the challenges
    std::string lineChallenges;
    std::getline(isLevels, lineChallenges);
    fileString += lineChallenges;

    // Fifth line is initial camera position and range, q, r, range
    std::string lineCamera;
    std::getline(isLevels, lineCamera);
    fileString += lineCamera;
    auto items = misc::split(lineCamera, ',');
    misc::HexCoord center{};
    std::uint8_t range{};

    assert(items.size() == 3);
    if (items.size() == 3)
    {
        std::cout << std::format("\tCamera: [{0}, {1}] : {2}\n", items[CAMERA_Q_INDEX], items[CAMERA_R_INDEX], items[CAMERA_RANGE_INDEX]);
        try
        {
            center = { static_cast<misc::HexCoord::Type>(std::stoi(items[CAMERA_Q_INDEX])), static_cast<misc::HexCoord::Type>(std::stoi(items[CAMERA_R_INDEX])) };
            range = static_cast<uint8_t>(std::stoi(items[CAMERA_RANGE_INDEX]));
        }
        catch (std::exception&)
        {
            std::cout << "error in camera settings\n";
            return false;
        }
    }
    else
    {
        std::cout << "Incorrect number of camera setting parameters\n";
        return false;
    }

    //
    // Sixth line is # of layers, width, and height
    std::string lineSize;
    std::getline(isLevels, lineSize);
    fileString += lineSize;
    items = misc::split(lineSize, 'x');
    assert(items.size() == 3);
    if (items.size() == 3)
    {
        std::cout << std::format("\tLayer Count: {0}\n", items[LEVEL_LAYERS_INDEX]);
        std::cout << std::format("\tLayer Dims : [{0}, {1}]\n", items[LEVEL_LAYER_WIDTH_INDEX], items[LEVEL_LAYER_HEIGHT_INDEX]);

        try
        {
            std::uint8_t layers = static_cast<uint8_t>(std::stoi(items[LEVEL_LAYERS_INDEX]));
            std::uint16_t width = static_cast<uint16_t>(std::stoi(items[LEVEL_LAYER_WIDTH_INDEX]));
            std::uint16_t height = static_cast<uint16_t>(std::stoi(items[LEVEL_LAYER_HEIGHT_INDEX]));

            std::shared_ptr<Level> level = std::make_shared<Level>(name, lineHint, lineUUID, lineChallenges, layers, width, height, center, range);
            m_levels.push_back(level);

            //
            // Next the width x height blocks are the entity types
            for (std::uint8_t layer = 0; layer < layers; layer++)
            {
                level->m_levelData[layer] = readLevelBlock(isLevels, level, width, fileString);
            }
        }
        catch (std::exception&)
        {
            std::cout << "error in layer settings\n";
            return false;
        }
    }
    else
    {
        return false;
    }

    return true;
}

// --------------------------------------------------------------
//
// This is used to create a simple level for the game in the case
// the levels file fails validation.
//
// --------------------------------------------------------------
void Levels::createSimpleLevel()
{
    std::shared_ptr<Level> level = std::make_shared<Level>(
        "Puzzles Validation Failure",
        "",
        "2d3262eb-02a1-4b79-9447-d82ffc724532",
        "[1:6]",
        std::uint8_t(2), std::uint16_t(9), std::uint16_t(7),
        misc::HexCoord{ 4, 3 },
        std::uint8_t(8));

    std::vector<std::string> layer1 = {
        "   1 1 1 1 1 1 1  ",
        " 1 2 2 2 2 2 2 1  ",
        " 1 2 2 2 2 2 2 2 1",
        " 1 2 2 2 2 2 2 1  ",
        " 1 2 2 2 2 2 2 2 1",
        " 1 2 2 2 2 2 2 1  ",
        "   1 1 1 1 1 1 1  "
    };
    std::vector<std::string> layer2 = {
        "                  ",
        "  426073  47 6    ",
        "            6076  ",
        "                  ",
        "  4061            ",
        "   748            ",
        "                  "
    };

    level->m_levelData[0] = layer1;
    level->m_levelData[1] = layer2;

    m_levels.push_back(level);
}
