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

#include "Audio.hpp"

#include "services//ContentKey.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

// -----------------------------------------------------------------
//
// Using the Meyer's Singleton technique...this is thread safe
//
// -----------------------------------------------------------------
Audio& Audio::instance()
{
    static Audio instance;
    return instance;
}

// --------------------------------------------------------------
//
// Call this one time at program startup.  This gets the worker
// thread up and running.
//
// --------------------------------------------------------------
void Audio::initialize()
{
    m_threadSound = std::make_unique<std::thread>(&Audio::runAudio, this);
    m_threadMusic = std::make_unique<std::thread>(&Audio::runMusic, this);
    //
    // Create a queue of 100 sf::Sound object that we'll cycle through.
    while (m_sounds.size() < 100)
    {
        m_sounds.enqueue(std::make_shared<sf::Sound>());
    }
}

// --------------------------------------------------------------
//
// Call this one time as the program is shutting down.  This gets
// the worker thread gracefully terminated.
//
// --------------------------------------------------------------
void Audio::terminate()
{
    m_doneSounds = true;
    m_doneMusic = true;

    m_eventSound.notify_one();
    m_threadSound->join();
    m_threadMusic->join();
}

// --------------------------------------------------------------
//
// Public method to allow client code to initiate a sound.
//
// --------------------------------------------------------------
void Audio::play(const std::string& key, float volume)
{
    instance().m_tasksSound.enqueue({ key, volume });
    instance().m_eventSound.notify_one();
}

// --------------------------------------------------------------
//
// Public method to allow client code to add a music key for
// background music.
//
// --------------------------------------------------------------
void Audio::addMusic(const std::string& key)
{
    instance().m_keysMusic.enqueue(key);
}

// --------------------------------------------------------------
//
// This is the worker thread for sounds.  It pulls tasks from the queue and
// completes them.  If there are no tasks, it goes into an efficient
// wait state until a new task is added.
//
// --------------------------------------------------------------
void Audio::runAudio()
{
    while (!m_doneSounds)
    {
        auto task = m_tasksSound.dequeue();
        if (task.has_value())
        {
            auto sound = m_sounds.dequeue().value();
            sound->setBuffer(*Content::get<sf::SoundBuffer>(task.value().key));
            sound->setVolume(task.value().volume);
            sound->play();
            m_sounds.enqueue(sound);
        }
        else
        {
            std::unique_lock<std::mutex> lock(m_mutexSound);
            m_eventSound.wait(lock);
        }
    }
}

// --------------------------------------------------------------
//
// This is the worker thread for music.  It pulls items from the
// queue of music keys.  As long as the player has requested background
// music, it will play music, looping through the queue.  Because
// SFML doesn't have an event that fires when a music file ends
// this thread just sleeps and wakes up every X seconds to check
// on things.
//
// --------------------------------------------------------------
void Audio::runMusic()
{
    while (!m_doneMusic)
    {
        if (Configuration::get<bool>(config::PLAY_BACKGROUND_MUSIC) && m_keysMusic.size() > 0)
        {
            // First check is to see if anything is playing, if not, then get something player
            // Second check, if something is playing, see if it is done or not and move
            // to the next song if it isn't playing.
            if (m_currentMusic == nullptr || m_currentMusic->getStatus() == sf::Music::Stopped)
            {
                // Yes, we remove whatever is at the front, and then immediately place it at the back
                // of the queue so we keep looping through the whole group of songs.
                auto key = m_keysMusic.dequeue();
                m_keysMusic.enqueue(key.value());

                m_currentMusic = Content::get<sf::Music>(key.value());
                m_currentMusic->stop();
                m_currentMusic->setVolume(15);
                m_currentMusic->setLoop(false);
                m_currentMusic->play();
            }
        }
        else if (!Configuration::get<bool>(config::PLAY_BACKGROUND_MUSIC))
        {
            if (instance().m_currentMusic != nullptr)
            {
                instance().m_currentMusic->stop();
                instance().m_currentMusic = nullptr;
            }
        }
        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}
