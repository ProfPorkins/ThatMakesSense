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

#include "BurnEffect.hpp"

#include "components/Camera.hpp"
#include "components/Position.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <cmath>

namespace systems
{
    BurnEffect::BurnEffect(misc::HexCoord position, std::uint16_t levelWidth, std::uint16_t levelHeight) :
        m_position(position),
        m_levelWidth(levelWidth),
        m_levelHeight(levelHeight),
        m_texture(Content::get<sf::Texture>(content::KEY_IMAGE_PARTICLE_BURN)),
        m_spriteCount(Configuration::get<std::uint8_t>(config::IMAGE_PARTICLE_BURN_SPRITE_COUNT)),
        m_spriteTime(misc::msTous(Configuration::get<std::chrono::milliseconds>(config::IMAGE_PARTICLE_BURN_SPRITE_TIME)))
    {
    }

    void BurnEffect::update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle)
    {
        ParticleEffect::update(elapsedTime);

        emitShower(camera, m_position, 4, getParticle, addParticle);
    }

    void BurnEffect::setParticleParams(std::unique_ptr<systems::Particle>& p, const misc::RenderingDetails& details, const math::Point2f& origin)
    {
        p->lifetime = m_spriteCount * m_spriteTime;
        p->alive = std::chrono::microseconds::zero();
        p->currentSprite = 0;
        p->elapsedTime = std::chrono::microseconds::zero();
        p->spriteCount = m_spriteCount;
        p->spriteTime = p->lifetime / m_spriteCount;

        // Particle size is scaled based on the camera zoom; using the cell rendering size as a proxy for this
        p->size = p->sizeStart = details.renderDimX * 0.015f;
        p->sizeEnd = details.renderDimX * 0.25f;

        p->origin = origin;

        // There is no direction, they simply stay in place
        p->direction.x = 0;
        p->direction.y = 0;
        p->rotationRate = 0;

        // Because no direction, no speed either
        p->speed = 0;

        p->sprite.setTexture(*m_texture);
        auto textureSizeX = p->sprite.getTexture()->getSize().x / static_cast<float>(p->spriteCount);
        p->sprite.setOrigin(
            textureSizeX / 2.0f,
            p->sprite.getTexture()->getSize().y / 2.0f);
        p->sprite.setScale(math::getViewScale({ p->size * static_cast<float>(p->spriteCount), p->size * 1.0f }, p->sprite.getTexture()));
    }

    void BurnEffect::drawLine(std::uint16_t howMany, const misc::RenderingDetails& details, const math::Point2f& pt0, const math::Point2f& pt1, std::function<std::unique_ptr<Particle>()>& getParticle, std::function<void(std::unique_ptr<Particle>)>& addParticle)
    {
        for (decltype(howMany) i = 0; i < howMany; i++)
        {
            if (auto p = getParticle(); p != nullptr)
            {
                setParticleParams(p, details, math::pointOnLine(i / (howMany - 1.0f), pt0.x, pt1.x, pt0.y, pt1.y));

                addParticle(std::move(p));
            }
        }
    }

    void BurnEffect::emitShower(const entities::EntityPtr& camera, misc::HexCoord cell, std::uint16_t howMany, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle)
    {
        auto details = misc::computeRenderingDetails(camera, m_levelWidth, m_levelHeight);

        // Only create particles if the cell is currently visible
        if (cell.q >= details.startQ && cell.q <= details.endQ && cell.r >= details.startR && cell.r <= details.endR)
        {
            float posX = details.startX + ((cell.q - details.startQ) - static_cast<float>(details.numberQ / 2)) * details.hexDimX - 0.5f * details.hexDimX + details.renderDimX / 2.0f;
            if (cell.r % 2 == 1)
            {
                posX += 0.5f * details.hexDimX;
            }
            float posY = details.startY + ((cell.r - details.startR) - static_cast<float>(details.numberR / 2)) * details.hexDimY - 0.5f * details.hexDimY + details.renderDimY / 2.0f;

            // Compute the six points of the hex
            math::Point2f center{ posX, posY };
            {
                math::Point2f top{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 5) };
                math::Point2f topRight{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 0) };
                math::Point2f bottomRight{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 1) };
                math::Point2f bottom{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 2) };
                math::Point2f bottomLeft{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 3) };
                math::Point2f topLeft{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 4) };

                drawLine(howMany, details, top, topRight, getParticle, addParticle);
                drawLine(howMany, details, topRight, bottomRight, getParticle, addParticle);
                drawLine(howMany, details, bottomRight, bottom, getParticle, addParticle);
                drawLine(howMany, details, bottom, bottomLeft, getParticle, addParticle);
                drawLine(howMany, details, bottomLeft, topLeft, getParticle, addParticle);
                drawLine(howMany, details, topLeft, top, getParticle, addParticle);
            }

            // Then another set of particles inside these lines
            details.renderDimY *= 0.55f;
            {
                math::Point2f top{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 5) };
                math::Point2f topRight{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 0) };
                math::Point2f bottomRight{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 1) };
                math::Point2f bottom{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 2) };
                math::Point2f bottomLeft{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 3) };
                math::Point2f topLeft{ misc::HexCoord::hexPoint(center, details.renderDimY / 2, 4) };

                drawLine(howMany - 1, details, top, topRight, getParticle, addParticle);
                drawLine(howMany - 1, details, topRight, bottomRight, getParticle, addParticle);
                drawLine(howMany - 1, details, bottomRight, bottom, getParticle, addParticle);
                drawLine(howMany - 1, details, bottom, bottomLeft, getParticle, addParticle);
                drawLine(howMany - 1, details, bottomLeft, topLeft, getParticle, addParticle);
                drawLine(howMany - 1, details, topLeft, top, getParticle, addParticle);
            }

            // Finally, one more particle in the center
            if (auto p = getParticle(); p != nullptr)
            {
                setParticleParams(p, details, center);

                addParticle(std::move(p));
            }
        }
    }

} // namespace systems