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

#include "misc.hpp"

#include <algorithm>
#include <rapidjson/stringbuffer.h>
#include <sstream>

namespace misc
{
    // --------------------------------------------------------------
    //
    // Some functions for trimming whitespace from std::string
    //
    // Adapted from: https://stackoverflow.com/questions/216823/how-to-trim-a-stdstring
    //
    // --------------------------------------------------------------
    void ltrim(std::string& s)
    {
        s.erase(
            s.begin(),
            std::find_if(s.begin(),
                         s.end(),
                         [](auto c)
                         {
                             return !std::isspace(c);
                         }));
    }

    void rtrim(std::string& s)
    {
        s.erase(
            std::find_if(s.rbegin(),
                         s.rend(),
                         [](auto c)
                         {
                             return !std::isspace(c);
                         })
                .base(),
            s.end());
    }

    void trim(std::string& str)
    {
        ltrim(str);
        rtrim(str);
    }

    // --------------------------------------------------------------
    //
    // Helper method used to find the member iterator in the rapidJSON document.
    //
    // --------------------------------------------------------------
    rapidjson::GenericMemberIterator<false, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> findJSONValue(rapidjson::Document& dom, const std::vector<std::string>& path)
    {
        auto itr = path.begin();
        auto node = dom.FindMember(itr->c_str());
        while (++itr != path.end())
        {
            node = node->value.FindMember(itr->c_str());
        }

        return node;
    }

    // --------------------------------------------------------------
    //
    // Some functions for trimming whitespace from std::string
    //
    // Source: https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
    //
    // --------------------------------------------------------------
    std::vector<std::string> split(const std::string& s, char delimiter)
    {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter))
        {
            tokens.push_back(token);
        }
        return tokens;
    }
} // namespace misc
