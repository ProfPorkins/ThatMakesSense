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

#include <chrono>
#include <rapidjson/document.h>
#include <string>
#include <vector>

// --------------------------------------------------------------
//
// Right now just a grab bag of misc code that doesn't belong
// anywhere else.
//
// --------------------------------------------------------------
namespace misc
{
    using namespace std::string_literals;

    static const auto GAME_NAME = "That Makes Sense"s;

    static constexpr float PER_MS_TO_US = static_cast<float>(1.0f / std::chrono::duration_cast<std::chrono::microseconds>(std::chrono::milliseconds(1)).count());

    constexpr std::chrono::microseconds msTous(std::chrono::microseconds ms)
    {
        return std::chrono::duration_cast<std::chrono::microseconds>(ms);
    }
    void trim(std::string& str);
    std::vector<std::string> split(const std::string& s, char delimiter);
    // No kidding, I couldn't use 'auto' here, because the function needs to be defined elsewhere, ugh!
    rapidjson::GenericMemberIterator<false, rapidjson::UTF8<>, rapidjson::MemoryPoolAllocator<rapidjson::CrtAllocator>> findJSONValue(rapidjson::Document& dom, const std::vector<std::string>& path);

    //
    // Reference: https://stackoverflow.com/questions/11421432/how-can-i-output-the-value-of-an-enum-class-in-c11
    //
    template <typename T>
    constexpr auto as_integer(T const value)
        -> typename std::underlying_type<T>::type
    {
        return static_cast<typename std::underlying_type<T>::type>(value);
    }

} // namespace misc