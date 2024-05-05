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
#include "ParticleEffect.hpp"
#include "misc/HexCoord.hpp"

#include <memory>
#include <queue>
#include <random>

namespace systems
{
    // --------------------------------------------------------------
    //
    // Used to highlight the new "I" entities
    //
    // --------------------------------------------------------------
    class LevelCompletedEffect : public ParticleEffect
    {
      public:
        enum class Type
        {
            PreDefined,
            BeyondCategory
        };
        LevelCompletedEffect(std::shared_ptr<Level> level, Type type);

        virtual void update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle) override;

      private:
        std::shared_ptr<Level> m_level;
        std::chrono::microseconds m_nextShowerDelta;
        std::chrono::microseconds m_countdown{ 0 };
        std::queue<misc::HexCoord> m_positions;
        std::shared_ptr<sf::Texture> m_texture;
        std::uint8_t m_spriteCount;
        std::chrono::microseconds m_spriteTime;

        // Random number stuff
        std::mt19937 m_generator;
        std::normal_distribution<float> m_distNormal;
        std::uniform_real_distribution<float> m_distCircle;

        void emitShower(const entities::EntityPtr& camera, misc::HexCoord cell, std::uint16_t howMany, std::function<std::unique_ptr<Particle>()>& getParticle, std::function<void(std::unique_ptr<Particle>)>& addParticle);
    };
} // namespace systems
