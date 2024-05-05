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

#include "Undo.hpp"

#include "components/PhraseDirection.hpp"
#include "components/Position.hpp"
#include "components/Property.hpp"
#include "misc/misc.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/ControllerInput.hpp"
#include "services/KeyboardInput.hpp"

namespace systems
{
    Undo::Undo(std::function<void()> signalReset, std::function<void(entities::EntityPtr)> addEntity, std::function<void(entities::Entity::IdType)> removeEntity) :
        System({ ctti::unnamed_type_id<components::Position>() }),
        funcSignalReset(signalReset),
        funcAddEntity(addEntity),
        funcRemoveEntity(removeEntity)
    {
        m_keyboardUndoHandlerId = KeyboardInput::instance().registerKeyDownHandler(
            Configuration::get<std::string>(config::KEYBOARD_UNDO),
            KeyboardInput::Modifier::None,
            true,
            misc::msTous(std::chrono::milliseconds(Configuration::get<std::uint16_t>(config::KEYBOARD_REPEAT_DELAY))),
            [this](std::chrono::microseconds)
            {
                m_performUndo = true;
            });

        m_keyboardResetHandlerId = KeyboardInput::instance().registerKeyReleasedHandler(
            Configuration::get<std::string>(config::KEYBOARD_RESET),
            [this]()
            {
                m_performReset = true;
            });

        m_controllerUndoHandlerId = ControllerInput::instance().registerButtonDownHandler(
            ControllerInput::Button::Left,
            true,
            misc::msTous(std::chrono::milliseconds(Configuration::get<std::uint16_t>(config::CONTROLLER_REPEAT_DELAY))),
            [this](ControllerInput::Button, std::chrono::microseconds)
            {
                m_performUndo = true;
            });

        m_controllerResetHandlerId = ControllerInput::instance().registerButtonReleasedHandler(
            ControllerInput::Button::Top,
            [this](ControllerInput::Button, std::chrono::microseconds)
            {
                m_performReset = true;
            });
    }

    // --------------------------------------------------------------
    //
    // Because we don't track PhraseDirection, need to check if the
    // entity exists in our container
    //
    // --------------------------------------------------------------
    void Undo::removeEntity(entities::Entity::IdType entityId)
    {
        if (m_entities.contains(entityId))
        {
            m_takeSnapshot = true;
            m_entitiesRemoved.push_back(m_entities[entityId]);
            System::removeEntity(entityId);
        }
    }

    // --------------------------------------------------------------
    //
    // We don't care about PhraseDirection entities, as they get regenerated
    // by the RuleSearch system after any changes to the grid.
    //
    // --------------------------------------------------------------
    bool Undo::isInterested(const entities::EntityPtr& entity)
    {
        return !entity->hasComponent<components::PhraseDirection>() && System::isInterested(entity);
    }

    void Undo::update([[maybe_unused]] std::chrono::microseconds elapsedTime, bool& actionTaken)
    {
        //
        // Taking a snapshot must occur before performing an undo.  The reason
        // for this is that after an undo, a new snapshot is requested.  If the
        // undo is performed first, all the reverted entities haven't yet been
        // committed to the systems, including this undo system, and as a result
        // a snapshot with no entities will occur; and we don't want that.
        if (m_takeSnapshot)
        {
            m_stack.push({});

            // Look for new or updated entities
            for (auto&& [id, entity] : m_entities)
            {
                if (!m_surface.contains(id))
                {
                    // It is new, need to track it
                    auto clone = entity->clone();

                    m_surface[id] = clone;
                    m_stack.top().entitiesNew.insert({ id, clone });
                }                                            // Using this form of the != operator to eliminate a clang compiler warning
                else if (m_surface[id]->operator!=(*entity)) // Comparing entities for inequality to see if there are any changes
                {
                    // It has changed in some way, need to track it.
                    // The previous entity state (in the surface) is tracked in the surface,
                    // while the new entity state is tracked in the stack.
                    auto clone = entity->clone();
                    auto cloneSurface = m_surface[id]->clone();

                    m_stack.top().entitiesNew.insert({ id, clone });
                    m_stack.top().entitiesPrevious.insert({ id, cloneSurface });

                    m_surface[id] = clone;
                }
            }

            // Handle the removed entities
            for (auto&& entity : m_entitiesRemoved)
            {
                // Make a clone of the remove entity from the surface, because it might have
                // moved when it was removed, and we need to undo to its previous location
                // NOTE: The reason for this test is to prevent a program crash when the puzzle
                //       has entities that are burned right at startup.  A puzzle shouldn't be
                //       created to do this, but it happens, and we don't want the program
                //       to crash when that happens.
                if (m_surface.contains(entity->getId()))
                {
                    auto clone = m_surface[entity->getId()]->clone();
                    m_stack.top().entitiesRemoved.push_back(clone);
                    // Then, remove it from the surfface, because we don't want to keep tracking it
                    // now that it is removed.
                    m_surface.erase(entity->getId());
                }
                else
                {
                    assert(false); // This allows us to detect the program when running in debug
                }
            }
            m_entitiesRemoved.clear();

            m_takeSnapshot = false;
        }

        actionTaken = m_performUndo || m_performReset;
        if (m_performUndo)
        {
            performUndo();
            m_performUndo = false;
        }

        if (m_performReset)
        {
            performReset();
            m_performReset = false;
        }
    }

    void Undo::shutdown()
    {
        if (m_keyboardUndoHandlerId.has_value())
        {
            KeyboardInput::instance().unregisterKeyDownHandler(m_keyboardUndoHandlerId.value());
            m_keyboardUndoHandlerId.reset();
        }
        if (m_keyboardResetHandlerId.has_value())
        {
            KeyboardInput::instance().unregisterKeyReleasedHandler(m_keyboardResetHandlerId.value());
            m_keyboardResetHandlerId.reset();
        }
        if (m_controllerUndoHandlerId.has_value())
        {
            ControllerInput::instance().unregisterButtonDownHandler(m_controllerUndoHandlerId.value());
            m_controllerUndoHandlerId.reset();
        }
        if (m_controllerResetHandlerId.has_value())
        {
            ControllerInput::instance().unregisterButtonReleasedHandler(m_controllerResetHandlerId.value());
            m_controllerResetHandlerId.reset();
        }
    }

    // --------------------------------------------------------------
    //
    // The current top of the stack is empty, so just throw it away,
    // it is the next item on the stack we care about.
    //
    // --------------------------------------------------------------
    void Undo::performUndo()
    {
        // The very first stack frame is the initial set of entities in the game, we
        // never want to undo it, because there is nothing to do if that happens.
        if (m_stack.size() >= 2)
        {
            // Remove the newly updated entities first
            for (auto&& [id, entity] : m_stack.top().entitiesNew)
            {
                funcRemoveEntity(entity->getId());
                m_surface.erase(id);
            }

            // Next, add back in the previously updated entities
            for (auto&& [id, entity] : m_stack.top().entitiesPrevious)
            {
                funcAddEntity(entity);
                m_surface.insert({ entity->getId(), entity->clone() });
            }

            // Finally, add back in any removed entities
            for (auto&& entity : m_stack.top().entitiesRemoved)
            {
                funcAddEntity(entity);
                m_surface.insert({ entity->getId(), entity->clone() });
            }

            m_stack.pop();
        }
    }

    void Undo::performReset()
    {
        // Let the game model take action based on the reset about to happen.
        // In short, it removes all entities.
        funcSignalReset();

        //
        // Need to unwind the stack all the way to the first one, because that
        // is the initial set of entities in the level.
        while (m_stack.size() > 1)
        {
            m_stack.pop();
        }
        // Now, the only remaining frame left on the stack is the initial set of
        // entities, let's restore them.
        // The reason for cloning them while adding, is that we always need to keep
        // live entities separate from saved state entities
        m_surface.clear();
        for (auto&& [id, entity] : m_stack.top().entitiesNew)
        {
            funcAddEntity(entity->clone());
            m_surface.insert({ id, entity });
        }

        m_entitiesRemoved.clear();
    }

} // namespace systems
