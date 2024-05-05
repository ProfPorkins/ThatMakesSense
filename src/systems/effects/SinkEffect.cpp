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

#include "SinkEffect.hpp"

#include "components/Camera.hpp"
#include "components/Position.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <cmath>
#include <numbers>

namespace systems
{
    SinkEffect::SinkEffect(misc::HexCoord position, std::uint16_t levelWidth, std::uint16_t levelHeight) :
        m_position(position),
        m_levelWidth(levelWidth),
        m_levelHeight(levelHeight),
        m_texture(Content::get<sf::Texture>(content::KEY_IMAGE_PARTICLE_SINK)),
        m_spriteCount(Configuration::get<std::uint8_t>(config::IMAGE_PARTICLE_SINK_SPRITE_COUNT)),
        m_spriteTime(misc::msTous(Configuration::get<std::chrono::milliseconds>(config::IMAGE_PARTICLE_SINK_SPRITE_TIME))),
        m_generator(std::random_device()()),
        m_distRotateRate(0.0001f, 0.000025f),
        m_distLifetimeSpeed(1, 0.25f),
        m_distJitter(1, 0.25f),
        m_distCircle(0.0f, 2.0f * std::numbers::pi_v<float>)
    {
    }

    void SinkEffect::update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle)
    {
        ParticleEffect::update(elapsedTime);

        emitShower(camera, m_position, 100, getParticle, addParticle);
    }

    void SinkEffect::setParticleParameters(std::unique_ptr<systems::Particle>& p, misc::RenderingDetails& details, const math::Point2f& center, const float& angle, const float& distScale)
    {
        p->lifetime = std::chrono::microseconds(static_cast<long>(1500000 * m_distLifetimeSpeed(m_generator)));
        p->alive = std::chrono::microseconds::zero();
        p->currentSprite = 0;
        p->elapsedTime = std::chrono::microseconds::zero();
        p->spriteCount = m_spriteCount;
        p->spriteTime = p->lifetime / m_spriteCount;

        // Particle size is scaled based on the camera zoom; using the cell rendering size as a proxy for this
        p->size = p->sizeStart = details.renderDimX * 0.1f;
        p->sizeEnd = details.renderDimX * 0.1f;

        // Use the angle to find the x,y circle location
        p->origin.x = center.x + std::cos(angle) * distScale;
        p->origin.y = center.y + std::sin(angle) * distScale;

        // Direction is outward from the center of the location
        p->direction.x = p->origin.x - center.x;
        p->direction.y = p->origin.y - center.y;
        p->rotationRate = m_distRotateRate(m_generator);

        // Speed is scaled based on the camera zoom; using the cell rendering size as a proxy for this
        p->speed = details.renderDimX * 0.00000025f * m_distLifetimeSpeed(m_generator);
        p->sprite.setTexture(*m_texture);
        auto textureSizeX = p->sprite.getTexture()->getSize().x / static_cast<float>(p->spriteCount);
        p->sprite.setOrigin(
            textureSizeX / 2.0f,
            p->sprite.getTexture()->getSize().y / 2.0f);
        p->sprite.setScale(math::getViewScale({ p->size * static_cast<float>(p->spriteCount), p->size * 1.0f }, p->sprite.getTexture()));
    }

    void SinkEffect::emitShower(const entities::EntityPtr& camera, misc::HexCoord cell, std::uint16_t howMany, std::function<std::unique_ptr<Particle>()>& getParticle, std::function<void(std::unique_ptr<Particle>)>& addParticle)
    {
        auto details = misc::computeRenderingDetails(camera, m_levelWidth, m_levelHeight);

        // Only create particles if the cell is currently visible
        if (cell.q >= details.startQ && cell.q <= details.endQ && cell.r >= details.startR && cell.r <= details.endR)
        {
            float posX = details.startX + ((cell.q - details.startQ) - details.numberQ / 2) * details.hexDimX - 0.5f * details.hexDimX + details.renderDimX / 2;
            if (cell.r % 2 == 1)
            {
                posX += 0.5f * details.hexDimX;
            }
            float posY = details.startY + ((cell.r - details.startR) - details.numberR / 2) * details.hexDimY - 0.5f * details.hexDimY + details.renderDimY / 2;

            math::Point2f center{ posX, posY };

            for (decltype(howMany) i = 0; i < howMany; i++)
            {
                // Outer circle
                if (auto p = getParticle(); p != nullptr)
                {
                    setParticleParameters(p, details, center, m_distCircle(m_generator), m_distJitter(m_generator));

                    addParticle(std::move(p));
                }

                // Inner circle
                if (auto p = getParticle(); p != nullptr)
                {
                    setParticleParameters(p, details, center, m_distCircle(m_generator), 0.5f * m_distJitter(m_generator));

                    addParticle(std::move(p));
                }
            }
        }
    }

} // namespace systems