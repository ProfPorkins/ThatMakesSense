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

#include "Content.hpp"

#include "services/ThreadPool.hpp"
#include "services/concurrency//Task.hpp"

#include <filesystem>
#include <iostream>

static const std::string CONTENT_PATH{ "assets" };
static const std::string CONTENT_FONT_PATH{ "fonts" };
static const std::string CONTENT_IMAGE_PATH{ "images" };
static const std::string CONTENT_AUDIO_PATH{ "audio" };
static const std::string CONTENT_MUSIC_PATH{ "music" };
static const std::string CONTENT_LEVELS_PATH{ "levels" };

// -----------------------------------------------------------------
//
// Using the Meyer's Singleton technique...this is thread safe
//
// -----------------------------------------------------------------
Content& Content::instance()
{
    static Content instance;
    return instance;
}

// -----------------------------------------------------------------
//
// Used to gracefully let go of all the acquired resources before shutdown
//
// -----------------------------------------------------------------
void Content::terminate()
{
    m_fonts.clear();
    m_fontsByFile.clear();
    m_textures.clear();
    m_texturesByFile.clear();
    m_audio.clear();
    m_music.clear();
    m_sound.clear();
}

// --------------------------------------------------------------
//
// NOTE: These loadImpl methods must come before the template specializations for
//       the load methods.  If they aren't, the compiler tries to
//       create its own instantiations and then when it sees the specializations
//       it has a little fit.
//
// --------------------------------------------------------------

// --------------------------------------------------------------
//
// Specialization on Levels for loading the game levels
//
// --------------------------------------------------------------
template <>
bool Content::loadImpl<Levels>(LoadParams& params)
{
    std::filesystem::path path(CONTENT_PATH);
    path /= CONTENT_LEVELS_PATH;
    path /= params.filename;

    return instance().m_levels.load(path);
}

// --------------------------------------------------------------
//
// Specialization on sf::Font for loading a font
//
// --------------------------------------------------------------
template <>
bool Content::loadImpl<sf::Font>(LoadParams& params)
{
    std::filesystem::path path(CONTENT_PATH);
    path /= CONTENT_FONT_PATH;
    path /= params.filename;

    // Check to see if this filename is already loaded by filename, and if so,
    // just reuse it...this saves quite a bit of time for this particular application
    // because the same font is used over and over
    std::shared_ptr<sf::Font> font{ nullptr };
    if (!instance().m_fontsByFile.contains(params.filename))
    {
        font = std::make_shared<sf::Font>();
        if (!font->loadFromFile(path.string()))
        {
            return false;
        }
        instance().m_fontsByFile[params.filename] = font;
    }
    else
    {
        font = instance().m_fontsByFile[params.filename];
    }

    instance().m_fonts[params.key] = font;

    return true;
}

// --------------------------------------------------------------
//
// Specialization on sf::Texture for loading textures (for adding to sprites for rendering)
//
// --------------------------------------------------------------
template <>
bool Content::loadImpl<sf::Texture>(LoadParams& params)
{
    std::filesystem::path path(CONTENT_PATH);
    path /= CONTENT_IMAGE_PATH;
    path /= params.filename;

    // Check to see if this filename is already loaded by filename, and if so,
    // just reuse it...this saves quite a bit of time for this particular application
    // because there is a basic hex texture that is reused quite a bit
    std::shared_ptr<sf::Texture> image{ nullptr };
    if (!instance().m_texturesByFile.contains(params.filename))
    {
        image = std::make_shared<sf::Texture>();
        if (!image->loadFromFile(path.string()))
        {
            return false;
        }
        image->setSmooth(true);
        instance().m_texturesByFile[params.filename] = image;
    }
    else
    {
        image = instance().m_texturesByFile[params.filename];
    }

    instance().m_textures[params.key] = image;

    return true;
}

// --------------------------------------------------------------
//
// Specialization on sf::SoundBuffer for loading an audio clip
//
// --------------------------------------------------------------
template <>
bool Content::loadImpl<sf::SoundBuffer>(LoadParams& params)
{
    std::filesystem::path path(CONTENT_PATH);
    path /= CONTENT_AUDIO_PATH;
    path /= params.filename;

    auto audio = std::make_shared<sf::SoundBuffer>();
    if (!audio->loadFromFile(path.string()))
    {
        return false;
    }

    instance().m_audio[params.key] = audio;
    // Create the matching sf::Sound that can be used to directly play the sound if desired
    instance().m_sound[params.key] = std::make_shared<sf::Sound>();
    instance().m_sound[params.key]->setBuffer(*audio);

    return true;
}

// --------------------------------------------------------------
//
// Specialization on sf::Music for creating a music object
//
// --------------------------------------------------------------
template <>
bool Content::loadImpl<sf::Music>(LoadParams& params)
{
    std::filesystem::path path(CONTENT_PATH);
    path /= CONTENT_MUSIC_PATH;
    path /= params.filename;

    auto audio = std::make_shared<sf::Music>();
    if (!audio->openFromFile(path.string()))
    {
        return false;
    }

    instance().m_music[params.key] = audio;

    return true;
}

// --------------------------------------------------------------
//
// Specialization on Levels for loading the game levels
//
// --------------------------------------------------------------
template <>
void Content::load<Levels>(std::string key, std::string filename, std::function<void(std::string)> onComplete, std::function<void(std::string)> onError)
{
    auto work = [=]()
    {
        auto params = LoadParams{
            key,
            filename,
            onComplete,
            onError
        };
        bool success = loadImpl<Levels>(params);
        Content::instance().loadComplete(success, params);
    };

    Content::instance().m_tasksRemaining++;
    auto task = ThreadPool::instance().createIOTask(work);
    ThreadPool::instance().enqueueTask(task);
}

// --------------------------------------------------------------
//
// Specialization on sf::Font for loading a font
//
// --------------------------------------------------------------
template <>
void Content::load<sf::Font>(std::string key, std::string filename, std::function<void(std::string)> onComplete, std::function<void(std::string)> onError)
{
    auto work = [=]()
    {
        auto params = LoadParams{
            key,
            filename,
            onComplete,
            onError
        };
        bool success = loadImpl<sf::Font>(params);
        Content::instance().loadComplete(success, params);
    };

    Content::instance().m_tasksRemaining++;
    auto task = ThreadPool::instance().createIOTask(work);
    ThreadPool::instance().enqueueTask(task);
}

// --------------------------------------------------------------
//
// Specialization on sf::Texture for loading textures (for adding to sprites for rendering)
//
// --------------------------------------------------------------
template <>
void Content::load<sf::Texture>(std::string key, std::string filename, std::function<void(std::string)> onComplete, std::function<void(std::string)> onError)
{
    auto work = [=]()
    {
        auto params = LoadParams{
            key,
            filename,
            onComplete,
            onError
        };
        bool success = loadImpl<sf::Texture>(params);
        Content::instance().loadComplete(success, params);
    };

    Content::instance().m_tasksRemaining++;
    auto task = ThreadPool::instance().createIOTask(work);
    ThreadPool::instance().enqueueTask(task);
}

// --------------------------------------------------------------
//
// Specialization on sf::SoundBuffer for loading an audio clip
//
// --------------------------------------------------------------
template <>
void Content::load<sf::SoundBuffer>(std::string key, std::string filename, std::function<void(std::string)> onComplete, std::function<void(std::string)> onError)
{
    auto work = [=]()
    {
        auto params = LoadParams{
            key,
            filename,
            onComplete,
            onError
        };
        bool success = loadImpl<sf::SoundBuffer>(params);
        Content::instance().loadComplete(success, params);
    };

    Content::instance().m_tasksRemaining++;
    auto task = ThreadPool::instance().createIOTask(work);
    ThreadPool::instance().enqueueTask(task);
}

// --------------------------------------------------------------
//
// Specialization on sf::Music for getting a music file ready for streaming
//
// --------------------------------------------------------------
template <>
void Content::load<sf::Music>(std::string key, std::string filename, std::function<void(std::string)> onComplete, std::function<void(std::string)> onError)
{
    auto work = [=]()
    {
        auto params = LoadParams{
            key,
            filename,
            onComplete,
            onError
        };
        bool success = loadImpl<sf::Music>(params);
        Content::instance().loadComplete(success, params);
    };

    Content::instance().m_tasksRemaining++;
    auto task = ThreadPool::instance().createIOTask(work);
    ThreadPool::instance().enqueueTask(task);
}

// --------------------------------------------------------------
//
// Specialization on sf::Font for obtaining a font
//
// --------------------------------------------------------------
template <>
std::shared_ptr<sf::Font> Content::get(std::string key)
{
    return instance().m_fonts[key];
}

template <>
bool Content::has<sf::Font>(std::string key)
{
    return instance().m_fonts.find(key) != instance().m_fonts.end();
}

// --------------------------------------------------------------
//
// Specialization on sf::Image for obtaining an image
//
// --------------------------------------------------------------
template <>
std::shared_ptr<sf::Texture> Content::get(std::string key)
{
    return instance().m_textures[key];
}

template <>
bool Content::has<sf::Texture>(std::string key)
{
    return instance().m_textures.find(key) != instance().m_textures.end();
}

// --------------------------------------------------------------
//
// Specialization on sf::SoundBuffer for obtaining an audio clip
//
// --------------------------------------------------------------
template <>
std::shared_ptr<sf::SoundBuffer> Content::get(std::string key)
{
    return instance().m_audio[key];
}

template <>
std::shared_ptr<sf::Sound> Content::get(std::string key)
{
    return instance().m_sound[key];
}

// Only need has<sf::SoundBuffer>, because it is good for both sf::Sound and sf::SoundBuffer
template <>
bool Content::has<sf::SoundBuffer>(std::string key)
{
    return instance().m_audio.find(key) != instance().m_audio.end();
}

// --------------------------------------------------------------
//
// Specialization on sf::Music for obtaining a Music streaming object
//
// --------------------------------------------------------------
template <>
std::shared_ptr<sf::Music> Content::get(std::string key)
{
    return instance().m_music[key];
}

template <>
bool Content::has<sf::Music>(std::string key)
{
    return instance().m_music.find(key) != instance().m_music.end();
}

void Content::loadComplete(bool success, LoadParams& params)
{
    if (success)
    {
        std::cout << "finished loading: " << params.key << std::endl;
        m_tasksRemaining--;
    }
    else
    {
        m_contentError = true;
        std::cout << "error in loading: " << params.filename << std::endl;
        m_tasksRemaining--;
    }
    if (success && params.onComplete != nullptr)
    {
        params.onComplete(params.key);
    }
    else if (!success && params.onError != nullptr)
    {
        params.onError(params.filename);
    }
}
