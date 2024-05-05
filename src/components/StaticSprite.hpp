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
#include <cstdint>
#include <memory>

namespace components
{
    class StaticSprite : public PolymorphicComparable<Component, StaticSprite>
    {
      public:
        StaticSprite(std::shared_ptr<sf::Texture> texture, sf::Color spriteColor) :
            m_spriteColor(spriteColor)
        {
            m_sprite = std::make_shared<sf::Sprite>();
            m_sprite->setTexture(*texture);
            // Point about which drawing, rotation, etc takes place, the center of the texture
            m_sprite->setOrigin({ texture->getSize().x / 2.0f, texture->getSize().y / 2.0f });
        }

        //
        // This one is only used and intended to be used by the clone method below
        StaticSprite(std::shared_ptr<sf::Sprite> sprite, sf::Color spriteColor) :
            m_sprite(sprite),
            m_spriteColor(spriteColor)
        {
        }

        auto getSprite() { return m_sprite; }
        auto getSpriteColor() { return m_spriteColor; }
        auto getCurrentSpriteRect()
        {
            return sf::IntRect({ 0, 0, static_cast<int>(m_sprite->getTexture()->getSize().x), static_cast<int>(m_sprite->getTexture()->getSize().y) });
        }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<StaticSprite>(), std::make_unique<StaticSprite>(m_sprite, m_spriteColor) };
        }

        bool operator==(StaticSprite& rhs)
        {
            return m_sprite == rhs.m_sprite &&
                   m_spriteColor == rhs.m_spriteColor;
        }

      private:
        std::shared_ptr<sf::Sprite> m_sprite;
        sf::Color m_spriteColor;
    };
} // namespace components
