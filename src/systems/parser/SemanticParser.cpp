#include "SemanticParser.hpp"

#include "components/Object.hpp"

#include <cassert>
#include <ranges>

namespace systems::parser
{
    // ------------------------------------------------------------------
    //
    // We know (ha, famous last words) we have a gramatically correct phrase,
    // so we can be confident in the order of things.
    //
    // ------------------------------------------------------------------
    SemanticParser::SemanticRuleSet SemanticParser::parse(std::deque<Parser::PhrasePair>& phrase)
    {
        using namespace components;

        // Two possibilities, and "I" phrase or an "Object" phrase
        if (phrase.front().word == TextType::Text_I)
        {
            return parseIPhrase(phrase);
        }
        else
        {
            return parseObjectPhrase(phrase);
        }
    }

    //
    // NOTE: Could I use a stack-based approach to parse this information, like an expression parser?
    //
    SemanticParser::SemanticRuleSet SemanticParser::parseIPhrase(std::deque<Parser::PhrasePair>& phrase)
    {
        using namespace components;

        SemanticParser::SemanticRuleSet rules;

        // By definition, the first word is the subject
        rules.insert({ components::TextTypeToNounType.at(phrase.front().word), {} });
        phrase.pop_front();

        bool isNoun{ false };
        bool isAbility{ false };
        while (!phrase.empty())
        {
            // Next words could be 'am', 'can' or an noun/ability
            if (phrase.front().word == TextType::Text_Am)
            {
                isNoun = true;
                isAbility = false;

                phrase.pop_front();
                rules[NounType::I].insert(TextTypeToNounType.at(phrase.front().word));
            }
            else if (phrase.front().word == TextType::Text_Can)
            {
                isAbility = true;
                isNoun = false;

                phrase.pop_front();
                rules[NounType::I].insert(TextTypeToAbilityType.at(phrase.front().word));
            }

            if (isNoun)
            {
                assert(Object::isNoun(phrase.front().word));

                rules[NounType::I].insert(TextTypeToNounType.at(phrase.front().word));
                phrase.pop_front();
            }
            else if (isAbility)
            {
                assert(Object::isPropAbility(phrase.front().word));

                rules[NounType::I].insert(TextTypeToAbilityType.at(phrase.front().word));
                phrase.pop_front();
            }

            if (!phrase.empty())
            {
                // Phrase could be...
                //  * and - just remove the 'and'
                //  * and can - just remove the 'and'
                //  * and I - remove both 'and' and 'I'
                assert(phrase.front().word == TextType::Text_And);
                phrase.pop_front();

                assert(!phrase.empty());
                if (phrase.front().word == TextType::Text_I)
                {
                    phrase.pop_front();
                }
            }
        }

        return rules;
    }

    SemanticParser::SemanticRuleSet SemanticParser::parseObjectPhrase(std::deque<Parser::PhrasePair>& phrase)
    {
        using namespace components;

        SemanticParser::SemanticRuleSet rules;

        // By definition, the first word is the subject
        rules.insert({ components::TextTypeToNounType.at(phrase.front().word), {} });
        phrase.pop_front();

        bool isSubject{ true };
        bool isNounOrProperty{ false };
        bool isAbility{ false };
        while (!phrase.empty())
        {
            // Next words could be: 'and', 'is', or 'can'
            if (isSubject && phrase.front().word == TextType::Text_And)
            {
                isSubject = true;
                isNounOrProperty = false;
                isAbility = false;

                phrase.pop_front();
                rules.insert({ components::TextTypeToNounType.at(phrase.front().word), {} });
                phrase.pop_front();
            }
            else if (phrase.front().word == TextType::Text_Is)
            {
                isSubject = false;
                isAbility = false;
                isNounOrProperty = true;

                phrase.pop_front();
            }
            else if (phrase.front().word == TextType::Text_Can)
            {
                isSubject = false;
                isNounOrProperty = false;
                isAbility = true;

                phrase.pop_front();
            }

            if (!phrase.empty() && isNounOrProperty)
            {
                // We add this to all of the subjects
                if (Object::isNoun(phrase.front().word))
                {
                    for (auto&& targets : rules | std::views::values)
                    {
                        targets.insert(TextTypeToNounType.at(phrase.front().word));
                    }
                    phrase.pop_front();
                }
                else if (Object::isPropAbility(phrase.front().word))
                {
                    for (auto&& targets : rules | std::views::values)
                    {
                        targets.insert(TextTypeToPropertyType.at(phrase.front().word));
                    }
                    phrase.pop_front();
                }
            }
            else if (isAbility)
            {
                for (auto&& targets : rules | std::views::values)
                {
                    targets.insert(TextTypeToAbilityType.at(phrase.front().word));
                }
                phrase.pop_front();
            }

            if (!isSubject && !phrase.empty())
            {
                // Phrase could be...
                // * and - just remove the 'and'
                // * and is - just remove the 'and'
                // * and has - just remove the 'and;
                assert(phrase.front().word == TextType::Text_And);
                phrase.pop_front();
            }
        }

        return rules;
    }
} // namespace systems::parser