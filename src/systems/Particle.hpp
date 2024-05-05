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

#include "misc/math.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>

namespace systems
{
    struct Particle
    {
        float alphaStart{ 1.0f };
        float alphaEnd{ 1.0f };
        float alpha{ 1.0f };

        float sizeStart{ 0.0f };
        float sizeEnd{ 0.0f };
        float size{ 0.0f };

        math::Point2f origin{ 0.0f, 0.0f };
        math::Vector2f direction{ 0.0f, 0.0f };

        float speed{ 0.0f };
        float rotationRate{ 0.0f };
        float rotation{ 0.0f };

        std::uint8_t spriteCount{ 1 };
        std::chrono::microseconds spriteTime{ 1 };
        std::uint8_t currentSprite{ 0 };
        std::chrono::microseconds elapsedTime{ 0 };

        void updateElapsedTime(std::chrono::microseconds howMuch) { elapsedTime += howMuch; }
        auto incrementSprite() { currentSprite = (currentSprite + 1) % spriteCount; }

        auto getCurrentSpriteRect()
        {
            int width = (sprite.getTexture()->getSize().x / spriteCount);
            int left = currentSprite * width;
            return sf::IntRect({ left, 0, width, static_cast<int>(sprite.getTexture()->getSize().y) });
        }

        std::chrono::microseconds lifetime{ 0 };
        std::chrono::microseconds alive{ 0 };

        sf::Sprite sprite;
    };
} // namespace systems
