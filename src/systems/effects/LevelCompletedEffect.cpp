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

#include "LevelCompletedEffect.hpp"

#include "components/Camera.hpp"
#include "components/Property.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <cmath>
#include <numbers>
#include <ranges>

namespace systems
{
    // This is the length of the music that plays, so matching how long it
    // takes to get all the fireworks done.
    const auto EFFECT_LIFETIME = misc::msTous(std::chrono::milliseconds(2000));

    LevelCompletedEffect::LevelCompletedEffect(std::shared_ptr<Level> level, Type type) :
        ParticleEffect(EFFECT_LIFETIME),
        m_level(level),
        m_generator(std::random_device()()),
        m_distNormal(1, 0.25f),
        m_distCircle(0.0f, 2.0f * std::numbers::pi_v<float>)
    {
        static const auto hasIorGoal = [](auto entity)
        {
            if (entity->template hasComponent<components::Property>())
            {
                auto property = entity->template getComponent<components::Property>();
                return property->has(components::PropertyType::I) || property->has(components::PropertyType::Goal);
            }

            return false;
        };

        if (type == Type::PreDefined)
        {
            m_texture = Content::get<sf::Texture>(content::KEY_IMAGE_PARTICLE_LEVEL_COMPLETE_PRE);
            m_spriteCount = Configuration::get<std::uint8_t>(config::IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_COUNT_PRE);
            m_spriteTime = misc::msTous(Configuration::get<std::chrono::milliseconds>(config::IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_TIME_PRE));
        }
        else if (type == Type::BeyondCategory)
        {
            m_texture = Content::get<sf::Texture>(content::KEY_IMAGE_PARTICLE_LEVEL_COMPLETE_BC);
            m_spriteCount = Configuration::get<std::uint8_t>(config::IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_COUNT_BC);
            m_spriteTime = misc::msTous(Configuration::get<std::chrono::milliseconds>(config::IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_TIME_BC));
        }

        // Find all the locations that have a Goal enttiy and an I entity
        for (std::int16_t r = 0; r < level->getHeight(); r++)
        {
            for (std::int16_t q = 0; q < level->getWidth(); q++)
            {
                bool hasI{ false };
                bool hasGoal{ false };
                for (auto&& entity : level->getEntities({ q, r }) | std::views::values | std::views::filter(hasIorGoal))
                {
                    hasI = hasI || entity->getComponent<components::Property>()->has(components::PropertyType::I);
                    hasGoal = hasGoal || entity->getComponent<components::Property>()->has(components::PropertyType::Goal);
                }
                if (hasI && hasGoal)
                {
                    m_positions.push({ q, r });
                }
            }
        }

        m_nextShowerDelta = EFFECT_LIFETIME / m_positions.size();
        m_countdown = m_nextShowerDelta; // So it starts right away
    }

    void LevelCompletedEffect::update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle)
    {
        ParticleEffect::update(elapsedTime);

        // Every ?? ms emit a bunch more particles, centered at the position of the entity
        // at the front of the queue.
        m_countdown += elapsedTime;
        if (m_positions.size() > 0 && m_countdown > m_nextShowerDelta)
        {
            emitShower(camera, m_positions.front(), 100, getParticle, addParticle);
            m_positions.pop();

            m_countdown -= m_nextShowerDelta;
        }
    }

    void LevelCompletedEffect::emitShower(const entities::EntityPtr& camera, misc::HexCoord cell, std::uint16_t howMany, std::function<std::unique_ptr<Particle>()>& getParticle, std::function<void(std::unique_ptr<Particle>)>& addParticle)
    {
        auto details = misc::computeRenderingDetails(camera, m_level->getWidth(), m_level->getHeight());

        // Only create particles if the cell is currently visible
        if (cell.q >= details.startQ && cell.q <= details.endQ && cell.r >= details.startR && cell.r <= details.endR)
        {
            float posX = details.startX + ((cell.q - details.startQ) - details.numberQ / 2) * details.hexDimX - 0.5f * details.hexDimX + details.renderDimX / 2;
            if (cell.r % 2 == 1)
            {
                posX += 0.5f * details.hexDimX;
            }
            float posY = details.startY + ((cell.r - details.startR) - details.numberR / 2) * details.hexDimY - 0.5f * details.hexDimY + details.renderDimY / 2;

            for (decltype(howMany) i = 0; i < howMany; i++)
            {
                if (auto p = getParticle(); p != nullptr)
                {
                    p->lifetime = misc::msTous(std::chrono::milliseconds(static_cast<int>(2000 * m_distNormal(m_generator))));
                    p->alive = std::chrono::microseconds::zero();
                    p->currentSprite = 0;
                    p->elapsedTime = std::chrono::microseconds::zero();
                    p->spriteCount = m_spriteCount;
                    p->spriteTime = p->lifetime / m_spriteCount;

                    // Particle size is scaled based on the camera zoom; using the cell rendering size as a proxy for this
                    p->size = p->sizeStart = details.renderDimX * 0.1f;
                    p->sizeEnd = details.renderDimX * 0.3f;

                    auto angle = m_distCircle(m_generator);

                    // Hack job to render them in the correct location
                    p->origin.x = posX + std::cos(angle);
                    p->origin.y = posY + std::sin(angle);

                    // Direction is outward from the center of the location
                    p->direction.x = p->origin.x - posX;
                    p->direction.y = p->origin.y - posY;

                    // Speed is scaled based on the camera zoom; using the cell rendering size as a proxy for this
                    p->speed = details.renderDimX * 0.0000005f * m_distNormal(m_generator);
                    p->rotationRate = m_distNormal(m_generator) * 0.0002f;

                    p->sprite.setTexture(*m_texture);
                    auto textureSizeX = p->sprite.getTexture()->getSize().x / static_cast<float>(p->spriteCount);
                    p->sprite.setOrigin(
                        textureSizeX / 2.0f,
                        p->sprite.getTexture()->getSize().y / 2.0f);
                    p->sprite.setScale(math::getViewScale({ p->size * static_cast<float>(p->spriteCount), p->size * 1.0f }, p->sprite.getTexture()));

                    addParticle(std::move(p));
                }
            }
        }
    }

} // namespace systems