#pragma once

#include "Parser.hpp"
#include "components/Ability.hpp"
#include "components/Noun.hpp"
#include "components/Property.hpp"

#include <deque>
#include <unordered_map>
#include <unordered_set>
#include <variant>

namespace systems::parser
{
    // ------------------------------------------------------------------
    //
    // This class is used to take a phrase and extract the semantic information
    // from it.  e.g.  I am blue and can climb
    //  * Subject: I
    //  * Property: blue
    //  * Ability: climb
    //
    // ------------------------------------------------------------------
    class SemanticParser
    {
      public:
        using SemanticFunctionType = std::variant<components::NounType, components::AbilityType, components::PropertyType>;
        using SemanticRuleSet = std::unordered_map<components::NounType, std::unordered_set<SemanticFunctionType>>;

        SemanticParser() = default;

        SemanticRuleSet parse(std::deque<Parser::PhrasePair>& phrase);

      private:
        SemanticRuleSet parseIPhrase(std::deque<Parser::PhrasePair>& phrase);
        SemanticRuleSet parseObjectPhrase(std::deque<Parser::PhrasePair>& phrase);
    };
} // namespace systems::parser