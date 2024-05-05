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

#include "services/concurrency/ConcurrentQueue.hpp"

#include <SFML/Audio/Music.hpp>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include <condition_variable>
#include <functional>
#include <memory>
#include <mutex>
#include <string>
#include <thread>
#include <vector>

// --------------------------------------------------------------
//
// This is used throughout the game to play sounds.  It creates
// the sf::Sound object that are responsible for the individual
// sounds.  Music is not handled by this class, because there
// aren't any serious issues related to playing music that need
// something more complex like this.
//
// I struggled to come up with a good name for it.  I didn't want
// to call it a "system", because there are other kinds of systems.
// I also didn't want to call it a "manager", because it seems
// like everything is called a manager.  In the end, I went with
// Audio, not great, but workable.
//
// Note: This is a Singleton
//
// --------------------------------------------------------------
class Audio
{
  public:
    Audio(const Audio&) = delete;
    Audio(Audio&&) = delete;
    Audio& operator=(const Audio&) = delete;
    Audio& operator=(Audio&&) = delete;

    static Audio& instance();
    void initialize();
    void terminate();

    static void play(const std::string& key, float volume = 100.0f);
    static void addMusic(const std::string& key);

  private:
    Audio() {}

    class Task
    {
      public:
        Task(std::string key, float volume) :
            key(key),
            volume(volume)
        {
        }

        std::string key;
        float volume;
    };

    // Storing shared_ptr because if you copy sf::Sound it stops playing, need to keep
    // the original sf::Sound object around all the time.  Could change it to unique_ptr
    // with a bunch of std::move I suppose, to improve efficiency.
    ConcurrentQueue<std::shared_ptr<sf::Sound>> m_sounds;

    bool m_doneSounds{ false };
    std::unique_ptr<std::thread> m_threadSound;
    ConcurrentQueue<Task> m_tasksSound;
    std::condition_variable m_eventSound;
    std::mutex m_mutexSound;

    // Music related items
    bool m_doneMusic{ false };
    std::unique_ptr<std::thread> m_threadMusic;
    ConcurrentQueue<std::string> m_keysMusic;
    std::shared_ptr<sf::Music> m_currentMusic{ nullptr };

    void runAudio();
    void runMusic();
};
