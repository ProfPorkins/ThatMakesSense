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

#include "RuleChangedEffect.hpp"

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
    RuleChangedEffect::RuleChangedEffect(const entities::EntityMap& allEntities, const entities::EntitySet& highlightEntities, std::uint16_t levelWidth, std::uint16_t levelHeight) :
        m_entities(allEntities),
        m_levelWidth(levelWidth),
        m_levelHeight(levelHeight),
        m_texture(Content::get<sf::Texture>(content::KEY_IMAGE_PARTICLE_GENERAL)),
        m_spriteCount(Configuration::get<std::uint8_t>(config::IMAGE_PARTICLE_GENERAL_SPRITE_COUNT)),
        m_spriteTime(misc::msTous(Configuration::get<std::chrono::milliseconds>(config::IMAGE_PARTICLE_GENERAL_SPRITE_TIME))),
        m_generator(std::random_device()()),
        m_distNormal(1, 0.25f),
        m_distCircle(0.0f, 2.0f * std::numbers::pi_v<float>)
    {
        // Take all the entities and place them into a queue that we'll later use in the
        // update to generate particles within.
        for (auto&& id : highlightEntities)
        {
            m_highlight.push(id);
        }
    }

    void RuleChangedEffect::update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle)
    {
        ParticleEffect::update(elapsedTime);

        while (!m_highlight.empty())
        {
            // Due to how rules can cause changes in rules, which then cascade into removing
            // entities, it can be the case a word entities has been deleted by the time we
            // get here, so have to deal with that condition.
            if (m_entities.contains(m_highlight.front()))
            {
                auto position = m_entities.at(m_highlight.front())->getComponent<components::Position>()->get();
                emitShower(camera, position, 100, getParticle, addParticle);
            }

            m_highlight.pop();
        }
    }

    void RuleChangedEffect::emitShower(const entities::EntityPtr& camera, misc::HexCoord cell, std::uint16_t howMany, std::function<std::unique_ptr<Particle>()>& getParticle, std::function<void(std::unique_ptr<Particle>)>& addParticle)
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

            for (decltype(howMany) i = 0; i < howMany; i++)
            {
                if (auto p = getParticle(); p != nullptr)
                {
                    p->lifetime = misc::msTous(std::chrono::milliseconds(static_cast<int>(500 * m_distNormal(m_generator))));
                    p->alive = std::chrono::microseconds::zero();
                    p->currentSprite = 0;
                    p->elapsedTime = std::chrono::microseconds::zero();
                    p->spriteCount = m_spriteCount;
                    p->spriteTime = p->lifetime / m_spriteCount;

                    // Particle size is scaled based on the camera zoom; using the cell rendering size as a proxy for this
                    p->size = p->sizeStart = details.renderDimX * 0.1f;
                    p->sizeEnd = details.renderDimX * 0.1f;

                    auto angle = m_distCircle(m_generator);

                    // Hack job to render them in the correct location
                    p->origin.x = posX + std::cos(angle);
                    p->origin.y = posY + std::sin(angle);

                    // Direction is outward from the center of the location
                    p->direction.x = p->origin.x - posX;
                    p->direction.y = p->origin.y - posY;

                    // Speed is scaled based on the camera zoom; using the cell rendering size as a proxy for this
                    p->speed = details.renderDimX * 0.00000050f * m_distNormal(m_generator);
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