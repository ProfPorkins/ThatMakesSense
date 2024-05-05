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

#include "GameModel.hpp"

#include "components/Audio.hpp"
#include "components/Hint.hpp"
#include "components/Position.hpp"
#include "entities/Factory.hpp"
#include "misc/math.hpp"
#include "misc/misc.hpp"
#include "services/Audio.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"
#include "services/ControllerInput.hpp"
#include "services/KeyboardInput.hpp"
#include "services/Scoring.hpp"
#include "services/ThreadPool.hpp"
#include "systems/effects/BurnEffect.hpp"
#include "systems/effects/LevelCompletedEffect.hpp"
#include "systems/effects/NewPhraseEffect.hpp"
#include "systems/effects/RuleChangedEffect.hpp"
#include "systems/effects/SinkEffect.hpp"

#include <algorithm>
#include <cassert>
#include <latch>

// Static member implementations
std::shared_ptr<Level> GameModel::m_level{ nullptr };

// --------------------------------------------------------------
//
// This is where all game model initialization occurs.
//
// --------------------------------------------------------------
void GameModel::initialize()
{
    // Busy wait for the content to finish loading.  It's okay, it is probably loaded by the time this is even encountered
    while (Content::instance().anyPending())
        ;

    m_sysMovement = std::make_unique<systems::Movement>(
        m_level,
        [this]() // some movement occurred
        {
            m_sysUndo->signalStateChange();
            m_sysRuleSearch->signalStateChange();
        },
        [this](entities::Entity::IdType entityId) // notifyUpdated
        {
            m_updatedEntities.insert(entityId);
        });
    m_sysCompletion = std::make_unique<systems::Completion>(
        m_level,
        [this](const Scoring::ChallengeGroup& score)
        {
            levelComplete(score);
        },
        [this](const entities::Entity::IdType id)
        {
            return m_removeEntities.contains(id);
        });

    m_sysParticle = std::make_unique<systems::ParticleSystem>();
    m_sysAnimatedSprite = std::make_unique<systems::AnimatedSprite>();
    m_sysHint = std::make_unique<systems::Hint>(
        [this](entities::Entity::IdType entityId)
        {
            m_updatedEntities.insert(entityId);
        },
        [this](entities::Entity::IdType entityId)
        {
            removeEntity(entityId, systems::ParticleEffect::Effect::None);
        });
    m_sysChallenge = std::make_unique<systems::Challenge>(
        m_level,
        [this](entities::EntityPtr addMe) // addEntity
        {
            addEntity(addMe);
        },
        [this](entities::Entity::IdType entityId) // removeEntity
        {
            removeEntity(entityId, systems::ParticleEffect::Effect::None);
        });

    m_sysRendererHexGridOutline = std::make_unique<systems::RendererHexGridOutline>(m_level);
    m_sysRendererHexGridCoords = std::make_unique<systems::RendererHexGridCoords>(m_level);
    m_sysRendererHexGridAnimatedSprites = std::make_unique<systems::RendererHexGridAnimatedSprites>(m_level);
    m_sysRendererHexGridStaticSprites = std::make_unique<systems::RendererHexGridStaticSprites>(m_level);
    m_sysRendererPhraseDirection = std::make_unique<systems::RendererHexGridPhraseDirection>(m_level);
    m_sysRendererHexGridGoalHighlight = std::make_unique<systems::RendererHexGridGoalHighlight>(
        m_level,
        [this](entities::EntityPtr entity)
        {
            addEntity(entity);
        });
    m_sysRendererHexGridSendHighlight = std::make_unique<systems::RendererHexGridSendHighlight>(
        m_level,
        [this](entities::EntityPtr entity)
        {
            addEntity(entity);
        });
    m_sysRendererHexGridIHighlight = std::make_unique<systems::RendererHexGridIHighlight>(
        m_level,
        [this](entities::EntityPtr entity)
        {
            addEntity(entity);
        });
    m_sysRendererHint = std::make_unique<systems::RendererHint>();
    m_sysRendererChallenge = std::make_unique<systems::RendererChallenge>();
    m_sysRendererParticleSystem = std::make_unique<systems::RendererParticleSystem>();

    m_sysCamera = std::make_unique<systems::Camera>(
        m_level->getWidth(),
        m_level->getHeight(),
        [this](math::Vector2f diff)
        {
            m_sysParticle->signalCameraPan(diff);
        },
        [this]()
        {
            m_sysParticle->signalCameraZoom();
        });

    // Create the camera entity - initial postion/range come from the level itself
    auto camera = entities::createCamera(m_level->getCameraStartPos(), m_level->getCameraStartRange());
    // This is the only entity the camera system ever has, adding it here to save on wasting
    // time having it look at every entity added/removed during the game.
    m_sysCamera->addEntity(camera);
    m_sysRendererPhraseDirection->addEntity(camera);

    m_sysUndo = std::make_unique<systems::Undo>(
        [this]()
        {
            clearEntitiesWithPosition();
        },
        [this](entities::EntityPtr reviveMe)
        {
            m_sysRuleSearch->signalStateChange();
            addEntity(reviveMe);
        },
        [this](entities::Entity::IdType entityId)
        {
            m_sysRuleSearch->signalStateChange();
            removeEntity(entityId, systems::ParticleEffect::Effect::None);
        });
    // Tell the undo system to take a snapshot of the initial game state during its first update
    m_sysUndo->signalStateChange();

    m_sysRuleExecute = std::make_unique<systems::RuleExecute>(
        m_level,
        [this](entities::Entity::IdType entityId, systems::RuleExecute::RemoveReason reason) // removeEntity
        {
            m_sysUndo->signalStateChange();
            m_sysRuleSearch->signalStateChange();
            switch (reason)
            {
                case systems::RuleExecute::RemoveReason::Hot:
                    removeEntity(entityId, systems::ParticleEffect::Effect::EntityBurn);
                    break;
                case systems::RuleExecute::RemoveReason::Water:
                    removeEntity(entityId, systems::ParticleEffect::Effect::EntitySink);
                    break;
                case systems::RuleExecute::RemoveReason::Collateral:
                    removeEntity(entityId, systems::ParticleEffect::Effect::None);
                    break;
            }
        });

    m_sysRuleSearch = std::make_unique<systems::RuleSearch>(
        m_level,
        [this](entities::EntityPtr addMe) // addEntity
        {
            addEntity(addMe);
        },
        [this](entities::Entity::IdType entityId) // removeEntity
        {
            removeEntity(entityId, systems::ParticleEffect::Effect::None);
        },
        [this](entities::Entity::IdType entityId) // notifyUpdated
        {
            m_updatedEntities.insert(entityId);
        },
        [this](const entities::EntitySet& ids) // notifyGoalChanged
        {
            if (!m_ruleChangedSoundPlayed)
            {
                Audio::play(content::KEY_AUDIO_RULE_CHANGED, 70);
                m_ruleChangedSoundPlayed = true;
            }
            m_sysParticle->addEffect(std::make_unique<systems::RuleChangedEffect>(m_allEntities, ids, m_level->getWidth(), m_level->getHeight()));
        },
        [this](const entities::EntitySet& ids) // notifyIChanged
        {
            if (!m_ruleChangedSoundPlayed)
            {
                Audio::play(content::KEY_AUDIO_RULE_CHANGED, 70);
                m_ruleChangedSoundPlayed = true;
            }
            m_sysParticle->addEffect(std::make_unique<systems::RuleChangedEffect>(m_allEntities, ids, m_level->getWidth(), m_level->getHeight()));
        },
        [this](misc::HexCoord position) // notifyNewPhrase position
        {
            if (!m_ruleChangedSoundPlayed)
            {
                Audio::play(content::KEY_AUDIO_RULE_CHANGED, 70);
                m_ruleChangedSoundPlayed = true;
            }
            m_sysParticle->addEffect(std::make_unique<systems::NewPhraseEffect>(position, m_level->getWidth(), m_level->getHeight()));
        });

    // This has to come after creating all the systems, so that as the
    // entities are added (below), all the systems are up and ready to
    // do something with those entities
    m_level->initialize(
        [this](entities::EntityPtr entity)
        {
            addEntity(entity);
        });

    // This is called so that all the entities have been added to the various systems
    // before the first update occurs.
    addNewEntities();

    // Let's go ahead and discover and apply the rules before the first update
    m_sysRuleSearch->signalStateChange();
    m_sysRuleSearch->update(std::chrono::microseconds::zero());
}

// --------------------------------------------------------------
//
// Shutdown anything that doesn't automatically shutdown on its own,
// for example, background music.
//
// --------------------------------------------------------------
void GameModel::shutdown()
{
    m_sysCamera->shutdown();
    m_sysMovement->shutdown();
    m_sysCompletion->shutdown();
    m_sysAnimatedSprite->shutdown();
    m_sysHint->shutdown();
    m_sysChallenge->shutdown();

    m_sysRendererHexGridStaticSprites->shutdown();
    m_sysRendererHexGridAnimatedSprites->shutdown();
    m_sysRendererHexGridOutline->shutdown();
    m_sysRendererHexGridCoords->shutdown();
    m_sysRendererPhraseDirection->shutdown();
    m_sysRendererHexGridGoalHighlight->shutdown();
    m_sysRendererHexGridSendHighlight->shutdown();
    m_sysRendererHexGridIHighlight->shutdown();
    m_sysRendererHint->shutdown();
    m_sysRendererChallenge->shutdown();
    m_sysRendererParticleSystem->shutdown();

    m_sysUndo->shutdown();
    m_sysRuleExecute->shutdown();
    m_sysRuleSearch->shutdown();
}

// --------------------------------------------------------------
//
// This is where everything performs its update.
//
// --------------------------------------------------------------
void GameModel::update(const std::chrono::microseconds elapsedTime)
{
    std::latch graphDone{ 1 };

    auto taskGraph = ThreadPool::instance().createTaskGraph(
        [&graphDone]()
        {
            graphDone.count_down();
        });

    auto task1 = ThreadPool::instance().createTask(
        taskGraph,
        [this, elapsedTime]()
        {
            m_sysParticle->update(elapsedTime, m_sysCamera->getCamera());
        });
    auto task2 = ThreadPool::instance().createTask(
        taskGraph,
        [this, elapsedTime]()
        {
            m_sysHint->update(elapsedTime);
            m_sysChallenge->update(elapsedTime);
        });
    auto task3 = ThreadPool::instance().createTask(
        taskGraph,
        [this, elapsedTime]()
        {
            m_sysCamera->update(elapsedTime);
            m_sysMovement->update(elapsedTime);
        });
    auto task4 = ThreadPool::instance().createTask(
        taskGraph,
        [this, elapsedTime]()
        {
            m_sysAnimatedSprite->update(elapsedTime);
        });

    auto task5 = ThreadPool::instance().createTask(
        taskGraph,
        [this, elapsedTime]()
        {
            // Apply the rules after movement, but before discovering the new ones
            m_sysRuleExecute->update(elapsedTime);
            removeDeadEntities(false);
            notifyUpdatedEntities();
            addNewEntities();
        });

    auto task6 = ThreadPool::instance().createTask(
        taskGraph,
        [this, elapsedTime]()
        {
            // Once everything else is done, can perform a rules update
            m_sysRuleSearch->update(elapsedTime);
            // Here is the deal, updating and applying the rules can cause entities to be
            // updated, so we commit them before the undo system is invoked to ensure it
            // captures those changes too.
            notifyUpdatedEntities();

            // Apply the rules after discovering the new state
            // NOTE: It is possible that deleting a word, or words, causes
            // new rules to be created, that should then also be applied.  Therefore
            // have this code in a small loop to handle when that might happen.
            std::uint8_t maxIterations{ 0 };
            do
            {
                m_sysRuleExecute->update(elapsedTime);
                // The above can cause more dead entities, so have to get them removed before the undo system is invoked
                removeDeadEntities(false);
                // Removing entities, could also cause the rules to change, but don't need to
                // stay in a "rule execute -> rule search" loop until nothing changes, because
                // the rule execute can only remove rules, not result in new rules being created...
                m_sysRuleSearch->update(elapsedTime);
                maxIterations--;
            } while (m_removeEntities.size() > 0 && maxIterations > 0);

            // This needs to be done only after all the rules have been updated and executed
            m_sysCompletion->update(elapsedTime);

            //
            // Wait until everything is settled, then perform the undo
            bool undoActionTaken{ false };
            m_sysUndo->update(elapsedTime, undoActionTaken);
            //
            // If a reset was performed, a bunch of new entities are waiting to be added.  But if
            // we wait to add them until above, a black frame gets inserted during the rendering
            // because the renderer has nothing to draw.  Therefore, if undo was performed, go
            // ahead and commit all the new entities added, as a result of the undo operation, right now.
            // There is also a bad recursion that can take place with removing entities.  Some entities
            // can get removed from an undo, but we don't want the undo system to add them back in.
            //
            // TODO: Right now I have some logic in the game model that prevents this, which is bad that
            // it knows that much about the undo system, but that is the way it is for now.
            if (undoActionTaken)
            {
                removeDeadEntities(true);
                addNewEntities();
            }
        });

    taskGraph->declarePredecessor(task1->getId(), task3->getId()); // movement, completion systems can create effects
    taskGraph->declarePredecessor(task1->getId(), task4->getId());
    taskGraph->declarePredecessor(task3->getId(), task4->getId());

    taskGraph->declarePredecessor(task2->getId(), task5->getId());
    taskGraph->declarePredecessor(task3->getId(), task5->getId());
    taskGraph->declarePredecessor(task4->getId(), task5->getId());

    taskGraph->declarePredecessor(task5->getId(), task6->getId());
    taskGraph->declarePredecessor(task1->getId(), task6->getId()); // rule system can create effects

    ThreadPool::instance().submitTaskGraph(taskGraph);

    // Barrier placed here to wait until all tasks inside of graph complete, before exiting this method
    graphDone.wait();

    m_ruleChangedSoundPlayed = false;
}

// --------------------------------------------------------------
//
// This method is intended to be passed around to various systems as
// they create entities.  It collects all the newly created entities
// and then they get added to the systems during the gameloop.
//
// --------------------------------------------------------------
void GameModel::addEntity(entities::EntityPtr entity)
{
    std::lock_guard<std::mutex> lock(m_mutexEntities);

    m_newEntities.push_back(entity);
}

// --------------------------------------------------------------
//
// Same as above, but for removing entities.
//
// --------------------------------------------------------------
void GameModel::removeEntity(entities::Entity::IdType id, systems::ParticleEffect::Effect effect)
{
    std::lock_guard<std::mutex> lock(m_mutexEntities);

    assert(m_allEntities.contains(id));

    if (m_allEntities[id]->hasComponent<components::Position>())
    {
        auto& position = m_allEntities[id]->getComponent<components::Position>()->get();
        switch (effect)
        {
            case systems::ParticleEffect::Effect::EntityBurn:
                Audio::play(content::KEY_AUDIO_BURN);
                m_sysParticle->addEffect(std::make_unique<systems::BurnEffect>(position, m_level->getWidth(), m_level->getHeight()));
                break;
            case systems::ParticleEffect::Effect::EntitySink:
                Audio::play(content::KEY_AUDIO_SINK);
                m_sysParticle->addEffect(std::make_unique<systems::SinkEffect>(position, m_level->getWidth(), m_level->getHeight()));
                break;
            default:
                // do nothing
                break;
        }
    }

    m_removeEntities.insert(id);
}

// ------------------------------------------------------------------
//
// When a reset is performed by the Undo system, we want to remove all
// entities with a position component, because those are the only ones
// tracked by the Undo system.  Things like hints are not tracked by
// undo and shouldn't be reset, as they aren't part of gameply.
//
// ------------------------------------------------------------------
void GameModel::clearEntitiesWithPosition()
{
    static const auto hasPosition = [](const std::pair<entities::Entity::IdType, entities::EntityPtr>& entity)
    {
        return entity.second->hasComponent<components::Position>();
    };

    for (auto&& removeMe : m_allEntities)
    {
        if (hasPosition(removeMe))
        {
            m_level->removeEntity(removeMe.first);
            m_sysMovement->removeEntity(removeMe.first);
            m_sysCompletion->removeEntity(removeMe.first);
            m_sysAnimatedSprite->removeEntity(removeMe.first);
            m_sysHint->removeEntity(removeMe.first);
            m_sysChallenge->removeEntity(removeMe.first);

            m_sysRendererHexGridOutline->removeEntity(removeMe.first);
            m_sysRendererHexGridCoords->removeEntity(removeMe.first);
            m_sysRendererHexGridAnimatedSprites->removeEntity(removeMe.first);
            m_sysRendererHexGridStaticSprites->removeEntity(removeMe.first);
            m_sysRendererPhraseDirection->removeEntity(removeMe.first);
            m_sysRendererHexGridGoalHighlight->removeEntity(removeMe.first);
            m_sysRendererHexGridSendHighlight->removeEntity(removeMe.first);
            m_sysRendererHexGridIHighlight->removeEntity(removeMe.first);
            m_sysRendererHint->removeEntity(removeMe.first);
            m_sysRendererChallenge->removeEntity(removeMe.first);

            m_sysUndo->removeEntity(removeMe.first);
            m_sysRuleExecute->removeEntity(removeMe.first);
            m_sysRuleSearch->removeEntity(removeMe.first);
        }
    }

    // Now we need to remove them from the master container
    std::erase_if(m_allEntities, hasPosition);

    m_newEntities.clear();
    m_removeEntities.clear();
    m_updatedEntities.clear();
}

void GameModel::clearAllEntities()
{
    m_allEntities.clear();
    m_newEntities.clear();
    m_removeEntities.clear();
    m_updatedEntities.clear();

    m_level->clear();
    m_sysMovement->clear();
    m_sysCompletion->clear();
    m_sysAnimatedSprite->clear();
    m_sysHint->clear();
    m_sysChallenge->clear();

    m_sysRendererHexGridOutline->clear();
    m_sysRendererHexGridCoords->clear();
    m_sysRendererHexGridAnimatedSprites->clear();
    m_sysRendererHexGridStaticSprites->clear();
    m_sysRendererPhraseDirection->clear();
    m_sysRendererHexGridGoalHighlight->clear();
    m_sysRendererHexGridSendHighlight->clear();
    m_sysRendererHexGridIHighlight->clear();
    m_sysRendererHint->clear();
    m_sysRendererChallenge->clear();

    m_sysUndo->clear();
    m_sysRuleExecute->clear();
    m_sysRuleSearch->clear();
}

// --------------------------------------------------------------
//
// Give all (most) systems a chance to see if they are interested
// in the new entities, along with the level, as it does some
// tracking of its own.
//
// --------------------------------------------------------------
void GameModel::addNewEntities()
{
    for (auto&& entity : m_newEntities)
    {
        // Even though the level isn't a system, it still tracks entities
        m_level->addEntity(entity);

        m_allEntities[entity->getId()] = entity;

        // NOTE: m_sysCamera only has a camera entity, does not need to execute here
        m_sysMovement->addEntity(entity);
        m_sysCompletion->addEntity(entity);
        m_sysAnimatedSprite->addEntity(entity);
        m_sysHint->addEntity(entity);
        m_sysChallenge->addEntity(entity);

        m_sysRendererHexGridAnimatedSprites->addEntity(entity);
        m_sysRendererHexGridStaticSprites->addEntity(entity);
        m_sysRendererPhraseDirection->addEntity(entity);
        m_sysRendererHexGridGoalHighlight->addEntity(entity);
        m_sysRendererHexGridSendHighlight->addEntity(entity);
        m_sysRendererHexGridIHighlight->addEntity(entity);
        m_sysRendererHint->addEntity(entity);
        m_sysRendererChallenge->addEntity(entity);
        // NOTE: The following systems don't track any entities, they get them all from
        //       the level, therefore, do not need to be called here.
        //
        //       m_sysRendererHexGridOutline
        //       m_sysRendererHexGridCoords

        // NOTE: Particle system renderer does not have entities added to it, it is it's own separate thing

        m_sysUndo->addEntity(entity);
        m_sysRuleSearch->addEntity(entity);
        m_sysRuleSearch->addEntity(entity);
    }
    m_newEntities.clear();
}

// --------------------------------------------------------------
//
// Give all (most) systems a chance to remove the entities that
// are no longer part of the model.
//
// --------------------------------------------------------------
void GameModel::removeDeadEntities(bool undoAction)
{
    for (auto&& entityId : m_removeEntities)
    {
        m_level->removeEntity(entityId);

        m_allEntities.erase(entityId);

        // NOTE: m_sysCamera only has a camera entity, does not need to execute here
        m_sysMovement->removeEntity(entityId);
        m_sysCompletion->removeEntity(entityId);
        m_sysAnimatedSprite->removeEntity(entityId);
        m_sysHint->removeEntity(entityId);
        m_sysChallenge->removeEntity(entityId);

        m_sysRendererHexGridAnimatedSprites->removeEntity(entityId);
        m_sysRendererHexGridStaticSprites->removeEntity(entityId);
        m_sysRendererPhraseDirection->removeEntity(entityId);
        m_sysRendererHexGridGoalHighlight->removeEntity(entityId);
        m_sysRendererHexGridSendHighlight->removeEntity(entityId);
        m_sysRendererHexGridIHighlight->removeEntity(entityId);
        m_sysRendererHint->removeEntity(entityId);
        m_sysRendererChallenge->removeEntity(entityId);
        // NOTE: The following systems don't track any entities, they get them all from
        //       the level, therefore, do not need to be called here.
        //
        //       m_sysRendererHexGridOutline
        //       m_sysRendererHexGridCoords

        if (!undoAction)
        {
            m_sysUndo->removeEntity(entityId);
        }
        m_sysRuleExecute->removeEntity(entityId);
        m_sysRuleSearch->removeEntity(entityId);

        // In case this entity was also considered updated, remove it from the updated entities
        m_updatedEntities.erase(entityId);
    }
    m_removeEntities.clear();
}

// --------------------------------------------------------------
//
// Give all (most) systems a chance to do something when an entity
// has an update of some kind.
//
// --------------------------------------------------------------
void GameModel::notifyUpdatedEntities()
{
    for (auto&& entityId : m_updatedEntities)
    {
        // There are some cases where entities get removed and updated at the same time.  It works
        // out to be "easiest" to just check here to verify the updated entity exists before telling
        // everyone about it.
        if (m_allEntities.contains(entityId))
        {
            // NOTE: m_sysCamera only has a camera entity, does not need to execute here
            m_sysMovement->updatedEntity(m_allEntities[entityId]);
            m_sysCompletion->updatedEntity(m_allEntities[entityId]);
            m_sysAnimatedSprite->updatedEntity(m_allEntities[entityId]);
            m_sysHint->updatedEntity(m_allEntities[entityId]);
            m_sysChallenge->updatedEntity(m_allEntities[entityId]);

            m_sysRendererHexGridAnimatedSprites->updatedEntity(m_allEntities[entityId]);
            m_sysRendererHexGridStaticSprites->updatedEntity(m_allEntities[entityId]);
            m_sysRendererPhraseDirection->updatedEntity(m_allEntities[entityId]);
            m_sysRendererHexGridGoalHighlight->updatedEntity(m_allEntities[entityId]);
            m_sysRendererHexGridSendHighlight->updatedEntity(m_allEntities[entityId]);
            m_sysRendererHexGridIHighlight->updatedEntity(m_allEntities[entityId]);
            m_sysRendererHint->updatedEntity(m_allEntities[entityId]);
            m_sysRendererChallenge->updatedEntity(m_allEntities[entityId]);

            // NOTE: The following systems don't track any entities, they get them all from
            //       the level, therefore, do not need to be called here.
            //
            //       m_sysRendererHexGridOutline
            //       m_sysRendererHexGridCoords

            m_sysUndo->updatedEntity(m_allEntities[entityId]);
            m_sysRuleExecute->updatedEntity(m_allEntities[entityId]);
            m_sysRuleSearch->updatedEntity(m_allEntities[entityId]);
        }
    }
    m_updatedEntities.clear();
}

// --------------------------------------------------------------
//
// Upon level completion, remove the ability to control the character
// and create the end of level particle effect.
//
// --------------------------------------------------------------
void GameModel::levelComplete(const Scoring::ChallengeGroup& score)
{
    // This flag prevents the end of level effect from continuing to be added to the particle system
    if (!m_complete)
    {
        m_complete = true;
        unregisterInputHandlers();
        Audio::play(content::KEY_AUDIO_LEVEL_COMPLETE);

        // Find out which challenge was met, then let the user know through the hint
        auto challenge = m_level->matchChallenge(score);
        if (challenge.has_value())
        {
            m_sysParticle->addEffect(std::make_unique<systems::LevelCompletedEffect>(m_level, systems::LevelCompletedEffect::Type::PreDefined));
            auto hint = std::make_shared<entities::Entity>();
            hint->addComponent(std::make_unique<components::Hint>("Challenge Complete - " + Scoring::formatChallengeFriendly(challenge.value()), misc::msTous(std::chrono::milliseconds(10000)), true));
            addEntity(hint);
        }
        else
        {
            m_sysParticle->addEffect(std::make_unique<systems::LevelCompletedEffect>(m_level, systems::LevelCompletedEffect::Type::BeyondCategory));
            auto hint = std::make_shared<entities::Entity>();
            hint->addComponent(std::make_unique<components::Hint>("You Found A Challenge!", misc::msTous(std::chrono::milliseconds(10000)), true));
            addEntity(hint);
        }

        // After the challenge hint is done, let the user know how to continue
        if (ControllerInput::instance().hasControllerBeenUsed())
        {
            if (ControllerInput::instance().whichControllerVendorUsed() == ControllerInput::VENDOR_ID_MICROSOFT)
            {
                std::string msgContinue = "{ Y } Replay { B } Leave";
                if (Content::getLevels().getNextLevel(m_level->getUUID()) != nullptr)
                {
                    msgContinue = "{ A } Next " + msgContinue;
                }
                auto hint = std::make_shared<entities::Entity>();
                hint->addComponent(std::make_unique<components::Hint>(msgContinue, misc::msTous(std::chrono::milliseconds(500000))));
                addEntity(hint);
            }
            else // Not a great default, but defaulting to Sony mapping
            {
                std::string msgContinue = "{ Triangle } Replay { Circle } Leave";
                if (Content::getLevels().getNextLevel(m_level->getUUID()) != nullptr)
                {
                    msgContinue = "{ X } Next " + msgContinue;
                }
                auto hint = std::make_shared<entities::Entity>();
                hint->addComponent(std::make_unique<components::Hint>(msgContinue, misc::msTous(std::chrono::milliseconds(500000))));
                addEntity(hint);
            }
        }
        else
        {
            std::string msgContinue = "'ENTER' Replay, 'ESC' Leave";
            if (Content::getLevels().getNextLevel(m_level->getUUID()) != nullptr)
            {
                msgContinue = "'SPACE' Next, " + msgContinue;
            }
            auto hint = std::make_shared<entities::Entity>();
            hint->addComponent(std::make_unique<components::Hint>(msgContinue, misc::msTous(std::chrono::milliseconds(500000))));
            addEntity(hint);
        }

        Scoring::instance().recordScore(m_level, score,
                                        [this]
                                        {
                                            // Ugh, this is a hack, but it does the job without doing something much
                                            // more complex
                                            m_sysChallenge->levelComplete();
                                        });
        m_notifyComplete(m_level->getUUID());
    }
}

// --------------------------------------------------------------
//
// All rendering takes place here.
//
// --------------------------------------------------------------
void GameModel::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
{
    // renderTarget.clear(sf::Color::White);
    renderTarget.clear(sf::Color::Black);
    // renderTarget.clear(sf::Color(238, 228, 253));

    m_sysRendererHexGridStaticSprites->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererHexGridAnimatedSprites->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererHexGridOutline->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererHexGridCoords->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererPhraseDirection->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererHexGridGoalHighlight->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererHexGridSendHighlight->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererHexGridIHighlight->update(elapsedTime, renderTarget, m_sysCamera->getCamera());
    m_sysRendererHint->update(elapsedTime, renderTarget);
    m_sysRendererChallenge->update(elapsedTime, renderTarget);
    m_sysRendererParticleSystem->update(*m_sysParticle, renderTarget);
}

// --------------------------------------------------------------
//
// Unregister the handlers setup for the game model.  It is unfortunate
// we have knowledge these are the systems to shutdown, but that's
// what I have right now.
//
// --------------------------------------------------------------
void GameModel::unregisterInputHandlers()
{
    m_sysMovement->shutdown();
    m_sysUndo->shutdown();
}
