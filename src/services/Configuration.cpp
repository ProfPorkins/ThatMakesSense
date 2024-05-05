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

#include "Configuration.hpp"

#include "misc/misc.hpp"
#include "services/ConfigurationPath.hpp"

#include <SFML/Graphics/Color.hpp>
#include <chrono>
#include <cstdint>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>

// -----------------------------------------------------------------
//
// Using the Meyer's Singleton technique...this is thread safe
//
// -----------------------------------------------------------------
Configuration& Configuration::instance()
{
    static Configuration instance;
    return instance;
}

// --------------------------------------------------------------
//
// Why two configuration files and why can the developer one not
// exist?
// For end users, I want an ability that allows various settings
// to be modified, like controls, resolution, etc.  But for the
// parameterized game-play data, I don't want the user to be able
// to modify them.  But, during development, I want it to be easy
// to change the game-play data, especially without having to recompile
// any code.  Therefore, the developer json, if it exists on the
// file system, will be used, but if it doesn't, then the embedded
// data is used.  Someone could figure this out and create their
// own develop json file and get around it, but I'll live with it
// because that means the game is massively popular and someone
// wanted to hack it!
//
// Online tool to convert JSON to a C++ string: https://tools.knowledgewalls.com/jsontostring
//
// --------------------------------------------------------------
bool Configuration::initialize(const std::string_view jsonSettings, const std::string_view jsonDeveloper)
{
    std::string jsonFull;
    // If running with a release json file, have to merge in the hard-coded settings
    if (jsonDeveloper.length() == 0)
    {
        // When updating remember:
        //  1.  Remove the leading {
        //  2.  Add a leading ,
        static const std::string jsonGame = ",}";
        std::string_view json1 = jsonSettings.substr(0, jsonSettings.size() - 2);
        jsonFull = std::string(json1) + jsonGame;
    }
    else
    {
        std::string_view json1 = jsonSettings.substr(0, jsonSettings.size() - 2);
        std::string_view json2 = jsonDeveloper.substr(1, jsonDeveloper.size() - 1);
        jsonFull = std::string(json1) + ", " + std::string(json2);
    }

    m_domFull.Parse(jsonFull.data());
    m_domSettings.Parse(jsonSettings.data());
    bool validParse = !m_domFull.HasParseError() && !m_domSettings.HasParseError();

    //
    // Have to call this because the resolution and scale-to-resolution settings are needed in
    // order to set this correctly.
    m_graphics.m_uiScaled = false;
    m_graphics.updateScale();

    return validParse;
}

// --------------------------------------------------------------
//
// Believe me, I know, a terrible thing to do!
//
// If I know this is such a terrible thing to do, why did I do it?
// It is because I wrote the Configuration long before I decided
// to make the program dynamically adjust to resolution changes during
// runtime.  In order to make it work "cleanly", it was expedient
// to add this reset to ensure the Graphics object starts out in
// a good, known state.
//
// --------------------------------------------------------------
void Configuration::reset()
{
    instance().m_graphics = Graphics{};
    instance().m_domFull = rapidjson::Document{};
    instance().m_domSettings = rapidjson::Document{};
}

// --------------------------------------------------------------
//
// Serialize the configuration to a std::string for persistence.
//
// --------------------------------------------------------------
std::string Configuration::serialize()
{
    //
    // Use pretty writing to make a nicely formatted string for serialization (human readable)
    rapidjson::StringBuffer buffer;
    rapidjson::PrettyWriter<rapidjson::StringBuffer, rapidjson::UTF8<>> writer(buffer);
    m_domSettings.Accept(writer);

    return buffer.GetString();
}

// --------------------------------------------------------------
//
// Various template specializations for get<>ing and set<>ing values
// in the configuration.
//
// --------------------------------------------------------------
template <>
bool Configuration::get(const std::vector<std::string>& path)
{
    return misc::findJSONValue(instance().m_domFull, path)->value.GetBool();
}

template <>
std::string Configuration::get(const std::vector<std::string>& path)
{
    return misc::findJSONValue(instance().m_domFull, path)->value.GetString();
}

template <>
std::uint8_t Configuration::get(const std::vector<std::string>& path)
{
    return static_cast<std::uint8_t>(misc::findJSONValue(instance().m_domFull, path)->value.GetUint());
}

template <>
std::uint16_t Configuration::get(const std::vector<std::string>& path)
{
    return static_cast<std::uint16_t>(misc::findJSONValue(instance().m_domFull, path)->value.GetUint());
}

template <>
std::uint32_t Configuration::get(const std::vector<std::string>& path)
{
    return static_cast<std::uint32_t>(misc::findJSONValue(instance().m_domFull, path)->value.GetUint());
}

template <>
int Configuration::get(const std::vector<std::string>& path)
{
    return static_cast<int>(misc::findJSONValue(instance().m_domFull, path)->value.GetInt());
}

template <>
float Configuration::get(const std::vector<std::string>& path)
{
    return static_cast<float>(misc::findJSONValue(instance().m_domFull, path)->value.GetDouble());
}

template <>
double Configuration::get(const std::vector<std::string>& path)
{
    return static_cast<double>(misc::findJSONValue(instance().m_domFull, path)->value.GetDouble());
}

template <>
std::chrono::milliseconds Configuration::get(const std::vector<std::string>& path)
{
    auto ms = static_cast<std::uint32_t>(misc::findJSONValue(instance().m_domFull, path)->value.GetUint());
    return std::chrono::milliseconds(ms);
}

template <>
sf::Color Configuration::get(const std::vector<std::string>& path)
{
    std::string colorString = misc::findJSONValue(instance().m_domFull, path)->value.GetString();
    auto items = misc::split(colorString, ',');
    auto red = static_cast<std::uint8_t>(std::stoi(items[0]));
    auto green = static_cast<std::uint8_t>(std::stoi(items[1]));
    auto blue = static_cast<std::uint8_t>(std::stoi(items[2]));
    std::uint8_t alpha{ 255 };
    if (items.size() == 4)
    {
        alpha = static_cast<std::uint8_t>(std::stoi(items[3]));
    }

    return sf::Color(red, green, blue, alpha);
}

template <>
void Configuration::set(const std::vector<std::string>& path, std::string value)
{
    // Modify values in both, because when serialization takes place, want the
    // changed values to persist.
    misc::findJSONValue(instance().m_domFull, path)->value.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.size()), instance().m_domFull.GetAllocator());
    misc::findJSONValue(instance().m_domSettings, path)->value.SetString(value.c_str(), static_cast<rapidjson::SizeType>(value.size()), instance().m_domSettings.GetAllocator());
}

template <>
void Configuration::set(const std::vector<std::string>& path, std::uint16_t value)
{
    misc::findJSONValue(instance().m_domFull, path)->value.SetUint(value);
    misc::findJSONValue(instance().m_domSettings, path)->value.SetUint(value);
}

template <>
void Configuration::set(const std::vector<std::string>& path, std::uint8_t value)
{
    misc::findJSONValue(instance().m_domFull, path)->value.SetUint(value);
    misc::findJSONValue(instance().m_domSettings, path)->value.SetUint(value);
}

template <>
void Configuration::set(const std::vector<std::string>& path, bool value)
{
    misc::findJSONValue(instance().m_domFull, path)->value.SetBool(value);
    misc::findJSONValue(instance().m_domSettings, path)->value.SetBool(value);
}

template <>
void Configuration::set(const std::vector<std::string>& path, float value)
{
    misc::findJSONValue(instance().m_domFull, path)->value.SetDouble(value);
    misc::findJSONValue(instance().m_domSettings, path)->value.SetDouble(value);
}

// --------------------------------------------------------------
//
// When either the resolution or scaling option changes, need to update
// the SFML scaling factor in order to render correctly.
//
// --------------------------------------------------------------
void Configuration::Graphics::updateScale()
{
    if (!m_uiScaled)
    {
        auto aspectRatio = Configuration::get<float>(config::GRAPHICS_WIDTH) / Configuration::getGraphics().getResolution().height;
        if (aspectRatio < 1.0)
        {
            m_scaleUI.y *= aspectRatio;
        }
        m_uiScaled = true;
    }
}
