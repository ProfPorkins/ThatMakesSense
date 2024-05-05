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

#include "systems/Particle.hpp"

#include <chrono>
#include <functional>
#include <memory>

namespace systems
{
    // --------------------------------------------------------------
    //
    // A particle effect is the thing that generates particles.  The particle
    // system accepts effects and then calls on them to generate particles
    // for as long as they are alive.
    //
    // --------------------------------------------------------------
    class ParticleEffect
    {
      public:
        enum class Effect
        {
            None,
            LevelComplete,
            RuleChanged,
            EntityBurn,
            EntitySink
        };
        ParticleEffect() :
            m_lifetime(0),
            m_alive(0)
        {
        }
        ParticleEffect(std::chrono::microseconds lifetime) :
            m_lifetime(lifetime),
            m_alive(0)
        {
        }

        virtual ~ParticleEffect() = default;

        virtual void update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera = nullptr, std::function<std::unique_ptr<Particle>()> getParticle = nullptr, std::function<void(std::unique_ptr<Particle>)> addParticle = nullptr);

        auto getLifetime() { return m_lifetime; }
        auto getAlive() { return m_alive; }

      private:
        std::chrono::microseconds m_lifetime;
        std::chrono::microseconds m_alive;
    };
} // namespace systems
