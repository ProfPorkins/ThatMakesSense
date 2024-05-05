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

#include "Levels.hpp"
#include "services/concurrency/ConcurrentQueue.hpp"

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Texture.hpp>
#include <atomic>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <unordered_map>

// --------------------------------------------------------------
//
// Used to hold content used throughout the game.  Things like
// fonts, images, sound, etc.
//
// Note: This is a Singleton
//
// --------------------------------------------------------------
class Content
{
  public:
    Content(const Content&) = delete;
    Content(Content&&) = delete;
    Content& operator=(const Content&) = delete;
    Content& operator=(Content&&) = delete;

    static Content& instance();
    void terminate();

    // This is a template so it is possible to specialize based on type, because there
    // isn't a parameter on which to overload.
    template <typename T>
    static void load(std::string key, std::string filename, std::function<void(std::string)> onComplete = nullptr, std::function<void(std::string)> onError = nullptr);

    // This is a template with specializations, because can't overload on return type
    template <typename T>
    static std::shared_ptr<T> get(std::string key);

    static const Levels& getLevels() { return instance().m_levels; }

    template <typename T>
    static bool has(std::string key);

    bool anyPending() { return m_tasksRemaining > 0; }
    bool isError() { return m_contentError; }

  private:
    Content() {}

    class LoadParams
    {
      public:
        std::string key;
        std::string filename;
        std::function<void(std::string)> onComplete;
        std::function<void(std::string)> onError;
    };

    Levels m_levels;
    std::unordered_map<std::string, std::shared_ptr<sf::Font>> m_fonts;
    std::unordered_map<std::string, std::shared_ptr<sf::Font>> m_fontsByFile;
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> m_textures;
    std::unordered_map<std::string, std::shared_ptr<sf::Texture>> m_texturesByFile;
    std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> m_audio;
    std::unordered_map<std::string, std::shared_ptr<sf::Music>> m_music;
    std::unordered_map<std::string, std::shared_ptr<sf::Sound>> m_sound;

    std::atomic_bool m_contentError{ false };
    std::atomic_uint16_t m_tasksRemaining{ 0 };

    template <typename T>
    static bool loadImpl(LoadParams& params);

    void loadComplete(bool success, LoadParams& task);
};
