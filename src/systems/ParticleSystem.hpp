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

#include "Particle.hpp"
#include "effects/ParticleEffect.hpp"
#include "misc/math.hpp"

#include <SFML/Graphics.hpp>
#include <array>
#include <atomic>
#include <chrono>
#include <cstdint>
#include <memory>
#include <queue>

namespace systems
{
    // Forward declaration for the 'friend use below'
    class RendererParticleSystem;
    // --------------------------------------------------------------
    //
    // A particle system is a collection of all active particles and
    // particle effects.  An effect is code that knows how to generate
    // particles for some particlar pattern, emitting particles over time.
    // The particle system calls into all active effects to have them
    // generate particles.
    //
    // --------------------------------------------------------------
    class ParticleSystem // I know it is redundant to put System in the name, but I also need a Particle class, so there!
    {
      public:
        ParticleSystem();

        void update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera);
        void addEffect(std::unique_ptr<ParticleEffect> effect) { m_effects.push(std::move(effect)); }

        void signalCameraPan(math::Vector2f diff);
        void signalCameraZoom();

      private:
        friend systems::RendererParticleSystem;

        static const auto MAX_PARTICLES = 10'000; // NOTE: Purely arbitrary number for now, no specific reason for it.
        std::queue<std::unique_ptr<Particle>> m_available;
        std::array<std::unique_ptr<Particle>, MAX_PARTICLES> m_inUse; // This has to be a random access container for the particle system rendering to use and not have to keep removing/adding things to a queue-like container
        std::uint16_t m_particleCount{ 0 };
        std::queue<std::unique_ptr<ParticleEffect>> m_effects;
        std::atomic<math::Vector2f> m_panDiff;
        bool m_cameraZoom{ false };

        void updateParticles(const std::chrono::microseconds& elapsedTime);
        void updateEffects(const std::chrono::microseconds& elapsedTime, const entities::EntityPtr& camera);
        std::unique_ptr<Particle> getAvailableParticle();
        void preAllocateParticles();
        void addParticle(std::unique_ptr<Particle> p);
    };
} // namespace systems
