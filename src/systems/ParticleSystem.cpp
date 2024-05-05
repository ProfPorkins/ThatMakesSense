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

#include "ParticleSystem.hpp"

#include "effects/ParticleEffect.hpp"
#include "services/Configuration.hpp"
#include "services/ThreadPool.hpp"

namespace systems
{
    ParticleSystem::ParticleSystem() :
        m_panDiff({ 0, 0 })
    {
        preAllocateParticles();
    }

    // --------------------------------------------------------------
    //
    // Two simple things need to be done here...
    //   1.  Update the state of all active particles
    //   2.  Update the active effects, generating new particles
    //
    // --------------------------------------------------------------
    void ParticleSystem::update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera)
    {
        // auto graph = ThreadPool::instance().createTaskGraph();
        // auto taskUpdateParticles = ThreadPool::instance().createTask(
        //     graph,
        //     [this, elapsedTime]()
        //     {
        //         this->updateParticles(elapsedTime);
        //     });
        // auto taskUpdateEffects = ThreadPool::instance().createTask(
        //     graph,
        //     [this, elapsedTime]()
        //     {
        //         this->updateEffects(elapsedTime);
        //     });
        // graph->declarePredecessor(taskUpdateParticles->getId(), taskUpdateEffects->getId());
        // ThreadPool::instance().submitTaskGraph(graph);
        this->updateParticles(elapsedTime);
        this->updateEffects(elapsedTime, camera);
    }

    // --------------------------------------------------------------
    //
    // This is called whenever the camera pans.  When that occurs
    // all particle origins need to be updated by the difference
    // provided in the parameter.
    //
    // --------------------------------------------------------------
    void ParticleSystem::signalCameraPan(math::Vector2f diff)
    {
        auto current = m_panDiff.load();
        m_panDiff = { current.x + diff.x, current.y + diff.y };
    }

    // --------------------------------------------------------------
    //
    // Yes, I know it is terrible I'm not zooming the particles.  For
    // now, it is a bigger problem to solve than I want to.  Perhaps
    // in the future I'll come back and solve it.
    //
    // --------------------------------------------------------------
    void ParticleSystem::signalCameraZoom()
    {
        for (decltype(ParticleSystem::m_particleCount) p = 0; p < m_particleCount; p++)
        {
            m_available.push(std::move(m_inUse[p]));
        }
        m_particleCount = 0;
    }

    // --------------------------------------------------------------
    //
    // Work through the particles, updating them and discarding those
    // whose lifetime has expired.
    //
    // --------------------------------------------------------------
    void ParticleSystem::updateParticles(const std::chrono::microseconds& elapsedTime)
    {
        // The reason for doing this is that the pan can get updated at the same time,
        // because the particles update happens (potentially) in parallel with the
        // camera update.
        auto currentPan = m_panDiff.load();
        m_panDiff = { 0, 0 };

        //
        // Step 1: Update all existing particles
        decltype(ParticleSystem::m_particleCount) keeperCount = 0;
        for (decltype(ParticleSystem::m_particleCount) p = 0; p < m_particleCount; p++)
        {
            auto particle = std::move(m_inUse[p]);
            particle->alive += elapsedTime;
            //
            // Check to see if it is alive before going to the trouble of doing a full update on the particle
            if (particle->alive < particle->lifetime)
            {
                // Update origin, based on camera movement
                particle->origin.x += currentPan.x;
                particle->origin.y += currentPan.y;

                // Update size
                if (particle->sizeStart != particle->sizeEnd)
                {
                    particle->size = std::lerp(particle->sizeStart, particle->sizeEnd, static_cast<float>(particle->alive.count()) / static_cast<float>(particle->lifetime.count()));
                    particle->sprite.setScale(math::getViewScale({ particle->size * static_cast<float>(particle->spriteCount), particle->size * 1.0f }, particle->sprite.getTexture()));
                }

                // Update rotation
                particle->rotation += particle->rotationRate * elapsedTime.count();

                // Update alpha transparency
                particle->alpha = std::lerp(particle->alphaStart, particle->alphaEnd, static_cast<float>(particle->alive.count()) / static_cast<float>(particle->lifetime.count()));
                particle->sprite.setColor(sf::Color(255, 255, 255, static_cast<sf::Uint8>(particle->alpha * 255.0f)));

                // Update sprite animation
                particle->updateElapsedTime(elapsedTime);

                // TODO: Work up computing the sprite image based on the elapsed
                //       time to eliminate this conditional, and also prevent a problem
                //       where the framerate doesn't keep up.
                if (particle->elapsedTime >= particle->spriteTime)
                {
                    particle->incrementSprite();
                    particle->updateElapsedTime(-particle->spriteTime);
                }

                // Place it back to the in use set of particles
                m_inUse[keeperCount++] = std::move(particle);
            }
            else
            {
                // Send it back to the available queue
                m_available.push(std::move(particle));
            }
        }
        m_particleCount = keeperCount;
    }

    // --------------------------------------------------------------
    //
    // Go through each of the active effects, giving them a chance
    // to generate new particles.
    //
    // --------------------------------------------------------------
    void ParticleSystem::updateEffects(const std::chrono::microseconds& elapsedTime, const entities::EntityPtr& camera)
    {
        //
        // Step 2: Update active effects
        auto effectCount = m_effects.size();
        while (effectCount-- > 0)
        {
            auto effect = std::move(m_effects.front());
            m_effects.pop();
            effect->update(
                elapsedTime,
                camera,
                [this]()
                {
                    return getAvailableParticle();
                },
                [this](std::unique_ptr<Particle> p)
                {
                    addParticle(std::move(p));
                });
            // Put it back in the queue if its lifetime hasn't expired
            if (effect->getAlive() < effect->getLifetime())
            {
                m_effects.push(std::move(effect));
            }
        }
    }

    std::unique_ptr<Particle> ParticleSystem::getAvailableParticle()
    {
        if (!m_available.empty())
        {
            // Yuck!
            auto p = std::move(m_available.front());
            m_available.pop();
            return p;
        }
        return nullptr; // Could return an optional, but I'm fine with nullptr for this.
    }

    void ParticleSystem::preAllocateParticles()
    {
        while (m_available.size() < MAX_PARTICLES)
        {
            m_available.push(std::make_unique<Particle>());
        }
    }

    void ParticleSystem::addParticle(std::unique_ptr<Particle> p)
    {
        m_inUse[m_particleCount++] = std::move(p);
    }

} // namespace systems