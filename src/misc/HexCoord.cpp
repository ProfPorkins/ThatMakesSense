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

#include "HexCoord.hpp"

#include "components/Camera.hpp"
#include "services/Configuration.hpp"

#include <algorithm> // std::max, std::min
#include <cassert>
#include <cmath>

namespace misc
{
    HexCoord::Direction HexCoord::getOpposite(Direction direction)
    {
        switch (direction)
        {
            case Direction::NW:
                return Direction::SE;
            case Direction::NE:
                return Direction::SW;
            case Direction::SW:
                return Direction::NE;
            case Direction::SE:
                return Direction::NW;
            case Direction::W:
                return Direction::E;
            case Direction::E:
                return Direction::W;
            default:
                // This really can't happen, but it is here to quell a compiler warning
                throw new std::exception();
        }
    }

    HexCoord HexCoord::getNextCell(HexCoord current, Direction direction)
    {
        switch (direction)
        {
            case Direction::NW:
                return current.NW();
            case Direction::NE:
                return current.NE();
            case Direction::SW:
                return current.SW();
            case Direction::SE:
                return current.SE();
            case Direction::W:
                return current.W();
            case Direction::E:
                return current.E();
            default:
                // This really can't happen, but it is here to quell a compiler warning
                throw new std::exception();
        }
    }

    // --------------------------------------------------------------
    //
    // Returns the direction of the vector between two adjacent cells
    //
    // --------------------------------------------------------------
    HexCoord::Direction HexCoord::getDirection(const HexCoord& first, const HexCoord& second)
    {
        misc::HexCoord diff = {
            static_cast<misc::HexCoord::Type>(second.q - first.q),
            static_cast<misc::HexCoord::Type>(second.r - first.r),
        };
        // If there is no difference, we can't handle it
        assert(!(diff.q == 0 && diff.r == 0));
        // If the difference is greater than 1, then not adjacent and we can't handle it
        assert(!(diff.q > 1));
        assert(!(diff.q < -1));
        assert(!(diff.r > 1));
        assert(!(diff.r < -1));

        // Handle even row
        if (first.r % 2 == 0)
        {
            if (diff.r == 1)
            {
                if (diff.q == 0)
                {
                    return Direction::SE;
                }
                else if (diff.q == -1)
                {
                    return Direction::SW;
                }
            }
            else if (diff.r == -1)
            {
                if (diff.q == 0)
                {
                    return Direction::NE;
                }
                else if (diff.q == -1)
                {
                    return Direction::NW;
                }
            }
            else // diff.r == 0
            {
                if (diff.q == 1)
                {
                    return Direction::E;
                }
                if (diff.q == -1)
                {
                    return Direction::W;
                }
            }
        }
        else // Handle odd row
        {
            if (diff.r == 1)
            {
                if (diff.q == 1)
                {
                    return Direction::SE;
                }
                else if (diff.q == 0)
                {
                    return Direction::SW;
                }
            }
            else if (diff.r == -1)
            {
                if (diff.q == 1)
                {
                    return Direction::NE;
                }
                else if (diff.q == 0)
                {
                    return Direction::NW;
                }
            }
            else // diff.r == 0
            {
                if (diff.q == 1)
                {
                    return Direction::E;
                }
                if (diff.q == -1)
                {
                    return Direction::W;
                }
            }
        }

        throw std::exception();
    }

    //------------------------------------------------------------------
    //
    // Returns true if the point is contained with the hex boundaries.
    // Using a point in polygon approach: http://paulbourke.net/geometry/polygonmesh/
    //
    //------------------------------------------------------------------
    bool HexCoord::pointInHex(math::Point2f point, const HexCoord& hex, float hexSizeX, float hexSizeY)
    {
        float width2 = 0.5f * hexSizeX;
        float height4 = hexSizeY / 4.0f;
        float height2 = hexSizeY / 2;

        //
        // Polygon points defined in local coordinates
        math::Point2f points[] = {
            math::Point2f(0, -height2),
            math::Point2f(width2, -height2 + height4),
            math::Point2f(width2, height2 - height4),
            math::Point2f(0, height2),
            math::Point2f(-width2, height2 - height4),
            math::Point2f(-width2, -height2 + height4),
            math::Point2f(0, -height2)
        };

        //
        // Convert to local cell coordinates
        float localX = 0.0f;
        if (hex.r % 2 == 1)
        {
            localX = point.x - hex.q * hexSizeX - 0.5f * hexSizeX;
        }
        else
        {
            localX = point.x - hex.q * hexSizeX;
        }
        float localY = point.y - hex.r * hexSizeY;

        int counter = 0;
        math::Point2f p1 = points[0];
        for (int i = 1; i <= 6; i++)
        {
            math::Point2f p2 = points[i % 6];
            if (localY > std::min(p1.y, p2.y))
            {
                if (localY <= std::max(p1.y, p2.y))
                {
                    if (localX <= std::max(p1.x, p2.x))
                    {
                        if (p1.y != p2.y)
                        {
                            float xinters = (localY - p1.y) * (p2.x - p1.x) / (p2.y - p1.y) + p1.x;
                            if (p1.x == p2.x || localX <= xinters)
                            {
                                counter++;
                            }
                        }
                    }
                }
            }
            p1 = p2;
        }

        return !(counter % 2 == 0);
    }

    //------------------------------------------------------------------
    //
    // Converts a floating point grid coordinate to a hex cell.
    // This is a lazy man's (Dean) way to do this, but it works.
    // Take the point's integer coords, convert them into the following hex's, and test each of them
    // Point (x, y)
    // Hex-Center (truncate(x), truncate(y))
    // Other Hexes : NE, E, SE, SW, W, NW
    //
    //------------------------------------------------------------------
    HexCoord HexCoord::pointToHex(const math::Point2f& point, const math::Vector2f& hexSize, const HexCoord cameraCenter)
    {
        // Transform the point by the camera
        math::Point2f ptTransform{ point.x + cameraCenter.q * hexSize.x, point.y + cameraCenter.r * hexSize.y };

        // Estimate the hex the point hits
        auto centerX = (point.x / hexSize.x) + cameraCenter.q;
        auto centerY = (point.y / hexSize.y) + cameraCenter.r;

        misc::HexCoord testCenter(static_cast<misc::HexCoord::Type>(centerX), static_cast<misc::HexCoord::Type>(centerY));
        misc::HexCoord testNE = testCenter.NE();
        misc::HexCoord testE = testCenter.E();
        misc::HexCoord testSE = testCenter.SE();
        misc::HexCoord testSW = testCenter.SW();
        misc::HexCoord testW = testCenter.W();
        misc::HexCoord testNW = testCenter.NW();

        misc::HexCoord hitCell = testCenter;
        if (pointInHex(ptTransform, testCenter, hexSize.x, hexSize.y))
        {
            hitCell = testCenter;
        }
        else if (pointInHex(ptTransform, testNE, hexSize.x, hexSize.y))
        {
            hitCell = testNE;
        }
        else if (pointInHex(ptTransform, testE, hexSize.x, hexSize.y))
        {
            hitCell = testE;
        }
        else if (pointInHex(ptTransform, testSE, hexSize.x, hexSize.y))
        {
            hitCell = testSE;
        }
        else if (pointInHex(ptTransform, testSW, hexSize.x, hexSize.y))
        {
            hitCell = testSW;
        }
        else if (pointInHex(ptTransform, testW, hexSize.x, hexSize.y))
        {
            hitCell = testW;
        }
        else if (pointInHex(ptTransform, testNW, hexSize.x, hexSize.y))
        {
            hitCell = testNW;
        }

        return hitCell;
    }

    //------------------------------------------------------------------
    //
    // Returns the 2d location of a point around the hex, where 'point' is
    // a value from 0 to 5.
    //
    //------------------------------------------------------------------
    math::Point2f HexCoord::hexPoint(math::Point2f center, float size, std::uint8_t point)
    {
        auto angle = (std::numbers::pi_v<float> / 180) * (60 * point - 30);
        return math::Point2f(center.x + size * std::cos(angle), center.y + size * std::sin(angle));
    }

    //------------------------------------------------------------------
    //
    // This is used by the various HexGridRendering systems, along with
    // the particle system (effects) to know where to render/place things.
    // It is just a mess of things that needed to be computed to do that.
    //
    //------------------------------------------------------------------
    RenderingDetails computeRenderingDetails(const entities::EntityPtr& cameraEntity, std::uint16_t levelWidth, std::uint16_t levelHeight)
    {
        RenderingDetails details;

        auto camera = cameraEntity->getComponent<components::Camera>();

        // Camera range tells us how many hexes at min to draw along the longest side.
        // Given the aspect ratio, we can determine how many hexes fit in each direction.
        details.numberR = camera->getRange();
        details.numberQ = camera->getRange();

        details.coords = Configuration::getGraphics().getViewCoordinates();
        auto aspectRatio = details.coords.height / details.coords.width;

        if (aspectRatio >= 1) // Taller
        {
            details.numberR += static_cast<uint16_t>((aspectRatio - 1.0f) * details.numberR);

            // These sizes are the rectangular drawing area
            details.renderDimX = details.coords.width / camera->getRange();
            details.renderDimY = aspectRatio * (details.coords.height / camera->getRange());
        }
        else // Wider
        {
            details.numberQ += static_cast<uint16_t>((1.0f - aspectRatio) * details.numberQ);

            // These sizes are the rectangular drawing area
            details.renderDimX = aspectRatio * (details.coords.width / camera->getRange());
            details.renderDimY = details.coords.height / camera->getRange();
        }

        // These sizes are the hex cell drawing dimensions
        details.hexDimX = details.renderDimX * misc::HEX_HORIZONTAL_DISTANCE;
        details.hexDimY = details.renderDimY * misc::HEX_VERTICAL_DISTANCE;

        // Based on the center of the camera, the visible range of cells, and level size
        // determine the starting/ending q and r values.
        details.offsetR = camera->getCenter().r - details.numberR / 2;
        details.offsetQ = camera->getCenter().q - details.numberQ / 2;

        details.startR = static_cast<std::uint16_t>(std::max(0, details.offsetR));
        details.startQ = static_cast<std::uint16_t>(std::max(0, details.offsetQ));

        details.endR = std::min(static_cast<std::uint16_t>(camera->getCenter().r + details.numberR / 2), levelHeight) - 1;
        details.endQ = std::min(static_cast<std::uint16_t>(camera->getCenter().q + details.numberQ / 2), levelWidth) - 1;

        // Adjust for the case where the camera causes the left/top edge to be indented
        details.startX = details.offsetQ >= 0 ? 0 : details.hexDimX * std::abs(details.offsetQ);
        details.startY = details.offsetR >= 0 ? 0 : details.hexDimY * std::abs(details.offsetR);

        return details;
    }

    math::Vector2f computeCellSize(const components::Camera* camera)
    {
        auto coords = Configuration::getGraphics().getViewCoordinates();
        auto aspectRatio = coords.height / coords.width;
        if (aspectRatio >= 1) // Taller
        {
            return {
                (coords.width / camera->getRange()) * misc::HEX_HORIZONTAL_DISTANCE,
                aspectRatio * (coords.height / camera->getRange()) * misc::HEX_VERTICAL_DISTANCE
            };
        }
        else // Wider
        {
            return {
                (aspectRatio * (coords.width / camera->getRange())) * misc::HEX_HORIZONTAL_DISTANCE,
                (coords.height / camera->getRange()) * misc::HEX_VERTICAL_DISTANCE
            };
        }
    }
} // namespace misc
