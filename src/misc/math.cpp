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

#include "math.hpp"

#include "components/Position.hpp"
#include "services/Configuration.hpp"

#include <chrono>
#include <cmath>

namespace math
{
    float distance(const Point2f& a, const Point2f& b)
    {
        return std::sqrt(std::pow(a.x - b.x, 2.0f) + std::pow(a.y - b.y, 2.0f));
    }

    float magnitude(const Vector2f& v)
    {
        return std::sqrt(v.x * v.x + v.y * v.y);
    }

    Vector2f vector(Point2f a, Point2f b)
    {
        return { a.x - b.x, a.y - b.y };
    }

    Vector2f vectorFromRadians(float radians)
    {
        return math::Vector2f(std::cos(radians), std::sin(radians));
    }

    Vector2f vectorFromDegrees(float degrees)
    {
        return math::Vector2f(std::cos(toRadians(degrees)), std::sin(toRadians(degrees)));
    }

    // --------------------------------------------------------------
    //
    // This function is used to compute a scaling factor for a game object
    // based on how many units in 'size' it should be based on the view
    // coordinates settings, but has a texture of potentially any number of pixels.
    //
    // --------------------------------------------------------------
    sf::Vector2f getViewScale(const Dimension2f size, const sf::Texture* texture)
    {
        auto coords = Configuration::getGraphics().getViewCoordinates();
        return { (size.width / coords.width) * (coords.width / texture->getSize().x),
                 (size.height / coords.height) * (coords.height / texture->getSize().y) };
    }

    sf::Vector2f getViewScale(const float size, const sf::Texture* texture)
    {
        return getViewScale({ size, size }, texture);
    }

    float lerp(std::chrono::microseconds x, std::chrono::microseconds x0, std::chrono::microseconds x1, float y0, float y1)
    {
        return y0 + (x.count() - x0.count()) * ((y1 - y0) / (x1.count() - x0.count()));
    }

} // namespace math