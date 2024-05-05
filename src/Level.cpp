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

#include "Level.hpp"

#include "components/Ability.hpp"
#include "components/Hint.hpp"
#include "components/Noun.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"
#include "components/Verb.hpp"
#include "entities/Factory.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/ContentKey.hpp"
#include "services/ControllerInput.hpp"
#include "services/Scoring.hpp"

#include <algorithm> // std::transform
#include <cassert>
#include <cctype> // std::::toupper
#include <charconv>
#include <format>
#include <functional>
#include <iostream>
#include <memory>
#include <string_view>
#include <unordered_map>

Level::Level(std::string name, std::string hint, std::string uuid, std::string challenges, std::uint8_t layers, std::uint16_t width, std::uint16_t height, misc::HexCoord cameraStartPos, std::uint8_t cameraStartRange) :
    m_name(name),
    m_hint(hint),
    m_uuid(uuid),
    m_width(width),
    m_height(height),
    m_cameraStartPos(cameraStartPos),
    m_cameraStartRange(cameraStartRange)
{
    m_entitiesByHash.resize(height);
    for (auto&& row : m_entitiesByHash)
    {
        row.resize(width);
    }

    m_entitiesByRenderOrder.resize(height);
    for (auto&& row : m_entitiesByRenderOrder)
    {
        row.resize(width);
    }

    m_levelData.resize(layers);

    m_challenges = Scoring::parseChallenges(challenges);
}

// Reference: https://stackoverflow.com/questions/67664399/trim-left-implementation-using-string-view-disallowing-temporary-parameters
template <typename T, std::enable_if_t<std::is_same<T, std::string_view>::value || !std::is_rvalue_reference_v<T&&>, int> = 0>
std::string_view trim_left(T&& data, std::string_view trimChars)
{
    std::string_view sv{ std::forward<T>(data) };
    sv.remove_prefix(std::min(sv.find_first_not_of(trimChars), sv.size()));
    return sv;
}

// --------------------------------------------------------------
//
// Verifies this is a valid position in the level.
//
// --------------------------------------------------------------
bool Level::isValid(const misc::HexCoord& cell)
{
    return cell.r >= 0 && cell.q >= 0 && cell.r < this->getHeight() && cell.q < this->getWidth();
}

// --------------------------------------------------------------
//
// Search through the pre-defined challenges and see if we have a
// match.  If so, return that challenge, if not, return a nullopt
//
// --------------------------------------------------------------
std::optional<Scoring::ChallengeGroup> Level::matchChallenge(const Scoring::ChallengeGroup& result)
{
    // Just do an exhaustive search.  There aren't that many things, so it
    // isn't a performance senstive bit of code.
    for (auto&& group : m_challenges)
    {
        // The sizes have to at least be equal
        if (bool allEqual = group.size() == result.size(); allEqual)
        {
            for (auto&& challenge : group)
            {
                auto item = std::find_if(result.begin(), result.end(), [&](auto& test)
                                         {
                                             return std::get<0>(test) == std::get<0>(challenge);
                                         });
                if (item != result.end())
                {
                    if (std::get<1>(challenge) != std::get<1>(*item))
                    {
                        allEqual = false;
                    }
                }
                else
                {
                    allEqual = false;
                }
            }

            if (allEqual)
            {
                return group;
            }
        }
    }

    return std::nullopt;
}

// --------------------------------------------------------------
//
// Get the level into the initial gameplay state, based on the
// settings from the file.
//
// --------------------------------------------------------------
void Level::initialize(std::function<void(entities::EntityPtr)> addEntity)
{
    //
    // Make sure nothing else is lying around
    clear();

    //
    // Create the initial set of entities, based on the data read from the file
    for (auto&& data : m_levelData)
    {
        initialize(data, addEntity);
    }

    // If there is a hint, create a hint entity
    if (!m_hint.empty())
    {
        auto hintText{ m_hint };
        auto hintUpper{ m_hint };
        auto hintEntity = std::make_shared<entities::Entity>();
        // First check for one of the built-in hints
        std::transform(hintUpper.begin(), hintUpper.end(), hintUpper.begin(),
                       [](unsigned char c)
                       {
                           // Have to cast to remove a compiler warning
                           return static_cast<unsigned char>(std::toupper(c));
                       });
        if (hintUpper == HINT_MOVEMENT)
        {
            // Show keyboard controls if no controller input has occurred, otherwise
            // show controller, um, controls
            if (ControllerInput::instance().hasControllerBeenUsed())
            {
                hintText = std::format("Movement {{ Left Joystick }}");
            }
            else
            {
                hintText = std::format(
                    "Movement Keys: '{0}' '{1}' '{2}' '{3}' '{4}' '{5}'",
                    Configuration::get<std::string>(config::KEYBOARD_UP_LEFT),
                    Configuration::get<std::string>(config::KEYBOARD_UP_RIGHT),
                    Configuration::get<std::string>(config::KEYBOARD_LEFT),
                    Configuration::get<std::string>(config::KEYBOARD_RIGHT),
                    Configuration::get<std::string>(config::KEYBOARD_DOWN_LEFT),
                    Configuration::get<std::string>(config::KEYBOARD_DOWN_RIGHT));
            }
            auto hintTime = misc::msTous(std::chrono::milliseconds(10000));

            hintEntity->addComponent(std::make_unique<components::Hint>(hintText, hintTime));
            addEntity(hintEntity);
        }
        else if (hintUpper == HINT_PULL)
        {
            // Pull Is Shift / Left - Trigger + Direction
            //   Show keyboard controls if no controller input has occurred, otherwise
            //   show controller, um, controls
            if (ControllerInput::instance().hasControllerBeenUsed())
            {
                hintText = std::format("Pull is {{ Left Joystick + L2 }}");
            }
            else
            {
                hintText = std::format("Pull Is Shift + Direction");
            }
            auto hintTime = misc::msTous(std::chrono::milliseconds(20000));

            hintEntity->addComponent(std::make_unique<components::Hint>(hintText, hintTime));
            addEntity(hintEntity);
        }
        else if (hintUpper == HINT_CAMERA)
        {
            // Show keyboard controls if no controller input has occurred, otherwise
            // show controller, um, controls
            if (ControllerInput::instance().hasControllerBeenUsed())
            {
                auto hintText1 = std::format("Camera Pan {{ Right Joystick }}");
                auto hintText2 = std::format("Camera Zoom {{ Right Joystick + R2 }}");
                auto hintTime = misc::msTous(std::chrono::milliseconds(10000));

                auto hint1Entity = std::make_shared<entities::Entity>();
                hint1Entity->addComponent(std::make_unique<components::Hint>(hintText1, hintTime));
                addEntity(hint1Entity);

                auto hint2Entity = std::make_shared<entities::Entity>();
                hint2Entity->addComponent(std::make_unique<components::Hint>(hintText2, hintTime));
                addEntity(hint2Entity);
            }
            else
            {
                hintText = std::format(
                    "Camera Keys: '{0}' '{1}' '{2}' '{3}' '{4}' '{5}'",
                    Configuration::get<std::string>(config::KEYBOARD_CAMERA_UP),
                    Configuration::get<std::string>(config::KEYBOARD_CAMERA_DOWN),
                    Configuration::get<std::string>(config::KEYBOARD_CAMERA_LEFT),
                    Configuration::get<std::string>(config::KEYBOARD_CAMERA_RIGHT),
                    Configuration::get<std::string>(config::KEYBOARD_CAMERA_ZOOM_IN),
                    Configuration::get<std::string>(config::KEYBOARD_CAMERA_ZOOM_OUT));

                // Show it for a long time
                auto hintTime = misc::msTous(std::chrono::milliseconds(20000));

                hintEntity->addComponent(std::make_unique<components::Hint>(hintText, hintTime));
                addEntity(hintEntity);
            }
        }
        else // A regular old fashioned hint from the puzzle itself
        {
            auto hintTime = misc::msTous(std::chrono::milliseconds(10000));
            hintEntity->addComponent(std::make_unique<components::Hint>(hintText, hintTime));
            addEntity(hintEntity);
        }
    }
}

// --------------------------------------------------------------
//
// Used both during initialization and with undo to ensure everything
// is cleared from the level.
//
// --------------------------------------------------------------
void Level::clear()
{
    m_entitiesAll.clear();

    for (auto&& row : m_entitiesByHash)
    {
        for (auto&& cell : row)
        {
            cell.clear();
        }
    }

    for (auto&& row : m_entitiesByRenderOrder)
    {
        for (auto&& cell : row)
        {
            cell.clear();
        }
    }
}

void Level::addEntity(entities::EntityPtr entity)
{
    if (entity->hasComponent<components::Position>() && entity->hasComponent<components::Object>())
    {
        // By coordinate
        auto position = entity->getComponent<components::Position>();
        m_entitiesByHash[position->get().r][position->get().q][entity->getId()] = entity;

        // By render order
        auto renderOrder = entity->getComponent<components::Object>();
        m_entitiesByRenderOrder[position->get().r][position->get().q].insert({ renderOrder->getType(), entity });

        // All
        m_entitiesAll[entity->getId()] = entity;
    }
}

void Level::removeEntity(entities::Entity::IdType entityId)
{
    if (m_entitiesAll.find(entityId) != m_entitiesAll.end())
    {
        auto entity = m_entitiesAll[entityId];

        // By coordinate
        auto position = entity->getComponent<components::Position>();
        m_entitiesByHash[position->get().r][position->get().q].erase(entity->getId());

        // By render order
        Level::RenderOrderStorage::iterator itrEntity;
        for (auto itr = m_entitiesByRenderOrder[position->get().r][position->get().q].begin(); itr != m_entitiesByRenderOrder[position->get().r][position->get().q].end(); ++itr)
        {
            if (itr->second->getId() == entityId)
            {
                itrEntity = itr;
                break; // extremely rare I do this, but in this case, there isn't anything to continue searching for
            }
        }
        m_entitiesByRenderOrder[position->get().r][position->get().q].erase(itrEntity);

        // All
        m_entitiesAll.erase(entityId);
    }
}

const entities::EntityMap& Level::getEntities(const misc::HexCoord& cell) const
{
    if (cell.r < 0 || cell.q < 0 || cell.r >= this->getHeight() || cell.q >= this->getWidth())
    {
        static entities::EntityMap empty;
        return empty;
    }

    return m_entitiesByHash[cell.r][cell.q];
}

Level::RenderOrderStorage Level::getEntitiesByRender(const misc::HexCoord& cell)
{
    assert(cell.r >= 0);
    assert(cell.r < this->getHeight());
    assert(cell.q >= 0);
    assert(cell.q < this->getWidth());

    return m_entitiesByRenderOrder[cell.r][cell.q];
}

void Level::moveEntity(entities::EntityPtr entity, misc::HexCoord previous)
{
    //
    // --------------- By Coordinate ---------------
    //
    // Remove it from it's old location
    m_entitiesByHash[previous.r][previous.q].erase(entity->getId());
    //
    // Place it in it's new location
    auto position = entity->getComponent<components::Position>();
    assert(this->isValid(position->get()));

    m_entitiesByHash[position->get().r][position->get().q].insert({ entity->getId(), entity });

    //
    // --------------- By Render Order ---------------
    //
    // Remove it from it's old location
    Level::RenderOrderStorage::iterator itrEntity;
    for (auto itr = m_entitiesByRenderOrder[previous.r][previous.q].begin(); itr != m_entitiesByRenderOrder[previous.r][previous.q].end(); ++itr)
    {
        if (itr->second->getId() == entity->getId())
        {
            itrEntity = itr;
            break; // extremely rare I do this, but in this case, there isn't anything to continue searching for
        }
    }
    m_entitiesByRenderOrder[previous.r][previous.q].erase(itrEntity);

    //
    // Place it in it's new location
    auto renderOrder = entity->getComponent<components::Object>();
    m_entitiesByRenderOrder[position->get().r][position->get().q].insert({ renderOrder->getType(), entity });
}

// --------------------------------------------------------------
//
// Get all the entities created and ready to go.
//
// --------------------------------------------------------------
void Level::initialize(const std::vector<std::string>& levelData, std::function<void(entities::EntityPtr)> addEntity)
{
    auto map = entities::buildCodeToEntityCommandMap();

    for (misc::HexCoord::Type r = 0; r < getHeight(); r++)
    {
        std::string_view line = levelData[r];
        for (misc::HexCoord::Type q = 0; q < getWidth(); q++)
        {
            std::uint16_t code{ 0 };
            auto codeView = trim_left(line.substr(static_cast<std::size_t>(q) * 2, 2), " ");
            std::from_chars(codeView.data(), codeView.data() + codeView.size(), code);
            if (map.contains(static_cast<entities::EntityCode>(code)))
            {
                auto entity = map[static_cast<entities::EntityCode>(code)]({ q, r });
                addEntity(entity);
            }
        }
    }
}
