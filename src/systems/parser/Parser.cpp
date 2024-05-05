#include "Parser.hpp"

#include <iostream>
#include <optional>

namespace systems::parser
{
    std::string phraseToString(std::deque<Parser::PhrasePair> phrase)
    {
        std::string message;
        for (std::size_t word = 0; word < phrase.size(); word++)
        {
            message += (components::TextTypeToString.at(phrase[word].word) + " ");
        }

        return message;
    }

    // ------------------------------------------------------------------
    //
    // S ->
    //       I_STMT
    //     | O_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parse(std::deque<PhrasePair> phrase)
    {
        try
        {
            if (m_reportPhrase)
            {
                std::cout << phraseToString(phrase) << std::endl;
            }

            nextSymbol(phrase);

            auto wordCount = phrase.size();
            if (auto result = parseI_STMT(phrase); result == ParseResult::Valid || result == ParseResult::Incomplete)
            {
                return result;
            }

            // If the number of words in the phrase has changed, it means we started down an "I" path,
            // therefore, the "Object" path can't possibly be correct.
            if (wordCount == phrase.size())
            {
                return parseO_STMT(phrase);
            }
        }
        catch (...)
        {
        }

        return ParseResult::Invalid;
    }

    bool Parser::isObject(components::TextType s)
    {
        // Unfortunately, due to my bad design, have to do this test as greater than Text_I instead of Before_Noun
        return s > components::TextType::Text_I && s < components::TextType::After_Noun;
    }

    // Ugh, these are fragile to changes
    bool Parser::isProperty(components::TextType s)
    {
        return s == components::TextType::Text_Goal ||
               s == components::TextType::Text_Push ||
               s == components::TextType::Text_Pull ||
               s == components::TextType::Text_Stop ||
               s == components::TextType::Text_Steep ||
               s == components::TextType::Text_Water ||
               s == components::TextType::Text_Hot;
    }

    bool Parser::isAbility(components::TextType s)
    {
        return s == components::TextType::Text_Climb ||
               s == components::TextType::Text_Float ||
               s == components::TextType::Text_Push ||
               s == components::TextType::Text_Pull ||
               s == components::TextType::Text_Chill ||
               s == components::TextType::Text_Send;
    }

    bool Parser::isVerb(components::TextType s)
    {
        return s > components::TextType::Before_Verb && s < components::TextType::After_Verb;
    }

    void Parser::nextSymbol(std::deque<PhrasePair>& phrase)
    {
        if (!phrase.empty())
        {
            previous = current;
            current = phrase.front().word;
            phrase.pop_front();
        }
        else
        {
            previous = current;
            current = std::nullopt;
        }
    }

    bool Parser::accept(std::deque<PhrasePair>& phrase, components::TextType s)
    {
        if (current.has_value() && current.value() == s)
        {
            nextSymbol(phrase);
            return true;
        }

        return false;
    }

    bool Parser::expect(std::deque<PhrasePair>& phrase, components::TextType s)
    {
        if (accept(phrase, s))
        {
            return true;
        }

        if (m_reportSyntaxError)
        {
            std::cout << "unexpected components::TextType: " << components::TextTypeToString.at(s) << std::endl;
        }
        return false;
    }

    // ------------------------------------------------------------------
    //
    // I_STMT -> i I_OR_OBJECT_ABILITY.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_STMT(std::deque<PhrasePair>& phrase)
    {
        if (accept(phrase, components::TextType::Text_I))
        {
            return parseI_OR_OBJECT_ABILITY(phrase);
        }

        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // O_STMT ->
    //        OBJECT and OBJECT IS_OBJECT_OR_PROPERTY
    //      | OBJECT and OBJECT_VERB_ABILITY_PHRASE
    //      | OBJECT IS_OBJECT_OR_PROPERTY
    //      | OBJECT OBJECT_VERB_ABILITY_PHRASE.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseO_STMT(std::deque<PhrasePair>& phrase)
    {
        if (current.has_value() && isObject(current.value()))
        {
            expect(phrase, current.value());
            if (!current.has_value())
            {
                return ParseResult::Incomplete;
            }
            // Special case:  If there is a double verb, it is invalid, no matter what
            // Example: word can is push
            if (current.has_value() && !phrase.empty() && isVerb(current.value()) && isVerb(phrase.front().word))
            {
                return ParseResult::Invalid;
            }
            // We are going to peek ahead to decide which path, if any, we should go down
            if (current.has_value())
            {
                if (current.value() == components::TextType::Text_Can)
                {
                    if (auto result = parseOBJECT_VERB_ABILITY_PHRASE(phrase); result == ParseResult::Valid || result == ParseResult::Incomplete)
                    {
                        return result;
                    }
                }
                else if (current.value() == components::TextType::Text_Is)
                {
                    if (auto result = parseIS_OBJECT_OR_PROPERTY(phrase); result == ParseResult::Valid || result == ParseResult::Incomplete)
                    {
                        // Turns out, if we say the phrase is valid, but there are still words left in the phrase, it is actually
                        // invalid because of that.  The valid part of the phrase will get discovered correctly in the phrase
                        // search with the shorter phrase.
                        if (result == ParseResult::Valid && (current.has_value() || phrase.size() > 0))
                        {
                            return ParseResult::Invalid;
                        }
                        return result;
                    }
                }
            }

            // We can 'and' any number of objects as part of the phrase
            while (accept(phrase, components::TextType::Text_And))
            {
                if (!current.has_value())
                {
                    return ParseResult::Incomplete;
                }
                if (current.has_value() && isObject(current.value()))
                {
                    expect(phrase, current.value());
                    // We need to make a copy of the phrase right now, because if the first condition comes back invalid
                    // then need to start over from where we just were.
                    auto phraseBeforeVerbAbility = phrase;
                    auto currentBeforeVerbAbility = current;
                    if (auto result = parseOBJECT_VERB_ABILITY_PHRASE(phrase); result == ParseResult::Valid || result == ParseResult::Incomplete)
                    {
                        return result;
                    }
                    phrase = phraseBeforeVerbAbility;
                    current = currentBeforeVerbAbility;
                    if (auto result = parseIS_OBJECT_OR_PROPERTY(phrase); result == ParseResult::Valid || result == ParseResult::Incomplete)
                    {
                        return result;
                    }
                }
            }
        }

        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_OR_OBJECT_ABILITY ->
    //        I_VERB_OBJECT_PHRASE
    //      | I_VERB_ABILITY_PHRASE.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_OR_OBJECT_ABILITY(std::deque<PhrasePair>& phrase)
    {
        auto originalSize = phrase.size();
        if (auto result1 = parseI_VERB_OBJECT_PHRASE(phrase); result1 == ParseResult::Valid || result1 == ParseResult::Incomplete)
        {
            return result1;
        }
        // If the size hasn't changed, no problem give this a try
        // If the size has changed, the previous word must have been an 'and'
        else if (originalSize == phrase.size() || (previous.has_value() && previous.value() == components::TextType::Text_And))
        {
            if (auto result2 = parseI_VERB_ABILITY_PHRASE(phrase); result2 == ParseResult::Valid || result2 == ParseResult::Incomplete)
            {
                return result2;
            }
        }

        if (!current.has_value() && phrase.empty())
        {
            return ParseResult::Incomplete;
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_OR_OBJECT_ABILITY" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_VERB_OBJECT_PHRASE -> am I_OBJECT_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_VERB_OBJECT_PHRASE(std::deque<PhrasePair>& phrase)
    {
        if (accept(phrase, components::TextType::Text_Am))
        {
            return parseI_OBJECT_STMT(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_VERB_OBJECT_PHRASE" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_OBJECT_STMT -> OBJECT I_AND_OBJECT_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_OBJECT_STMT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value() && phrase.empty())
        {
            return ParseResult::Incomplete;
        }

        if (isObject(current.value()))
        {
            if (expect(phrase, current.value()))
            {
                return parseI_AND_OBJECT_STMT(phrase);
            }
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_OBJECT_STMT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_AND_OBJECT_STMT ->
    //      | and I_AND_STMT_OR_OBJECT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_AND_OBJECT_STMT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Valid;
        }
        else if (accept(phrase, components::TextType::Text_And))
        {
            return parseI_AND_STMT_OR_OBJECT(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_AND_OBJECT_STMT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_AND_STMT_OR_OBJECT ->
    //        I_STMT
    //      | OBJECT
    //      | am OBJECT I_AND_OBJECT_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_AND_STMT_OR_OBJECT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }

        if (accept(phrase, components::TextType::Text_I))
        {
            return parseI_STMT(phrase);
        }
        else if (isObject(current.value()))
        {
            // TODO: Check for incomplete
            expect(phrase, current.value());
            if (!current.has_value() && phrase.size() == 0)
            {
                return ParseResult::Valid;
            }
            else // This is basically an LL(2) peek to get left-recursion, rather than fitting the LL(1) grammar
            {
                if (accept(phrase, components::TextType::Text_And))
                {
                    return parseI_AND_STMT_OR_OBJECT(phrase);
                }
                else
                {
                    return ParseResult::Invalid;
                }
            }
        }
        else if (accept(phrase, components::TextType::Text_Am))
        {
            if (isObject(current.value()))
            {
                expect(phrase, current.value());
                return parseI_AND_OBJECT_STMT(phrase);
            }
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_AND_STMT_OR_OBJECT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_VERB_ABILITY_PHRASE -> can I_ABILITY_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_VERB_ABILITY_PHRASE(std::deque<PhrasePair>& phrase)
    {
        if (accept(phrase, components::TextType::Text_Can))
        {
            return parseI_ABILITY_STMT(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_VERB_ABILITY_PHRASE" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_ABILITY_STMT -> ABILITY I_AND_ABILITY_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_ABILITY_STMT(std::deque<PhrasePair>& phrase)
    {
        if (current.has_value() && isAbility(current.value()))
        {
            if (expect(phrase, current.value()))
            {
                return parseI_AND_ABILITY_STMT(phrase);
            }
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_ABILITY_STMT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_AND_ABILITY_STMT ->
    //      | and I_AND_STMT_OR_ABILITY.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_AND_ABILITY_STMT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Valid;
        }
        else if (accept(phrase, components::TextType::Text_And))
        {
            return parseI_AND_STMT_OR_ABILITY(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_AND_ABILITY_STMT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // I_AND_STMT_OR_ABILITY ->
    //        I_STMT
    //      | ABILITY
    //      | I_VERB_OBJECT_PHRASE
    //      | I_VERB_ABILITY_PHRASE.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseI_AND_STMT_OR_ABILITY(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value() && phrase.empty())
        {
            return ParseResult::Incomplete;
        }

        if (auto result1 = parseI_STMT(phrase); result1 == ParseResult::Valid || result1 == ParseResult::Incomplete)
        {
            return result1;
        }
        else if (isAbility(current.value()))
        {
            // TODO: Check for incomplete
            if (expect(phrase, current.value()))
            {
                if (!current.has_value() && phrase.size() == 0)
                {
                    return ParseResult::Valid;
                }
                else // This is basically an LL(2) peek to get left-recursion, rather than fitting the LL(1) grammar
                {
                    if (accept(phrase, components::TextType::Text_And))
                    {
                        return parseI_AND_STMT_OR_ABILITY(phrase);
                    }
                    else
                    {
                        return ParseResult::Invalid;
                    }
                }
            }
            else
            {
                return ParseResult::Invalid;
            }
        }
        else if (auto result2 = parseI_VERB_OBJECT_PHRASE(phrase); result2 == ParseResult::Valid || result2 == ParseResult::Incomplete)
        {
            return result2;
        }
        else if (auto result3 = parseI_VERB_ABILITY_PHRASE(phrase); result3 == ParseResult::Valid || result3 == ParseResult::Incomplete)
        {
            return result3;
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at I_AND_STMT_OR_ABILITY" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // IS_OBJECT_OR_PROPERTY -> is OBJECT_OR_PROPERTY.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseIS_OBJECT_OR_PROPERTY(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }

        if (accept(phrase, components::TextType::Text_Is))
        {
            return parseOBJECT_OR_PROPERTY(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at IS_OBJECT_OR_PROPERTY" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // OBJECT_OR_PROPERTY ->
    //       OBJECT AND_IS_PROPERTY
    //     | PROPERTY AND_PROPERTY
    //     | OBJECT_VERB_ABILITY_PHRASE.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseOBJECT_OR_PROPERTY(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }

        if (isObject(current.value()))
        {
            expect(phrase, current.value());
            // We have to peed ahead to see which parsing path to follow
            if (!current.has_value())
            {
                return ParseResult::Valid;
            }
            if (phrase.size() == 0 && current.value() == components::TextType::Text_And)
            {
                return ParseResult::Incomplete;
            }
            if (phrase.size() > 0 && current.value() == components::TextType::Text_And)
            {
                if (isProperty(phrase.front().word))
                {
                    return parseAND_IS_PROPERTY(phrase);
                }
                else
                {
                    expect(phrase, current.value()); // by definition this is TextType::Text_And
                    return parseOBJECT_VERB_ABILITY_PHRASE(phrase);
                }
            }
        }
        else if (isProperty(current.value()))
        {
            expect(phrase, current.value());
            // We have to peek and to see if there is an 'and' only or an 'and can' to determine which phrase to parse
            if (current.has_value() && phrase.size() > 0)
            {
                if (current.value() == components::TextType::Text_And && phrase.front().word == components::TextType::Text_Can)
                {
                    expect(phrase, components::TextType::Text_And);
                    return parseOBJECT_VERB_ABILITY_PHRASE(phrase);
                }
                else
                {
                    // We'll just try this, it'll fail property in the case current is an 'and'
                    return parseAND_PROPERTY(phrase);
                }
            }
            else
            {
                // When the if statement fails, it can't possibly be OBJECt_VERB_ABILITY_PHRASE, so just test AND_PROPORTY
                return parseAND_PROPERTY(phrase);
            }
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at OBJECT_OR_PROPERTY" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // OBJECT_VERB_ABILITY_PHRASE -> can OBJECT_ABILITY_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseOBJECT_VERB_ABILITY_PHRASE(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }
        if (accept(phrase, components::TextType::Text_Can))
        {
            return parseOBJECT_ABILITY_STMT(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at OBJECT_VERB_ABILITY_PHRASE" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // OBJECT_ABILITY_STMT -> ABILITY OBJECT_AND_ABILITY_STMT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseOBJECT_ABILITY_STMT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }
        if (isAbility(current.value()))
        {
            expect(phrase, current.value());
            return parseOBJECT_AND_ABILITY_STMT(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at OBJECT_ABILITY_STMT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // OBJECT_AND_ABILITY_STMT ->
    //      | and OBJECT_AND_STMT_OR_ABILITY.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseOBJECT_AND_ABILITY_STMT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Valid;
        }
        if (accept(phrase, components::TextType::Text_And))
        {
            return parseOBJECT_AND_STMT_OR_ABILITY(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at OBJECT_ABILITY_STMT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // OBJECT_AND_STMT_OR_ABILITY ->
    //        O_STMT
    //      | ABILITY
    //      | OBJECT_VERB_ABILITY_PHRASE.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseOBJECT_AND_STMT_OR_ABILITY(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }
        if (isAbility(current.value()))
        {
            expect(phrase, current.value());
            if (!current.has_value() && phrase.size() == 0)
            {
                return ParseResult::Valid;
            }
            else // This is basically an LL(2) peek to get left-recursion, rather than fitting the LL(1) grammar
            {
                if (accept(phrase, components::TextType::Text_And))
                {
                    return parseOBJECT_AND_STMT_OR_ABILITY(phrase);
                }
                else
                {
                    return ParseResult::Invalid;
                }
            }
        }

        if (auto result = parseOBJECT_VERB_ABILITY_PHRASE(phrase); result == ParseResult::Valid || result == ParseResult::Incomplete)
        {
            return result;
        }

        if (current.has_value() && accept(phrase, components::TextType::Text_Is))
        {
            if (!current.has_value())
            {
                return ParseResult::Incomplete;
            }
            if (isObject(current.value()))
            {
                expect(phrase, current.value());
                return parseOBJECT_AND_ABILITY_STMT(phrase);
            }
        }

        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // AND_IS_OBJECT ->
    //      | and IS_OBJECT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseAND_IS_OBJECT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Valid;
        }
        else if (accept(phrase, components::TextType::Text_And))
        {
            return parseIS_OBJECT(phrase);
        }
        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at IS_OBJECT" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // AND_IS_PROPERTY ->
    //      | and IS_PROPERTY.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseAND_IS_PROPERTY(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Valid;
        }
        else if (accept(phrase, components::TextType::Text_And))
        {
            return parseIS_PROPERTY(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at IS_PROPERTY" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // IS_PROPERTY->
    //        PROPERTY
    //      | is PROPERTY.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseIS_PROPERTY(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }
        if (isProperty(current.value()))
        {
            expect(phrase, current.value());
            if (current.has_value() && current.value() == components::TextType::Text_And)
            {
                return parseAND_PROPERTY(phrase);
            }
            else
            {
                return ParseResult::Valid;
            }
        }
        else if (accept(phrase, components::TextType::Text_Is))
        {
            // Leaving this here as a reminder of how it was originally coded
            // return isProperty(current.value()) ? ParseResult::Valid : ParseResult::Invalid;
            return parseIS_PROPERTY(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at IS_PROPERTY" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // AND_PROPERTY->
    //      | and IS_OBJECT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseAND_PROPERTY(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Valid;
        }
        else if (accept(phrase, components::TextType::Text_And))
        {
            return parseIS_PROPERTY(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at AND_PROPERTY" << std::endl;
        }
        return ParseResult::Invalid;
    }

    // ------------------------------------------------------------------
    //
    // IS_OBJECT ->
    //        OBJECT
    //      | is OBJECT.
    //
    // ------------------------------------------------------------------
    Parser::ParseResult Parser::parseIS_OBJECT(std::deque<PhrasePair>& phrase)
    {
        if (!current.has_value())
        {
            return ParseResult::Incomplete;
        }
        if (isObject(current.value()))
        {
            return ParseResult::Valid;
        }
        else if (accept(phrase, components::TextType::Text_Is))
        {
            // Leaving this here as a reminder of how it was originally coded
            // return isObject(current.value()) ? ParseResult::Valid : ParseResult::Invalid;
            return parseIS_OBJECT(phrase);
        }

        if (m_reportSyntaxError)
        {
            std::cout << "syntax error at IS_OBJECT" << std::endl;
        }
        return ParseResult::Invalid;
    }
} // namespace systems::parser