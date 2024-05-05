#include "PhraseSearch.hpp"

#include "components/Position.hpp"

#include <cassert>
#include <ranges>

namespace systems::parser
{
    // ------------------------------------------------------------------
    //
    // Searches a level for valid phrases, return a vector of valid phrases
    //
    // gridDirectiion: Used to hold the directions phrases traverse through the cells.
    //
    // ------------------------------------------------------------------
    std::vector<std::deque<Parser::PhrasePair>> PhraseSearch::search(const Level& level, components::PhraseDirection::DirectionGrid& gridDirection)
    {
        m_visited.clear();
        m_gridWords.clear();
        gridDirection.clear();

        m_visited.resize(level.getHeight());
        m_gridWords.resize(level.getHeight());
        gridDirection.resize(level.getHeight());
        for (auto&& row : m_visited)
        {
            row.resize(level.getWidth(), false);
        }
        for (auto&& row : m_gridWords)
        {
            row.resize(level.getWidth(), components::TextType::None);
        }
        for (auto&& row : gridDirection)
        {
            row.resize(level.getWidth());
        }

        // Step 1: Find all groups of words
        // This performs top to bottom, left to right search.  It searches row by row, to find
        // the start of a group, then collects the group, sorts the group by location and then
        // phrases are parsed from each group.
        for (misc::HexCoord::Type r = 0; r < level.getHeight(); r++)
        {
            for (misc::HexCoord::Type q = 0; q < level.getWidth(); q++)
            {
                auto position = misc::HexCoord{ q, r };
                if (!m_visited[r][q] && getTextCount(level, position) == 1)
                {
                    entities::EntityVector group;
                    collectGroup(level, position, group);
                    sortGroupByLocation(group);

                    // Place the words from these entities into our gridWords
                    for (auto&& entity : group)
                    {
                        auto location = entity->getComponent<components::Position>()->get();
                        m_gridWords[location.r][location.q] = entity->getComponent<components::Object>()->getText();
                    }

                    // Step 2: Find all phrase start words
                    for (auto&& entity : group)
                    {
                        // Step 3: Using these start words, begin the search for valid phrases in the groups
                        auto location = entity->getComponent<components::Position>()->get();
                        if (isStartWord(location))
                        {
                            std::deque<Parser::PhrasePair> currentPhrase;
                            std::unordered_set<misc::HexCoord> visitedGroup;

                            findPhrases(currentPhrase, location, level, gridDirection, visitedGroup);
                        }
                    }

                    // Return the gridWords back to its original (blank) state
                    for (auto&& entity : group)
                    {
                        auto location = entity->getComponent<components::Position>()->get();
                        m_gridWords[location.r][location.q] = components::TextType::None;
                    }
                }
            }
        }

        // In theory, this should move, sure hope it does!
        return m_phrases;
    }

    // ------------------------------------------------------------------
    //
    // Resursive method that collects the entities in a connected group.
    //
    // ------------------------------------------------------------------
    void PhraseSearch::collectGroup(const Level& level, const misc::HexCoord& position, entities::EntityVector& group)
    {
        static const auto isText = [](auto entity)
        {
            return entity->template hasComponent<components::Object>() && entity->template getComponent<components::Object>()->getType() == components::ObjectType::Text;
        };
        // Base case, if out of the level bounds, then nothing to do.
        if (!position.isValid(level.getWidth(), level.getHeight()))
        {
            return;
        }
        // Base case, if we've been here before, then nothing to do.
        if (m_visited[position.r][position.q])
        {
            return;
        }
        m_visited[position.r][position.q] = true;
        // Base case, if has no text, then we don't continue searching, so get out of here
        auto textCount = getTextCount(level, position);
        if (textCount == 0)
        {
            return;
        }

        if (textCount == 1)
        {
            auto itr = (level.getEntities(position) | std::views::values | std::views::filter(isText)).begin();
            group.push_back(*itr);
        }

        // Recursively visit the neighbors
        collectGroup(level, position.NE(), group);
        collectGroup(level, position.E(), group);
        collectGroup(level, position.SE(), group);
        collectGroup(level, position.SW(), group);
        collectGroup(level, position.W(), group);
        collectGroup(level, position.NW(), group);
    }

    // ------------------------------------------------------------------
    //
    // Examines the entities in this position for a single text entity.
    //
    // ------------------------------------------------------------------
    std::uint8_t PhraseSearch::getTextCount(const Level& level, const misc::HexCoord& position)
    {
        std::uint8_t count{ 0 };

        for (auto&& entity : level.getEntities(position) | std::views::values)
        {
            if (entity->hasComponent<components::Object>() && entity->getComponent<components::Object>()->getType() == components::ObjectType::Text)
            {
                count++;
            }
        }

        return count;
    }

    // ------------------------------------------------------------------
    //
    // A word is a start word if...
    //   * It is a noun
    //
    // ------------------------------------------------------------------
    bool PhraseSearch::isStartWord(misc::HexCoord position)
    {
        return components::Object::isNoun(m_gridWords[position.r][position.q]);
    }

    // ------------------------------------------------------------------
    //
    //
    //
    // ------------------------------------------------------------------
    void PhraseSearch::findPhrases(std::deque<Parser::PhrasePair> phrase, const misc::HexCoord& location, const Level& level, components::PhraseDirection::DirectionGrid& gridDirection, std::unordered_set<misc::HexCoord> visitedGroup)
    {
        // Base case: Invalid grid location
        if (!location.isValid(level.getWidth(), level.getHeight()))
        {
            return;
        }
        // Base case: Current position is not components::TextType
        if (m_gridWords[location.r][location.q] == components::TextType::None)
        {
            return;
        }

        // Add this location's word to the phrase
        phrase.push_back({ m_gridWords[location.r][location.q], location });

        auto parseResult = m_parser.parse(phrase);
        // Base case: Current phrase is invalid (not incomplete, but invalid)
        if (parseResult == Parser::ParseResult::Invalid)
        {
            return;
        }
        // Base case: Current phrase is valid
        if (parseResult == Parser::ParseResult::Valid)
        {
            // NOTE: There is no "end" phrase element that gets added, because the last word in a phrase doesn't have a direction arrow associated with it.
            auto phraseElement = components::PhraseDirection::PhraseElement::Start;
            // record the directions used in this phrase
            for (std::size_t word = 1; word < phrase.size(); word++)
            {
                gridDirection[phrase[word - 1].cell.r][phrase[word - 1].cell.q].insert({ m_nextPhraseId, { misc::HexCoord::getDirection(phrase[word - 1].cell, phrase[word].cell), phraseElement } });
                phraseElement = (phraseElement == components::PhraseDirection::PhraseElement::Start) ? components::PhraseDirection::PhraseElement::Middle : phraseElement;
            }
            m_phrases.push_back(phrase);
            m_nextPhraseId++;
        }

        // If this cell is a Noun or PropAbility, mark it as visited so it doesn't get considered again during this phrase search,
        // which, if not done, can result in an infinite loop.
        // if (components::Object::isNoun(gridWords[location.r][location.q]) || components::Object::isPropAbility(gridWords[location.r][location.q]))
        {
            visitedGroup.insert(location);
        }

        auto hasPhraseDirection = [&gridDirection](const misc::HexCoord& cell, const misc::HexCoord::Direction& direction)
        {
            for (auto&& data : gridDirection[cell.r][cell.q] | std::views::values)
            {
                if (std::get<components::PhraseDirection::GridDirection>(data) == direction)
                {
                    return true;
                }
            }

            return false;
        };

        // Visit neighbors
        auto neighbor = location.NW();
        if (neighbor.isValid(level.getWidth(), level.getHeight()) && !visitedGroup.contains(neighbor))
        {
            // Can't visit this neighbor if there is a phrase in the opposite direction already
            if (!hasPhraseDirection(neighbor, misc::HexCoord::getDirection(neighbor, location)))
            {
                findPhrases(phrase, neighbor, level, gridDirection, visitedGroup);
            }
        }
        neighbor = location.NE();
        if (neighbor.isValid(level.getWidth(), level.getHeight()) && !visitedGroup.contains(neighbor))
        {
            if (!hasPhraseDirection(neighbor, misc::HexCoord::getDirection(neighbor, location)))
            {
                findPhrases(phrase, neighbor, level, gridDirection, visitedGroup);
            }
        }
        neighbor = location.SW();
        if (neighbor.isValid(level.getWidth(), level.getHeight()) && !visitedGroup.contains(neighbor))
        {
            if (!hasPhraseDirection(neighbor, misc::HexCoord::getDirection(neighbor, location)))
            {
                findPhrases(phrase, neighbor, level, gridDirection, visitedGroup);
            }
        }
        neighbor = location.SE();
        if (neighbor.isValid(level.getWidth(), level.getHeight()) && !visitedGroup.contains(neighbor))
        {
            if (!hasPhraseDirection(neighbor, misc::HexCoord::getDirection(neighbor, location)))
            {
                findPhrases(phrase, neighbor, level, gridDirection, visitedGroup);
            }
        }
        neighbor = location.W();
        if (neighbor.isValid(level.getWidth(), level.getHeight()) && !visitedGroup.contains(neighbor))
        {
            if (!hasPhraseDirection(neighbor, misc::HexCoord::getDirection(neighbor, location)))
            {
                findPhrases(phrase, neighbor, level, gridDirection, visitedGroup);
            }
        }
        neighbor = location.E();
        if (neighbor.isValid(level.getWidth(), level.getHeight()) && !visitedGroup.contains(neighbor))
        {
            if (!hasPhraseDirection(neighbor, misc::HexCoord::getDirection(neighbor, location)))
            {
                findPhrases(phrase, neighbor, level, gridDirection, visitedGroup);
            }
        }
    }

    void PhraseSearch::sortGroupByLocation(entities::EntityVector& group)
    {
        // Order items by left-to-right and then top-to-bottom
        static const auto comparator = [](const entities::EntityPtr& a, const entities::EntityPtr& b)
        {
            assert(a->hasComponent<components::Position>());
            assert(b->hasComponent<components::Position>());

            auto aPosition = a->getComponent<components::Position>();
            auto bPosition = b->getComponent<components::Position>();

            if (aPosition->get().q < bPosition->get().q)
            {
                return true;
            }
            if (aPosition->get().q > bPosition->get().q)
            {
                return false;
            }
            if (aPosition->get().r < bPosition->get().r)
            {
                return true;
            }

            return false;
        };

        std::ranges::sort(group, comparator);
    }
} // namespace systems::parser