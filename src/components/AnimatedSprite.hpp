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

#include "Component.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <cstdint>
#include <memory>

namespace components
{
    class AnimatedSprite : public PolymorphicComparable<Component, AnimatedSprite>
    {
      public:
        AnimatedSprite(std::shared_ptr<sf::Texture> texture, std::uint8_t spriteCount, std::chrono::microseconds spriteTime, sf::Color spriteColor) :
            m_spriteCount(spriteCount),
            m_spriteTime(spriteTime),
            m_spriteColor(spriteColor)
        {
            m_sprite = std::make_shared<sf::Sprite>();
            m_sprite->setTexture(*texture);
        }

        //
        // This one is only used and intended to be used by the clone method below
        AnimatedSprite(std::shared_ptr<sf::Sprite> sprite, std::uint8_t spriteCount, std::chrono::microseconds spriteTime, sf::Color spriteColor) :
            m_sprite(sprite),
            m_spriteCount(spriteCount),
            m_spriteTime(spriteTime),
            m_spriteColor(spriteColor),
            m_currentSprite(0),
            m_elapsedTime(0)
        {
        }

        auto getSprite() { return m_sprite; }
        auto getSpriteCount() { return m_spriteCount; }
        auto getSpriteTime() { return m_spriteTime; }
        auto getSpriteColor() { return m_spriteColor; }
        auto getCurrentSprite() { return m_currentSprite; }
        auto getCurrentSpriteRect()
        {
            int width = (m_sprite->getTexture()->getSize().x / m_spriteCount);
            int left = m_currentSprite * width;
            return sf::IntRect({ left, 0, width, static_cast<int>(m_sprite->getTexture()->getSize().y) });
        }
        auto incrementSprite() { m_currentSprite = (m_currentSprite + 1) % m_spriteCount; }
        void resetAnimation()
        {
            m_currentSprite = 0;
            m_elapsedTime = std::chrono::microseconds::zero();
        }
        auto getElapsedTime() { return m_elapsedTime; }
        void updateElapsedTime(std::chrono::microseconds howMuch) { m_elapsedTime += howMuch; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<AnimatedSprite>(), std::make_unique<AnimatedSprite>(m_sprite, m_spriteCount, m_spriteTime, m_spriteColor) };
        }

        bool operator==(AnimatedSprite& rhs)
        {
            return m_sprite == rhs.m_sprite &&
                   m_spriteCount == rhs.m_spriteCount &&
                   m_spriteTime == rhs.m_spriteTime &&
                   m_spriteColor == rhs.m_spriteColor;
        }

      private:
        std::shared_ptr<sf::Sprite> m_sprite;
        std::uint8_t m_spriteCount;
        std::chrono::microseconds m_spriteTime;
        sf::Color m_spriteColor;
        std::uint8_t m_currentSprite{ 0 };
        std::chrono::microseconds m_elapsedTime{ 0 };
    };
} // namespace components
