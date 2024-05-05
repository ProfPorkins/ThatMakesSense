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

#include <filesystem>
#include <fstream>
#include <memory>
#include <string>
#include <vector>

class Levels
{
  public:
    bool load(std::filesystem::path filePath);

    auto size() const { return m_levels.size(); }
    auto get(std::uint8_t level) const { return m_levels[level]; }
    std::shared_ptr<Level> get(std::string name) const;
    std::shared_ptr<Level> getNextLevel(const std::string& uuid) const;

  private:
    std::vector<std::shared_ptr<Level>> m_levels;

    bool readLevel(std::ifstream& isLevels, std::string name, std::string& fileString);
    void createSimpleLevel();
};
