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

#include "misc/HexCoord.hpp"
#include "misc/math.hpp"

#include <gtest/gtest.h>

TEST(HexTests, PointInHex00)
{
    misc::HexCoord hex{ 0, 0 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };

    math::Point2f ptCenter{ 0, 0 };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptCenter, hex, size.x, size.y), true);

    math::Point2f ptLeft{ ptCenter.x - 0.49f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptLeft, hex, size.x, size.y), true);

    math::Point2f ptRight{ ptCenter.x + 0.49f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptRight, hex, size.x, size.y), true);

    math::Point2f ptTop{ ptCenter.x, ptCenter.y - 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptTop, hex, size.x, size.y), true);

    math::Point2f ptBottom{ ptCenter.x, ptCenter.y + 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptBottom, hex, size.x, size.y), true);
}

TEST(HexTests, PointNotInHex00)
{
    misc::HexCoord hex{ 0, 0 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };
    math::Point2f ptCenter{ 0, 0 };

    math::Point2f ptLeft{ ptCenter.x - 0.51f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptLeft, hex, size.x, size.y), false);

    math::Point2f ptRight{ ptCenter.x + 0.51f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptRight, hex, size.x, size.y), false);

    math::Point2f ptTop{ ptCenter.x, ptCenter.y - 0.51f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptTop, hex, size.x, size.y), false);

    math::Point2f ptBottom{ ptCenter.x, ptCenter.y + 0.51f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptBottom, hex, size.x, size.y), false);
}

TEST(HexTests, PointInHex10)
{
    misc::HexCoord hex{ 1, 0 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };

    math::Point2f ptCenter{ misc::HEX_HORIZONTAL_DISTANCE, 0 };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptCenter, hex, size.x, size.y), true);

    math::Point2f ptLeft{ ptCenter.x - 0.49f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptLeft, hex, size.x, size.y), true);

    math::Point2f ptRight{ ptCenter.x + 0.49f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptRight, hex, size.x, size.y), true);

    math::Point2f ptTop{ ptCenter.x, ptCenter.y - 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptTop, hex, size.x, size.y), true);

    math::Point2f ptBottom{ ptCenter.x, ptCenter.y + 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptBottom, hex, size.x, size.y), true);
}

TEST(HexTests, PointNotInHex10)
{
    misc::HexCoord hex{ 1, 0 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };
    math::Point2f ptCenter{ misc::HEX_HORIZONTAL_DISTANCE, 0 };

    math::Point2f ptLeft{ ptCenter.x - 0.51f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptLeft, hex, size.x, size.y), false);

    math::Point2f ptRight{ ptCenter.x + 0.51f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptRight, hex, size.x, size.y), false);

    math::Point2f ptTop{ ptCenter.x, ptCenter.y - 0.51f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptTop, hex, size.x, size.y), false);

    math::Point2f ptBottom{ ptCenter.x, ptCenter.y + 0.51f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptBottom, hex, size.x, size.y), false);
}

TEST(HexTests, PointInHex01)
{
    misc::HexCoord hex{ 0, 1 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };

    math::Point2f ptCenter{ 0.5f * misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptCenter, hex, size.x, size.y), true);

    math::Point2f ptLeft{ ptCenter.x - 0.49f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptLeft, hex, size.x, size.y), true);

    math::Point2f ptRight{ ptCenter.x + 0.49f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptRight, hex, size.x, size.y), true);

    math::Point2f ptTop{ ptCenter.x, ptCenter.y - 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptTop, hex, size.x, size.y), true);

    math::Point2f ptBottom{ ptCenter.x, ptCenter.y + 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptBottom, hex, size.x, size.y), true);
}

TEST(HexTests, PointNotInHex01)
{
    misc::HexCoord hex{ 0, 1 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };
    math::Point2f ptCenter{ 0.5f * misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };

    math::Point2f ptLeft{ ptCenter.x - 0.51f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptLeft, hex, size.x, size.y), false);

    math::Point2f ptRight{ ptCenter.x + 0.51f * misc::HEX_HORIZONTAL_DISTANCE, ptCenter.y };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptRight, hex, size.x, size.y), false);

    math::Point2f ptTop{ ptCenter.x, ptCenter.y - 0.51f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptTop, hex, size.x, size.y), false);

    math::Point2f ptBottom{ ptCenter.x, ptCenter.y + 0.51f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointInHex(ptBottom, hex, size.x, size.y), false);
}

TEST(HexTests, PointToHex00) // 00 is for camera at {0, 0}
{
    misc::HexCoord camera{ 0, 0 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };

    // Tests for hex [0, 0]
    misc::HexCoord hex00Expected{ 0, 0 };
    math::Point2f pt00Center{ 0, 0 };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Center, size, camera), hex00Expected);

    math::Point2f pt00Left{ pt00Center.x - 0.49f * misc::HEX_HORIZONTAL_DISTANCE, pt00Center.y };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Left, size, camera), hex00Expected);

    math::Point2f pt00Right{ pt00Center.x + 0.49f * misc::HEX_HORIZONTAL_DISTANCE, pt00Center.y };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Right, size, camera), hex00Expected);

    math::Point2f pt00Top{ pt00Center.x, pt00Center.y - 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Top, size, camera), hex00Expected);

    math::Point2f pt00Bottom{ pt00Center.x, pt00Center.y + 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Bottom, size, camera), hex00Expected);

    // Tests for hex [1, 0]
    misc::HexCoord hex10Expected{ 1, 0 };
    math::Point2f pt10Center{ misc::HEX_HORIZONTAL_DISTANCE, 0 };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt10Center, size, camera), hex10Expected);

    math::Point2f pt10Right{ pt10Center.x + 0.49f * misc::HEX_HORIZONTAL_DISTANCE, pt10Center.y };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt10Right, size, camera), hex10Expected);

    math::Point2f pt10Top{ pt10Center.x, pt10Center.y - 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt10Top, size, camera), hex10Expected);

    math::Point2f pt10Bottom{ pt10Center.x, pt10Center.y + 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt10Bottom, size, camera), hex10Expected);

    // Tests for hex [0, 1]
    misc::HexCoord hex01Expected{ 0, 1 };
    math::Point2f pt01Center{ 0.5f * misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt01Center, size, camera), hex01Expected);

    math::Point2f pt01Right{ pt01Center.x + 0.49f * misc::HEX_HORIZONTAL_DISTANCE, pt01Center.y };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt01Right, size, camera), hex01Expected);

    math::Point2f pt01Top{ pt01Center.x, pt01Center.y - 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt01Top, size, camera), hex01Expected);

    math::Point2f pt01Bottom{ pt01Center.x, pt01Center.y + 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt01Bottom, size, camera), hex01Expected);
}

TEST(HexTests, PointToHex10) // 10 is for camera at {1, 0}
{
    misc::HexCoord camera{ 1, 0 };
    math::Vector2f size{ misc::HEX_HORIZONTAL_DISTANCE, misc::HEX_VERTICAL_DISTANCE };

    // Tests for hex [1, 0]
    misc::HexCoord hex10Expected{ 1, 0 };
    math::Point2f pt10Center{ 0, 0 };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt10Center, size, camera), hex10Expected);

    math::Point2f pt00Left{ pt10Center.x - 0.49f * misc::HEX_HORIZONTAL_DISTANCE, pt10Center.y };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Left, size, camera), hex10Expected);

    math::Point2f pt00Right{ pt10Center.x + 0.49f * misc::HEX_HORIZONTAL_DISTANCE, pt10Center.y };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Right, size, camera), hex10Expected);

    math::Point2f pt00Top{ pt10Center.x, pt10Center.y - 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Top, size, camera), hex10Expected);

    math::Point2f pt00Bottom{ pt10Center.x, pt10Center.y + 0.49f * misc::HEX_VERTICAL_DISTANCE };
    EXPECT_EQ(misc::HexCoord::pointToHex(pt00Bottom, size, camera), hex10Expected);
}