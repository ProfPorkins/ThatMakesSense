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

#include "systems/parser/Parser.hpp"

#include <deque>
#include <gtest/gtest.h>
#include <string>

auto transformToParserQueue(std::deque<components::TextType>& original)
{
    std::deque<systems::parser::Parser::PhrasePair> phrase;

    for (auto&& word : original)
    {
        phrase.push_back({ word, { 0, 0 } });
    }

    return phrase;
}

TEST(ValidPhrases, IAmObjects)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Am, TextType::Text_Purple };
    std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green };
    std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red };
    std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Grey };
    std::deque<TextType> phrase5{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Grey, TextType::Text_And, TextType::Text_Yellow, TextType::Text_And, TextType::Text_Blue };
    std::deque<TextType> phrase6{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Valid);
}

TEST(ValidPhrases, ICanAbilities)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_I, TextType::Text_Can, TextType::Text_Climb };
    std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push };
    std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Can, TextType::Text_Pull };
    std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Float };
    std::deque<TextType> phrase5{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Float, TextType::Text_And, TextType::Text_Pull };
    std::deque<TextType> phrase6{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Can, TextType::Text_Float };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Valid);
}

TEST(ValidPhrases, IAmObjectsCanAbilities)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1 = { TextType::Text_I, TextType::Text_Can, TextType::Text_Climb, TextType::Text_And, TextType::Text_Am, TextType::Text_Blue };
    std::deque<TextType> phrase2 = { TextType::Text_I, TextType::Text_Can, TextType::Text_Climb, TextType::Text_And, TextType::Text_Am, TextType::Text_Purple, TextType::Text_And, TextType::Text_Am, TextType::Text_Blue, TextType::Text_And, TextType::Text_Am, TextType::Text_Red };
    std::deque<TextType> phrase3 = { TextType::Text_I, TextType::Text_Am, TextType::Text_Blue, TextType::Text_And, TextType::Text_Can, TextType::Text_Climb };
    std::deque<TextType> phrase4 = { TextType::Text_I, TextType::Text_Am, TextType::Text_Blue, TextType::Text_And, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Can, TextType::Text_Climb, TextType::Text_And, TextType::Text_Can, TextType::Text_Pull };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Valid);
}

TEST(ValidPhrases, ObjectIsProperties)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop };
    std::deque<TextType> phrase2{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop, TextType::Text_And, TextType::Text_Hot };
    std::deque<TextType> phrase3{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop, TextType::Text_And, TextType::Text_Is, TextType::Text_Hot };
    std::deque<TextType> phrase4{ TextType::Text_Purple, TextType::Text_And, TextType::Text_Blue, TextType::Text_Is, TextType::Text_Pull };
    std::deque<TextType> phrase5{ TextType::Text_Blue, TextType::Text_Is, TextType::Text_Goal };
    std::deque<TextType> phrase6{ TextType::Text_Purple, TextType::Text_And, TextType::Text_Blue, TextType::Text_And, TextType::Text_Yellow, TextType::Text_Is, TextType::Text_Goal };
    std::deque<TextType> phrase7{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Push,
        TextType::Text_And,
        TextType::Text_Pull,
        TextType::Text_And,
        TextType::Text_Stop
    };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase7)), Parser::ParseResult::Valid);
}

TEST(ValidPhrases, ObjectIsObjects)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Purple, TextType::Text_Is, TextType::Text_Grey };
    std::deque<TextType> phrase2{ TextType::Text_Purple, TextType::Text_Is, TextType::Text_Blue };
    std::deque<TextType> phrase3{ TextType::Text_Purple, TextType::Text_Is, TextType::Text_Green };
    std::deque<TextType> phrase4{ TextType::Text_Purple, TextType::Text_And, TextType::Text_Blue, TextType::Text_Is, TextType::Text_Grey };
    std::deque<TextType> phrase5{ TextType::Text_Word, TextType::Text_Is, TextType::Text_Word };
    std::deque<TextType> phrase6{ TextType::Text_Word, TextType::Text_Is, TextType::Text_Green };
    std::deque<TextType> phrase7{ TextType::Text_Red, TextType::Text_And, TextType::Text_Green, TextType::Text_Is, TextType::Text_Blue };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase7)), Parser::ParseResult::Valid);
}

TEST(ValidPhrases, ObjectCan)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push };
    std::deque<TextType> phrase2{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Pull };
    std::deque<TextType> phrase3{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Pull };
    std::deque<TextType> phrase4{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Can, TextType::Text_Pull };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Valid);
}

TEST(ValidPhrases, ObjectIsCan)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{
        TextType::Text_Word,
        TextType::Text_Can,
        TextType::Text_Push,
        TextType::Text_And,
        TextType::Text_Is,
        TextType::Text_Green
    };
    std::deque<TextType> phrase2{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Green,
        TextType::Text_And,
        TextType::Text_Can,
        TextType::Text_Push
    };
    std::deque<TextType> phrase3{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Green,
        TextType::Text_And,
        TextType::Text_Can,
        TextType::Text_Push,
        TextType::Text_And,
        TextType::Text_Pull
    };
    std::deque<TextType> phrase4{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Green,
        TextType::Text_And,
        TextType::Text_Can,
        TextType::Text_Push,
        TextType::Text_And,
        TextType::Text_Can,
        TextType::Text_Pull
    };
    std::deque<TextType> phrase5{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Pull,
        TextType::Text_And,
        TextType::Text_Can,
        TextType::Text_Pull
    };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Valid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Valid);
}

TEST(InvalidPhrases, ObjectIs)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);
    std::deque<TextType> phrase1{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Green,
        TextType::Text_And,
        TextType::Text_Blue
    };
    std::deque<TextType> phrase2{
        TextType::Text_Red,
        TextType::Text_Is,
        TextType::Text_Blue,
        TextType::Text_And,
        TextType::Text_Green
    };
    std::deque<TextType> phrase3{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Pull,
        TextType::Text_And,
        TextType::Text_Pull,
        TextType::Text_Can
    };
    std::deque<TextType> phrase4{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Pull,
        TextType::Text_And,
        TextType::Text_Pull,
        TextType::Text_Can,
        TextType::Text_Blue
    };
    std::deque<TextType> phrase5{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Pull,
        TextType::Text_And,
        TextType::Text_Pull,
        TextType::Text_Can,
        TextType::Text_Blue,
        TextType::Text_Is
    };
    std::deque<TextType> phrase6{
        TextType::Text_Word,
        TextType::Text_Is,
        TextType::Text_Pull,
        TextType::Text_And,
        TextType::Text_Pull,
        TextType::Text_Can,
        TextType::Text_Blue,
        TextType::Text_Is,
        TextType::Text_Goal
    };
    std::deque<TextType> phrase7{
        TextType::Text_Purple,
        TextType::Text_And,
        TextType::Text_Red,
        TextType::Text_Can,
        TextType::Text_Push,
        TextType::Text_Is,
        TextType::Text_Word
    };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase7)), Parser::ParseResult::Invalid);
}

TEST(InvalidPhrases, ObjectCan)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Word };
    std::deque<TextType> phrase2{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Green };
    std::deque<TextType> phrase3{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Green };
    std::deque<TextType> phrase4{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Can, TextType::Text_Green };
    std::deque<TextType> phrase5{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Is, TextType::Text_Push };
    std::deque<TextType> phrase6{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push, TextType::Text_Is, TextType::Text_Pull };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Invalid);
}

TEST(IncompletePhrases, ObjectIs)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Word, TextType::Text_Is };
    std::deque<TextType> phrase2{ TextType::Text_Word, TextType::Text_Is, TextType::Text_Push, TextType::Text_And };
    std::deque<TextType> phrase3{ TextType::Text_Word, TextType::Text_Is, TextType::Text_Push, TextType::Text_And, TextType::Text_Is };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Incomplete);
}

TEST(IncompletePhrases, ObjectCan)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Word, TextType::Text_Can };
    std::deque<TextType> phrase2{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push, TextType::Text_And };
    std::deque<TextType> phrase3{ TextType::Text_Word, TextType::Text_Can, TextType::Text_Push, TextType::Text_And, TextType::Text_Can };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Incomplete);
}

TEST(IncompletePhrases, IAmCan)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_I };
    std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_Am };
    std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And };

    std::deque<TextType> phrase10{ TextType::Text_I, TextType::Text_Can };
    std::deque<TextType> phrase11{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push, TextType::Text_And };

    std::deque<TextType> phrase20{ TextType::Text_Wall };
    std::deque<TextType> phrase21{ TextType::Text_Wall, TextType::Text_Is };
    std::deque<TextType> phrase22{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop, TextType::Text_And };
    std::deque<TextType> phrase23{ TextType::Text_Wall, TextType::Text_Is, TextType::Text_Stop, TextType::Text_And, TextType::Text_Is };

    std::deque<TextType> phrase30{ TextType::Text_Purple, TextType::Text_And };
    std::deque<TextType> phrase31{ TextType::Text_Purple, TextType::Text_And, TextType::Text_Blue };
    std::deque<TextType> phrase32{ TextType::Text_Purple, TextType::Text_And, TextType::Text_Blue, TextType::Text_Is };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Incomplete);

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase10)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase11)), Parser::ParseResult::Incomplete);

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase20)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase21)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase22)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase23)), Parser::ParseResult::Incomplete);

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase30)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase31)), Parser::ParseResult::Incomplete);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase32)), Parser::ParseResult::Incomplete);
}

TEST(InvalidPhrases, IAmIsCan)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Red, TextType::Text_I, TextType::Text_Am };
    std::deque<TextType> phrase2{ TextType::Text_Red, TextType::Text_Am, TextType::Text_I };
    std::deque<TextType> phrase3{ TextType::Text_I, TextType::Text_Am, TextType::Text_I };
    std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Is, TextType::Text_I };
    std::deque<TextType> phrase5{ TextType::Text_I, TextType::Text_Is, TextType::Text_Red };
    std::deque<TextType> phrase6{ TextType::Text_Red, TextType::Text_Is, TextType::Text_I };
    std::deque<TextType> phrase7{ TextType::Text_Red, TextType::Text_Am, TextType::Text_Red };
    std::deque<TextType> phrase8{ TextType::Text_I, TextType::Text_Am, TextType::Text_Pull };
    std::deque<TextType> phrase9{ TextType::Text_I, TextType::Text_Is, TextType::Text_Pull };
    std::deque<TextType> phrase10{ TextType::Text_I, TextType::Text_Can, TextType::Text_Red };
    std::deque<TextType> phrase11{ TextType::Text_I, TextType::Text_Can, TextType::Text_Hot };
    std::deque<TextType> phrase12{ TextType::Text_I, TextType::Text_Is, TextType::Text_Hot };

    std::deque<TextType> phrase20{ TextType::Text_I, TextType::Text_Can, TextType::Text_Blue };
    std::deque<TextType> phrase21{ TextType::Text_I, TextType::Text_Can, TextType::Text_Push, TextType::Text_Blue };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase7)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase8)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase9)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase10)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase11)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase12)), Parser::ParseResult::Invalid);

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase20)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase21)), Parser::ParseResult::Invalid);
}

TEST(InvalidPhrases, Nonsense)
{
    using namespace components;
    using namespace systems::parser;

    Parser parser(true, false);

    std::deque<TextType> phrase1{ TextType::Text_Push, TextType::Text_And, TextType::Text_Climb };
    std::deque<TextType> phrase2{ TextType::Text_I, TextType::Text_And, TextType::Text_Red, TextType::Text_Can, TextType::Text_Climb };
    std::deque<TextType> phrase3{ TextType::Text_Climb, TextType::Text_Is, TextType::Text_Green };
    std::deque<TextType> phrase4{ TextType::Text_I, TextType::Text_Am, TextType::Text_Purple, TextType::Text_Green };
    std::deque<TextType> phrase5{ TextType::Text_I, TextType::Text_Am, TextType::Text_Green, TextType::Text_And, TextType::Text_Grey, TextType::Text_Yellow };
    std::deque<TextType> phrase6{ TextType::Text_Red, TextType::Text_Is, TextType::Text_Can, TextType::Text_Pull };
    std::deque<TextType> phrase7{ TextType::Text_I, TextType::Text_Am, TextType::Text_Red, TextType::Text_And, TextType::Text_Blue, TextType::Text_Can, TextType::Text_Push };
    std::deque<TextType> phrase8{ TextType::Text_Is, TextType::Text_Pull };

    EXPECT_EQ(parser.parse(transformToParserQueue(phrase1)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase2)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase3)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase4)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase5)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase6)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase7)), Parser::ParseResult::Invalid);
    EXPECT_EQ(parser.parse(transformToParserQueue(phrase8)), Parser::ParseResult::Invalid);
}
