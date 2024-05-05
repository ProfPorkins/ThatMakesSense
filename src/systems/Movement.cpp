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

#include "Movement.hpp"

#include "components/Ability.hpp"
#include "components/Audio.hpp"
#include "components/InputControlled.hpp"
#include "components/Object.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"
#include "misc/misc.hpp"
#include "services/Audio.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/ControllerInput.hpp"
#include "services/KeyboardInput.hpp"

#include <algorithm>
#include <cmath>
#include <ranges>
#include <utility>

namespace systems
{
    const static auto canSend = [](auto entity)
    {
        return entity->template hasComponent<components::Ability>() && entity->template getComponent<components::Ability>()->has(components::AbilityType::Send);
    };

    Movement::Movement(std::shared_ptr<Level> level, std::function<void()> signalMovement, std::function<void(entities::Entity::IdType)> notifyUpdated) :
        System({ ctti::unnamed_type_id<components::Position>(),
                 ctti::unnamed_type_id<components::InputControlled>() }),
        m_level(level),
        m_signalMovement(signalMovement),
        m_notifyUpdated(notifyUpdated)
    {
        registerKeyboardInput();
        registerControllerInput();
    }
    void Movement::update([[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        static const auto CONTROLLER_REPEAT_DELAY = misc::msTous(std::chrono::milliseconds(Configuration::get<std::uint16_t>(config::CONTROLLER_REPEAT_DELAY)));

        // Need to check this, because after the puzzle has been solved, the controller
        // handler doesn't exist, but old queued up events are still processed.  Putting
        // this 'if' statement here ensures the player movement stops when puzzle is solved.
        if (m_controllerMoveHandler.has_value())
        {
            // Before handling movement, update input direction based on state of the joystick
            // We will let controller input override keyboard inputs
            m_controllerInputWait -= elapsedTime;
            if (m_controllerInputWait < std::chrono::microseconds::zero())
            {
                auto direction = computeControllerDirection();
                if (direction.has_value())
                {
                    m_inputDirection = direction.value();
                }
                m_controllerInputWait = CONTROLLER_REPEAT_DELAY;
            }
        }

        if (m_inputDirection)
        {
            m_audioPlayed = false;
            m_anyMoved = false;

            performMove(m_entities);

            if (m_anyMoved)
            {
                m_signalMovement();
            }
            m_inputDirection.reset();
        }
    }

    void Movement::shutdown()
    {
        for (auto&& id : m_keyboardInputHandlers)
        {
            KeyboardInput::instance().unregisterKeyDownHandler(id);
        }
        m_keyboardInputHandlers.clear();

        if (m_controllerMoveHandler.has_value())
        {
            ControllerInput::instance().unregisterAnyAxisHandler(m_controllerMoveHandler.value());
            m_controllerMoveHandler.reset();
        }
    }

    // --------------------------------------------------------------
    //
    // Have a tricky situation here.  Movement itself is only interested
    // in entities that have a position and are input controlled.  However,
    // to perform the 'send' movement, we want to track entities that
    // have that property separately.  That's why you see what you see below.
    //
    // --------------------------------------------------------------
    bool Movement::addEntity(entities::EntityPtr entity)
    {
        bool added{ false };
        if (added = System::addEntity(entity); !added)
        {
            if (canSend(entity))
            {
                m_sendByPosition[entity->getComponent<components::Position>()->get()] = entity;
                added = true;
            }
        }

        return added;
    }

    void Movement::removeEntity(entities::Entity::IdType entityId)
    {
        if (m_entities.contains(entityId) && m_entities[entityId]->hasComponent<components::Position>())
        {
            m_sendByPosition.erase(m_entities[entityId]->getComponent<components::Position>()->get());
        }

        System::removeEntity(entityId);
    }

    // --------------------------------------------------------------
    //
    // Because we track two different two different kinds of entities
    // the checking for 'send' or movement entities is a little more
    // complex to deal with.
    //
    // --------------------------------------------------------------
    void Movement::updatedEntity(entities::EntityPtr entity)
    {
        // Because the lambda is static, have to pass the send entities container in
        // as a parameter, otherwise the container will become invalid after the
        // second GameModel instance is created.
        static const auto isTracked = [](const entities::EntityPtr& entity, const std::unordered_map<misc::HexCoord, entities::EntityPtr>& entities)
        {
            if (entity->hasComponent<components::Position>())
            {
                auto position = entity->getComponent<components::Position>()->get();
                if (entities.contains(position) && entity->getId() == entities.at(position)->getId())
                {
                    return true;
                }
            }

            return false;
        };

        if (isTracked(entity, m_sendByPosition))
        {
            if (!canSend(entity))
            {
                m_sendByPosition.erase(entity->getComponent<components::Position>()->get());
            }
        }
        else if (canSend(entity))
        {
            m_sendByPosition[entity->getComponent<components::Position>()->get()] = entity;
        }
        else
        {
            System::updatedEntity(entity);
        }
    }

    void Movement::registerKeyboardInput()
    {
        static const auto keyboardRepeatDelay = misc::msTous(std::chrono::milliseconds(Configuration::get<std::uint16_t>(config::KEYBOARD_REPEAT_DELAY)));

        auto id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_UP_LEFT),
            KeyboardInput::Modifier::None,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = false;
                m_inputDirection = misc::HexCoord::Direction::NW;
            });
        m_keyboardInputHandlers.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_UP_LEFT),
            KeyboardInput::Modifier::Shift,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = true;
                m_inputDirection = misc::HexCoord::Direction::NW;
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_UP_RIGHT),
            KeyboardInput::Modifier::None,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = false;
                m_inputDirection = misc::HexCoord::Direction::NE;
            });
        m_keyboardInputHandlers.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_UP_RIGHT),
            KeyboardInput::Modifier::Shift,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = true;
                m_inputDirection = misc::HexCoord::Direction::NE;
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_DOWN_LEFT),
            KeyboardInput::Modifier::None,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = false;
                m_inputDirection = misc::HexCoord::Direction::SW;
            });
        m_keyboardInputHandlers.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_DOWN_LEFT),
            KeyboardInput::Modifier::Shift,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = true;
                m_inputDirection = misc::HexCoord::Direction::SW;
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_DOWN_RIGHT),
            KeyboardInput::Modifier::None,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = false;
                m_inputDirection = misc::HexCoord::Direction::SE;
            });
        m_keyboardInputHandlers.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_DOWN_RIGHT),
            KeyboardInput::Modifier::Shift,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = true;
                m_inputDirection = misc::HexCoord::Direction::SE;
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_LEFT),
            KeyboardInput::Modifier::None,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = false;
                m_inputDirection = misc::HexCoord::Direction::W;
            });
        m_keyboardInputHandlers.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_LEFT),
            KeyboardInput::Modifier::Shift,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = true;
                m_inputDirection = misc::HexCoord::Direction::W;
            });
        m_keyboardInputHandlers.push_back(id);

        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_RIGHT),
            KeyboardInput::Modifier::None,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = false;
                m_inputDirection = misc::HexCoord::Direction::E;
            });
        m_keyboardInputHandlers.push_back(id);
        id = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_RIGHT),
            KeyboardInput::Modifier::Shift,
            true,
            keyboardRepeatDelay,
            [this](std::chrono::microseconds)
            {
                m_withPull = true;
                m_inputDirection = misc::HexCoord::Direction::E;
            });
        m_keyboardInputHandlers.push_back(id);
    }

    void Movement::registerControllerInput()
    {
        m_controllerMoveHandler = ControllerInput::instance().registerAnyAxisHandler(
            [this](ControllerInput::Axis axis, float position, const std::chrono::microseconds)
            {
                switch (axis)
                {
                    case ControllerInput::Axis::Joystick1UpDown:
                        m_axisUpDown = position;
                        break;
                    case ControllerInput::Axis::Joystick1LeftRight:
                        m_axisLeftRight = position;
                        break;
                    case ControllerInput::Axis::TriggerLeft:
                        m_axisLeftTrigger = position;
                        break;
                    default: // This is here to prevent a compiler warning
                        break;
                }
            });
    }

    // --------------------------------------------------------------
    //
    // Based on the current position of the joystick, determine the
    // move direction (if any)
    //
    // --------------------------------------------------------------
    std::optional<misc::HexCoord::Direction> Movement::computeControllerDirection()
    {
        static const auto SENSITIVITY_GENERAL = 0.2f;
        static const auto SENSITIVITY_UPDOWN = 0.5f;

        std::optional<misc::HexCoord::Direction> direction = std::nullopt;
        if (m_axisUpDown < -SENSITIVITY_GENERAL && m_axisLeftRight > SENSITIVITY_GENERAL)
        {
            direction = misc::HexCoord::Direction::NE;
        }
        if ((m_axisUpDown < SENSITIVITY_UPDOWN && m_axisUpDown > -SENSITIVITY_UPDOWN) && m_axisLeftRight > SENSITIVITY_GENERAL)
        {
            direction = misc::HexCoord::Direction::E;
        }
        if (m_axisUpDown > SENSITIVITY_GENERAL && m_axisLeftRight > SENSITIVITY_GENERAL)
        {
            direction = misc::HexCoord::Direction::SE;
        }
        else if (m_axisUpDown < -SENSITIVITY_GENERAL && m_axisLeftRight < -SENSITIVITY_GENERAL)
        {
            direction = misc::HexCoord::Direction::NW;
        }
        if ((m_axisUpDown < SENSITIVITY_UPDOWN && m_axisUpDown > -SENSITIVITY_UPDOWN) && m_axisLeftRight < -SENSITIVITY_GENERAL)
        {
            direction = misc::HexCoord::Direction::W;
        }
        if (m_axisUpDown > SENSITIVITY_GENERAL && m_axisLeftRight < -SENSITIVITY_GENERAL)
        {
            direction = misc::HexCoord::Direction::SW;
        }

        // Only want to determine with pull if the controller is causing movement.  If this is just
        // always done, it can result in a false "with pull" for the keyboard input, due to when the lambdas
        // are called.
        if (direction.has_value())
        {
            m_withPull = m_axisLeftTrigger > SENSITIVITY_GENERAL;
        }

        return direction;
    }

    // --------------------------------------------------------------
    //
    // This performs a scan of all the entities that are input controlled
    // and determines if and how they can move.
    //
    // --------------------------------------------------------------
    void Movement::performMove(entities::EntityMap& entities)
    {
        // Interesting issue can happen.  Multiple objects that can 'pull' or 'push' can
        // end up in the same location.  When that happens, unless something is done, they each
        // 'push' and/or 'pull', causing two actions to occur on the neighboring objects, and
        // that isn't the behavior we want.  Instead, we want only one of the objects to take
        // the action on the neighbors.

        // Go through all the entities and see if they can move
        for (auto&& entity : entities | std::views::values)
        {
            auto position = entity->getComponent<components::Position>()->get();
            if (move(entity, m_inputDirection.value()))
            {
                // If pull is requested, lets see if it can be done and if any entities can be pulled
                if (m_withPull && entity->getComponent<components::Ability>()->has(components::AbilityType::Pull))
                {
                    auto previousCell = misc::HexCoord::getNextCell(position, misc::HexCoord::getOpposite(m_inputDirection.value()));
                    previousCell = transformBySend(previousCell, misc::HexCoord::getOpposite(m_inputDirection.value()));
                    pull(
                        previousCell,
                        m_inputDirection.value(),
                        misc::HexCoord::getOpposite(m_inputDirection.value()));
                }

                m_anyMoved = true;
            }
        }

        //
        // Commit any moves
        for (auto&& [id, move] : m_moves)
        {
            auto position = move.entity->getComponent<components::Position>();
            auto original = position->get();
            position->set(move.position);

            m_level->moveEntity(move.entity, original);

            // Finally, notify everyone of the move
            m_notifyUpdated(id);
        }
        m_moves.clear();

        //
        // Commit any pulls
        for (auto&& [entity, position] : m_pulls)
        {
            auto original = entity->getComponent<components::Position>()->get();
            entity->getComponent<components::Position>()->set(position);
            m_level->moveEntity(entity, original);
        }
        m_pulls.clear();
    }

    // --------------------------------------------------------------
    //
    // This is where the work of moving, stopping, pushing of entities
    // is done.
    //
    // --------------------------------------------------------------
    bool Movement::move(entities::EntityPtr entity, misc::HexCoord::Direction direction)
    {
        static const auto hasMovementProperties = [](entities::EntityPtr entity)
        {
            bool interested{ false };

            if (entity->hasComponent<components::Property>())
            {
                auto property = entity->getComponent<components::Property>();
                interested = interested || property->has(components::PropertyType::Pushable);
                interested = interested || property->has(components::PropertyType::Stop);
                interested = interested || property->has(components::PropertyType::Steep);
            }

            return interested;
        };
        bool moved{ false };

        //
        // Start by finding the coordinates for where this entity wants to move, because
        // we are going to recursively check entities at that location to see if they moved
        // or stop things
        auto compPosition = entity->getComponent<components::Position>();
        auto proposed = misc::HexCoord::getNextCell(compPosition->get(), direction);
        proposed = transformBySend(proposed, direction);

        //
        // Find the entities, if any, that can impact movement or cascade movement in some way
        std::vector<entities::EntityPtr> tryThese;
        for (auto&& neighbor : m_level->getEntities(proposed) | std::views::values | std::views::filter(hasMovementProperties))
        {
            tryThese.push_back(neighbor);
        }

        // First have to go through the neighbors to see if it could possibly move
        bool canMove{ true };
        for (auto ability = entity->getComponent<components::Ability>(); auto&& neighbor : tryThese)
        {
            auto property = neighbor->getComponent<components::Property>();
            if (property->has(components::PropertyType::Steep) && !ability->has(components::AbilityType::Climb))
            {
                canMove = false;
            }
            if (property->has(components::PropertyType::Stop))
            {
                canMove = false;
            }
        }

        // Once we know if it could move, then we check for ability to push, because that can also impact
        // whether or not the entity could move
        if (canMove)
        {
            bool canPush{ true };
            for (auto ability = entity->getComponent<components::Ability>(); auto&& neighbor : tryThese)
            {
                auto property = neighbor->getComponent<components::Property>();
                if (property->has(components::PropertyType::Pushable) && ability->has(components::AbilityType::Push))
                {
                    // Recursively try to move the neighbor
                    canMove = canMove && move(neighbor, direction);
                    canPush = canPush && true;
                }
            }
        }

        if (canMove)
        {
            if (m_level->isValid(proposed))
            {
                // Aggregate the moves, then they'll be committed at a later time
                m_moves[entity->getId()] = { entity, proposed };

                // We only want to play audio for one of the entities, rather than all of them
                if (!m_audioPlayed && entity->hasComponent<components::Audio>())
                {
                    Audio::play(entity->getComponent<components::Audio>()->getKey());
                    m_audioPlayed = true;
                }

                moved = true;
            }
        }

        return moved;
    }

    // --------------------------------------------------------------
    //
    //
    //
    // --------------------------------------------------------------
    void Movement::pull(misc::HexCoord position, misc::HexCoord::Direction toDirection, misc::HexCoord::Direction fromDirection)
    {
        static const auto isType = [](entities::EntityPtr entity)
        {
            bool interested{ false };

            if (entity->hasComponent<components::Property>())
            {
                auto property = entity->getComponent<components::Property>();
                interested = interested || property->has(components::PropertyType::Pullable);
            }
            // Don't pull anything that is InputControlled, as it has already moved
            interested = interested && !entity->hasComponent<components::InputControlled>();

            return interested;
        };
        static const auto isSteep = [](entities::EntityPtr entity)
        {
            return entity->hasComponent<components::Property>() &&
                   entity->getComponent<components::Property>()->has(components::PropertyType::Steep);
        };

        // Base case: If the position is outside the grid, we are done
        if (!position.isValid(m_level->getWidth(), m_level->getHeight()))
        {
            return;
        }

        // Get all the entities in this cell and see if they are pullable.
        // If they are pullable, then move them.
        // This has to be performed in two steps, because you can't iterate over the entities
        // in a cell while also modifying that collection.
        entities::EntityVector pullThese;
        entities::EntityPtr canPull{ nullptr };
        for (auto&& entity : m_level->getEntities(position) | std::views::values | std::views::filter(isType))
        {
            // If the next cell has a "steep" entity and this entity doesn't have "climb" it can't be pulled
            auto nextPosition = misc::HexCoord::getNextCell(position, toDirection);
            auto anySteep = std::ranges::any_of(m_level->getEntities(nextPosition) | std::views::values, isSteep);
            if (anySteep && entity->getComponent<components::Ability>()->has(components::AbilityType::Climb))
            {
                pullThese.push_back(entity);
            }
            else if (!anySteep) // I know I could combine the logic above, but this makes it more clear for later on when I have to read the logic again
            {
                pullThese.push_back(entity);
            }
            // As long as we get one, it doesn't matter which one
            if (entity->hasComponent<components::Ability>() && entity->getComponent<components::Ability>()->has(components::AbilityType::Pull))
            {
                canPull = entity;
            }
        }

        //
        // If any entity in this cell has the pull ability (detected above), then go ahead and recursively
        // pull more entities.
        if (canPull)
        {
            auto nextCell = misc::HexCoord::getNextCell(position, fromDirection);
            nextCell = transformBySend(nextCell, fromDirection);
            pull(nextCell, toDirection, fromDirection);
        }

        // Commit any pulls
        for (auto&& entity : pullThese)
        {
            auto nextCell = misc::HexCoord::getNextCell(position, toDirection);
            nextCell = transformBySend(nextCell, toDirection);
            m_pulls[entity] = nextCell;

            // Need to send notification of the pull
            m_notifyUpdated(entity->getId());
        }
    }

    // --------------------------------------------------------------
    //
    // If the proposed position has a 'send' entity, changed the proposed position to the next position
    // with a 'send' entity.
    //
    // --------------------------------------------------------------
    misc::HexCoord Movement::transformBySend(misc::HexCoord position, const misc::HexCoord::Direction& direction)
    {
        auto itr = m_sendByPosition.find(position);
        if (itr != m_sendByPosition.end())
        {
            // The iterator is with respect to the send entity we are on, therefore,
            // we increment this pointer to go to the "next" send entity, if it exists.
            ++itr;
            if (itr != m_sendByPosition.end())
            {
                position = itr->first;
            }
            else
            {
                position = m_sendByPosition.begin()->first;
            }
            // Then, we move the entity one more position, so it doesn't land on the send position, but one more position past it
            position = misc::HexCoord::getNextCell(position, direction);
        }

        return position;
    }

} // namespace systems
