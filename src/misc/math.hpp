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

#include "entities/Entity.hpp"

#include <SFML/Graphics/Texture.hpp>
#include <SFML/System/Vector2.hpp>
#include <cmath>
#include <numbers>

// --------------------------------------------------------------
//
// Right now just a grab bag of math-related code that doesn't belong
// anywhere else.
//
// --------------------------------------------------------------

namespace math
{
    const float PI = 3.14159f;

    struct Vector2f
    {
        Vector2f() :
            x(0), y(0)
        {
        }

        Vector2f(float x, float y) :
            x(x), y(y)
        {
        }

        operator sf::Vector2f() { return sf::Vector2f(x, y); }

        float x;
        float y;
    };

    struct Point2f
    {
        Point2f() :
            x(0), y(0)
        {
        }

        Point2f(float x, float y) :
            x(x), y(y)
        {
        }

        operator sf::Vector2f() { return sf::Vector2f(x, y); }
        operator Vector2f() { return Vector2f(x, y); }

        float x;
        float y;
    };

    struct Dimension2f
    {
        Dimension2f() :
            width(0), height(0)
        {
        }

        Dimension2f(float width, float height) :
            width(width), height(height)
        {
        }

        operator sf::Vector2f() { return sf::Vector2f(width, height); }

        float width;
        float height;
    };

    struct Dimension2u
    {
        Dimension2u() :
            width(0), height(0)
        {
        }

        Dimension2u(unsigned int width, unsigned int height) :
            width(width), height(height)
        {
        }

        unsigned int width;
        unsigned int height;
    };

    struct Rectangle2f
    {
        Rectangle2f() :
            left(0),
            top(0),
            width(0),
            height(0)
        {
        }

        Rectangle2f(float left, float top, float width, float height) :
            left(left),
            top(top),
            width(width),
            height(height)
        {
        }

        inline bool contains(const Point2f& point) const
        {
            return (point.x >= left) && (point.x <= left + width) &&
                   (point.y >= top) && (point.y <= top + height);
        }

        inline Point2f getCenter() const
        {
            return Point2f(left + width / 2.0f, top + height / 2.0f);
        }

        inline float getBottom() const
        {
            return top + height;
        }

        float left;
        float top;
        float width;
        float height;
    };

    auto toDegrees(auto radians)
    {
        static const float RADIANS_TO_DEGREES = 180.0f / std::numbers::pi_v<float>;
        return radians * RADIANS_TO_DEGREES;
    }

    auto toRadians(auto degrees)
    {
        static const float DEGREES_TO_RADIANS = std::numbers::pi_v<float> / 180.0f;
        return degrees * DEGREES_TO_RADIANS;
    }

    float distance(const Point2f& a, const Point2f& b);
    float magnitude(const Vector2f& v);
    Vector2f vector(Point2f a, Point2f b);
    Vector2f vectorFromRadians(float radians);
    Vector2f vectorFromDegrees(float degrees);
    Vector2f reflect(Vector2f n, Vector2f l);
    bool collides(entities::Entity& a, entities::Entity& b);
    sf::Vector2f getViewScale(const Dimension2f size, const sf::Texture* texture);
    sf::Vector2f getViewScale(const float size, const sf::Texture* texture);

    auto lerp(auto x, auto x0, auto x1, auto y0, auto y1)
    {
        return y0 + (x - x0) * ((y1 - y0) / (x1 - x0));
    }
    // An overload for a specific set of data used in the code
    float lerp(std::chrono::microseconds x, std::chrono::microseconds x0, std::chrono::microseconds x1, float y0, float y1);

    Point2f pointOnLine(auto dist, auto x0, auto x1, auto y0, auto y1)
    {
        // return { x0 + dist * (x1 - x0), y0 + dist * (y1 - y0) };
        return {
            std::lerp(x0, x1, dist),
            std::lerp(y0, y1, dist)
        };
    }
} // namespace math
