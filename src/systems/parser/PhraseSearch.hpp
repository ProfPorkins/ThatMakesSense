#pragma once

#include "Level.hpp"
#include "Parser.hpp"
#include "components/Object.hpp"
#include "components/PhraseDirection.hpp"
#include "entities/Entity.hpp"

#include <queue>
#include <tuple>
#include <unordered_map>
#include <vector>

namespace systems::parser
{
    class PhraseSearch
    {
      public:
        PhraseSearch() = default;
        std::vector<std::deque<Parser::PhrasePair>> search(const Level& level, components::PhraseDirection::DirectionGrid& gridDirection);

      private:
        Parser m_parser{ false, false };
        std::vector<std::deque<Parser::PhrasePair>> m_phrases;
        std::uint16_t m_nextPhraseId{ 0 };
        // We use this array to know if we have visited the location during the recursive traversal
        // to find groups of words.
        std::vector<std::vector<bool>> m_visited;
        // This array is used to hold just the TextType enums which is needed in searching for
        // neighboring words during the phrase search.
        std::vector<std::vector<components::TextType>> m_gridWords;

        void collectGroup(const Level& level, const misc::HexCoord& position, entities::EntityVector& group);
        std::uint8_t getTextCount(const Level& level, const misc::HexCoord& position);
        bool isStartWord(misc::HexCoord position);
        void findPhrases(std::deque<Parser::PhrasePair> phrase, const misc::HexCoord& location, const Level& level, components::PhraseDirection::DirectionGrid& m_gridDirection, std::unordered_set<misc::HexCoord> visitedGroup);
        void sortGroupByLocation(entities::EntityVector& group);
    };
} // namespace systems::parser