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

#include "systems/parser/SemanticParser.hpp"

#include <deque>
#include <gtest/gtest.h>

// --------------------------------------------------------------
//
//
//
// --------------------------------------------------------------
TEST(SemanticParser, IAmObjects)
{
    using namespace components;

    systems::parser::SemanticParser parser;

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 1);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Green, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 2);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Green), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Green, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Blue, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 3);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Green), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Blue), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Green, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 2);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Green), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Green, {} },
            { TextType::Text_And, {} },
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Blue, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 3);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Green), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Blue), true);
        }
    }
}

TEST(SemanticParser, ICanAbilities)
{
    using namespace components;

    systems::parser::SemanticParser parser;

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 1);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 2);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Climb, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 3);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Climb), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Pull, {} },
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 2);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Pull, {} },
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 2);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Pull, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Climb, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 3);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Climb), true);
        }
    }
}

TEST(SemanticParser, IAmObjectsAndCanAbilities)
{
    using namespace components;

    systems::parser::SemanticParser parser;

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 2);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 2);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Blue, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 3);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Blue, {} },
            { TextType::Text_And, {} },
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 3);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Blue, {} },
            { TextType::Text_And, {} },
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 4);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Blue, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 4);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_I, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} },
            { TextType::Text_And, {} },
            { TextType::Text_I, {} },
            { TextType::Text_Am, {} },
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Blue, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::I), true);
        if (rules.contains(NounType::I))
        {
            EXPECT_EQ(rules[NounType::I].size(), 4);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Purple), true);
            EXPECT_EQ(rules[NounType::I].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::I].contains(AbilityType::Pull), true);
        }
    }
}

TEST(SemanticParser, ObjectIsObject)
{
    using namespace components;

    systems::parser::SemanticParser parser;

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Red, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Blue, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Red), true);
        if (rules.contains(NounType::Red))
        {
            EXPECT_EQ(rules[NounType::Red].size(), 1);
            EXPECT_EQ(rules[NounType::Red].contains(NounType::Blue), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Red, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Yellow, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Blue, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 2);
        EXPECT_EQ(rules.contains(NounType::Red), true);
        EXPECT_EQ(rules.contains(NounType::Yellow), true);

        if (rules.contains(NounType::Red))
        {
            EXPECT_EQ(rules[NounType::Red].size(), 1);
            EXPECT_EQ(rules[NounType::Red].contains(NounType::Blue), true);
        }
        if (rules.contains(NounType::Yellow))
        {
            EXPECT_EQ(rules[NounType::Yellow].size(), 1);
            EXPECT_EQ(rules[NounType::Yellow].contains(NounType::Blue), true);
        }
    }
}

TEST(SemanticParser, ObjectIsProperty)
{
    using namespace components;

    systems::parser::SemanticParser parser;

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Wall, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Stop, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Wall), true);

        if (rules.contains(NounType::Wall))
        {
            EXPECT_EQ(rules[NounType::Wall].size(), 1);
            EXPECT_EQ(rules[NounType::Wall].contains(PropertyType::Stop), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Word, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Word), true);

        if (rules.contains(NounType::Word))
        {
            EXPECT_EQ(rules[NounType::Word].size(), 2);
            EXPECT_EQ(rules[NounType::Word].contains(PropertyType::Pushable), true);
            EXPECT_EQ(rules[NounType::Word].contains(PropertyType::Pullable), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Word, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Wall, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 2);
        EXPECT_EQ(rules.contains(NounType::Word), true);
        EXPECT_EQ(rules.contains(NounType::Wall), true);

        if (rules.contains(NounType::Word))
        {
            EXPECT_EQ(rules[NounType::Word].size(), 2);
            EXPECT_EQ(rules[NounType::Word].contains(PropertyType::Pushable), true);
            EXPECT_EQ(rules[NounType::Word].contains(PropertyType::Pullable), true);
        }
        if (rules.contains(NounType::Wall))
        {
            EXPECT_EQ(rules[NounType::Wall].size(), 2);
            EXPECT_EQ(rules[NounType::Wall].contains(PropertyType::Pushable), true);
            EXPECT_EQ(rules[NounType::Wall].contains(PropertyType::Pullable), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Purple, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Blue, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Yellow, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Goal, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 3);
        EXPECT_EQ(rules.contains(NounType::Purple), true);
        EXPECT_EQ(rules.contains(NounType::Blue), true);
        EXPECT_EQ(rules.contains(NounType::Yellow), true);

        if (rules.contains(NounType::Purple))
        {
            EXPECT_EQ(rules[NounType::Purple].size(), 1);
            EXPECT_EQ(rules[NounType::Purple].contains(PropertyType::Goal), true);
        }
        if (rules.contains(NounType::Blue))
        {
            EXPECT_EQ(rules[NounType::Blue].size(), 1);
            EXPECT_EQ(rules[NounType::Blue].contains(PropertyType::Goal), true);
        }
        if (rules.contains(NounType::Yellow))
        {
            EXPECT_EQ(rules[NounType::Yellow].size(), 1);
            EXPECT_EQ(rules[NounType::Yellow].contains(PropertyType::Goal), true);
        }
    }
}

TEST(SemanticParser, ObjectIsGoal)
{
    using namespace components;

    systems::parser::SemanticParser parser;

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Blue, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Goal, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Blue), true);
        if (rules.contains(NounType::Blue))
        {
            EXPECT_EQ(rules[NounType::Blue].size(), 1);
            EXPECT_EQ(rules[NounType::Blue].contains(PropertyType::Goal), true);
        }
    }
}

TEST(SemanticParser, ObjectCanAbility)
{
    using namespace components;

    systems::parser::SemanticParser parser;
    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Red, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Red), true);
        if (rules.contains(NounType::Red))
        {
            EXPECT_EQ(rules[NounType::Red].size(), 1);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Push), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Red, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Red), true);
        if (rules.contains(NounType::Red))
        {
            EXPECT_EQ(rules[NounType::Red].size(), 2);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Pull), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Red, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Blue, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Red), true);
        if (rules.contains(NounType::Red))
        {
            EXPECT_EQ(rules[NounType::Red].size(), 2);
            EXPECT_EQ(rules[NounType::Red].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Push), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Red, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Blue, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Pull, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Red), true);
        if (rules.contains(NounType::Red))
        {
            EXPECT_EQ(rules[NounType::Red].size(), 3);
            EXPECT_EQ(rules[NounType::Red].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Pull), true);
        }
    }

    {
        std::deque<systems::parser::Parser::PhrasePair> phrase{
            { TextType::Text_Red, {} },
            { TextType::Text_Is, {} },
            { TextType::Text_Blue, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Push, {} },
            { TextType::Text_And, {} },
            { TextType::Text_Can, {} },
            { TextType::Text_Pull, {} }
        };

        auto rules = parser.parse(phrase);
        EXPECT_EQ(rules.size(), 1);
        EXPECT_EQ(rules.contains(NounType::Red), true);
        if (rules.contains(NounType::Red))
        {
            EXPECT_EQ(rules[NounType::Red].size(), 3);
            EXPECT_EQ(rules[NounType::Red].contains(NounType::Blue), true);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Push), true);
            EXPECT_EQ(rules[NounType::Red].contains(AbilityType::Pull), true);
        }
    }
}
