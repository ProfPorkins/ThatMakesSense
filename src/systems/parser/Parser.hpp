#pragma once

#include "components/Object.hpp"
#include "misc/HexCoord.hpp"

#include <cstdint>
#include <deque>
#include <memory>
#include <optional>
#include <string>

namespace systems::parser
{
    class Parser
    {
      public:
        enum class ParseResult : std::uint8_t
        {
            Valid,
            Incomplete,
            Invalid
        };
        struct PhrasePair
        {
            components::TextType word;
            misc::HexCoord cell;
        };

        Parser(bool reportPhrase, bool reportSyntaxError) :
            m_reportPhrase(reportPhrase),
            m_reportSyntaxError(reportSyntaxError)
        {
        }

        ParseResult parse(std::deque<PhrasePair> phrase);

      private:
        bool m_reportPhrase{ false };
        bool m_reportSyntaxError{ false };
        std::optional<components::TextType> current{ std::nullopt };
        std::optional<components::TextType> previous{ std::nullopt };

        void nextSymbol(std::deque<PhrasePair>& phrase);
        bool accept(std::deque<PhrasePair>& phrase, components::TextType s);
        bool expect(std::deque<PhrasePair>& phrase, components::TextType s);

        bool isObject(components::TextType s);
        bool isProperty(components::TextType s);
        bool isAbility(components::TextType s);
        bool isVerb(components::TextType s);

        ParseResult parseI_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseO_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseI_OR_OBJECT_ABILITY(std::deque<PhrasePair>& phrase);
        ParseResult parseI_VERB_OBJECT_PHRASE(std::deque<PhrasePair>& phrase);
        ParseResult parseI_OBJECT_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseI_AND_OBJECT_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseI_AND_STMT_OR_OBJECT(std::deque<PhrasePair>& phrase);
        ParseResult parseI_VERB_ABILITY_PHRASE(std::deque<PhrasePair>& phrase);
        ParseResult parseI_ABILITY_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseI_AND_ABILITY_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseI_AND_STMT_OR_ABILITY(std::deque<PhrasePair>& phrase);
        ParseResult parseIS_OBJECT_OR_PROPERTY(std::deque<PhrasePair>& phrase);
        ParseResult parseOBJECT_OR_PROPERTY(std::deque<PhrasePair>& phrase);
        ParseResult parseOBJECT_VERB_ABILITY_PHRASE(std::deque<PhrasePair>& phrase);
        ParseResult parseOBJECT_ABILITY_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseOBJECT_AND_ABILITY_STMT(std::deque<PhrasePair>& phrase);
        ParseResult parseOBJECT_AND_STMT_OR_ABILITY(std::deque<PhrasePair>& phrase);
        ParseResult parseAND_IS_OBJECT(std::deque<PhrasePair>& phrase);
        ParseResult parseAND_IS_PROPERTY(std::deque<PhrasePair>& phrase);
        ParseResult parseIS_PROPERTY(std::deque<PhrasePair>& phrase);
        ParseResult parseAND_PROPERTY(std::deque<PhrasePair>& phrase);
        ParseResult parseIS_OBJECT(std::deque<PhrasePair>& phrase);
    };
} // namespace systems::parser