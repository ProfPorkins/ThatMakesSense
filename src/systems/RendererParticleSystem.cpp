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

#include "RendererParticleSystem.hpp"

namespace systems
{
    // --------------------------------------------------------------
    //
    // This class is a friend to the renderers::ParticleSystem so that
    // it has access to private data, without having to expose it
    // to everyone else.
    //
    // --------------------------------------------------------------
    void RendererParticleSystem::update(systems::ParticleSystem& ps, sf::RenderTarget& renderTarget)
    {
        math::Point2f position{};
        for (decltype(ps.m_particleCount) p = 0; p < ps.m_particleCount; p++)
        {
            auto& particle = ps.m_inUse[p];

            // We compute the position here, rather than doing an update of the position in the system,
            // because the camera zoom can change and when that happens, we need to ensure the position
            // is recomputed based on the new zoom state
            position.x = particle->origin.x + particle->alive.count() * particle->speed * particle->direction.x;
            position.y = particle->origin.y + particle->alive.count() * particle->speed * particle->direction.y;
            particle->sprite.setPosition(position);

            particle->sprite.setRotation(particle->rotation);

            particle->sprite.setTextureRect(particle->getCurrentSpriteRect());

            renderTarget.draw(particle->sprite);
        }
    }

} // namespace systems
