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

#include "math.hpp"

#include <cmath>
#include <functional> // std::hash

// Forward declaration for the computeCellSize function
namespace components
{
    class Camera;
}

// --------------------------------------------------------------
//
// Code related to Hexes
//
// --------------------------------------------------------------
namespace misc
{
    //
    // Reference: https://www.redblobgames.com/grids/hexagons/
    //
    static const float HEX_VERTICAL_DISTANCE = 3.0f / 4.0f;              // given by definition
    static const float HEX_HORIZONTAL_DISTANCE = std::sqrt(3.0f) / 2.0f; // given by definition

    struct HexCoord
    {
        enum class Direction : std::uint8_t
        {
            NW = 0,
            NE = 1,
            W = 2,
            E = 3,
            SW = 4,
            SE = 5
        };

        // These have to be signed values to allow them to go negative when testing
        // for valid positions and directions
        using Type = std::int16_t;

        HexCoord(Type _q, Type _r) :
            // clang wanted this, so here it is
            q(_q),
            r(_r)
        {
        }
        HexCoord() = default;

        Type q; // column
        Type r; // row

        bool operator==(const HexCoord& rhs) const
        {
            return q == rhs.q && r == rhs.r;
        }

        HexCoord NW() const { return HexCoord(q - (r % 2 == 0 ? 1 : 0), r - 1); }
        HexCoord NE() const { return HexCoord(q + (r % 2 == 0 ? 0 : 1), r - 1); }
        HexCoord SW() const { return HexCoord(q - (r % 2 == 0 ? 1 : 0), r + 1); }
        HexCoord SE() const { return HexCoord(q + (r % 2 == 0 ? 0 : 1), r + 1); }
        HexCoord W() const { return HexCoord(q - 1, r); }
        HexCoord E() const { return HexCoord(q + 1, r); }

        bool isValid(const Type width, Type height) const
        {
            return (r >= 0 && q >= 0 && r < height && q < width);
        }

        static Direction getOpposite(Direction direction);
        static HexCoord getNextCell(HexCoord current, Direction direction);
        static Direction getDirection(const HexCoord& first, const HexCoord& second);
        static bool pointInHex(math::Point2f point, const HexCoord& hex, float hexSizeX, float hexSizeY);
        static HexCoord pointToHex(const math::Point2f& point, const math::Vector2f& hexSize, const HexCoord cameraCenter);
        static math::Point2f hexPoint(math::Point2f center, float size, std::uint8_t point);
    };

    // For lack of a better name, this struct contains a bunch of items needed to render
    // hex grids and the placement of particles.
    struct RenderingDetails
    {
        math::Dimension2f coords{};

        std::uint16_t numberR{};
        std::uint16_t numberQ{};

        int offsetR{};
        int offsetQ{};

        std::uint16_t startR{};
        std::uint16_t startQ{};
        std::uint16_t endR{};
        std::uint16_t endQ{};

        float hexDimX{};
        float hexDimY{};

        float renderDimX{};
        float renderDimY{};

        float startX{};
        float startY{};
    };

    RenderingDetails computeRenderingDetails(const entities::EntityPtr& cameraEntity, std::uint16_t levelWidth, std::uint16_t levelHeight);
    math::Vector2f computeCellSize(const components::Camera* camera);

} // namespace misc

namespace std
{

    // --------------------------------------------------------------
    //
    // Hash function for misc::HexCoord, to allow it to be used in
    // std:: containers where a hash function is needed.
    //
    // Reference: https://stackoverflow.com/questions/2634690/good-hash-function-for-a-2d-index
    //
    // --------------------------------------------------------------
    template <>
    struct hash<misc::HexCoord>
    {
        size_t operator()(const misc::HexCoord& v) const noexcept
        {
            static const auto HASH_PRIME = 19997;
            return HASH_PRIME + hash<decltype(v.r)>{}(v.r) * HASH_PRIME + hash<decltype(v.q)>{}(v.q);
        }
    };
} // namespace std