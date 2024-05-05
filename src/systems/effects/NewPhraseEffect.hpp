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

#include "ParticleEffect.hpp"
#include "misc/HexCoord.hpp"

namespace systems
{
    // --------------------------------------------------------------
    //
    // Used to highlight a newly formed phrase cell
    //
    // --------------------------------------------------------------
    class NewPhraseEffect : public ParticleEffect
    {
      public:
        NewPhraseEffect(misc::HexCoord position, std::uint16_t levelWidth, std::uint16_t levelHeight);

        virtual void update(const std::chrono::microseconds elapsedTime, const entities::EntityPtr& camera, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle) override;

      private:
        misc::HexCoord m_position;
        std::uint16_t m_levelWidth;
        std::uint16_t m_levelHeight;
        std::shared_ptr<sf::Texture> m_texture;
        std::uint8_t m_spriteCount;
        std::chrono::microseconds m_spriteTime;

        void emitShower(const entities::EntityPtr& camera, misc::HexCoord cell, std::uint16_t howMany, std::function<std::unique_ptr<Particle>()> getParticle, std::function<void(std::unique_ptr<Particle>)> addParticle);
        void setParticleParams(std::unique_ptr<systems::Particle>& p, const misc::RenderingDetails& details, const math::Point2f& center, const math::Point2f& origin);
        void drawLine(std::uint16_t howMany, const misc::RenderingDetails& details, const math::Point2f& center, const math::Point2f& pt0, const math::Point2f& pt1, std::function<std::unique_ptr<Particle>()>& getParticle, std::function<void(std::unique_ptr<Particle>)>& addParticle);
    };
} // namespace systems