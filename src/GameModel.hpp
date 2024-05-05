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

#pragma once

#include "Level.hpp"
#include "systems/AnimatedSprite.hpp"
#include "systems/Camera.hpp"
#include "systems/Challenge.hpp"
#include "systems/Completion.hpp"
#include "systems/Hint.hpp"
#include "systems/Movement.hpp"
#include "systems/ParticleSystem.hpp"
#include "systems/RendererChallenge.hpp"
#include "systems/RendererHexGridAnimatedSprites.hpp"
#include "systems/RendererHexGridCoords.hpp"
#include "systems/RendererHexGridGoalHighlight.hpp"
#include "systems/RendererHexGridIHighlight.hpp"
#include "systems/RendererHexGridOutline.hpp"
#include "systems/RendererHexGridPhraseDirection.hpp"
#include "systems/RendererHexGridSendHighlight.hpp"
#include "systems/RendererHexGridStaticSprites.hpp"
#include "systems/RendererHint.hpp"
#include "systems/RendererParticleSystem.hpp"
#include "systems/RuleExecute.hpp"
#include "systems/RuleSearch.hpp"
#include "systems/Undo.hpp"

#include <SFML/Audio/Sound.hpp>
#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <chrono>
#include <cstdint>
#include <functional>
#include <map>
#include <memory>
#include <mutex>
#include <unordered_set>
#include <vector>

class GameModel
{
  public:
    GameModel(std::function<void(const std::string&)> notifyComplete) :
        m_notifyComplete(notifyComplete)
    {
    }

    void initialize();
    void shutdown();
    static void selectLevel(std::shared_ptr<Level> level) { m_level = level; }

    void update(const std::chrono::microseconds elapsedTime);
    void render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime);

  private:
    static std::shared_ptr<Level> m_level;
    std::function<void(const std::string&)> m_notifyComplete;
    bool m_complete{ false };

    std::unique_ptr<systems::ParticleSystem> m_sysParticle;
    std::unique_ptr<systems::Camera> m_sysCamera;
    std::unique_ptr<systems::Movement> m_sysMovement;
    std::unique_ptr<systems::Completion> m_sysCompletion;
    std::unique_ptr<systems::Undo> m_sysUndo;
    std::unique_ptr<systems::RuleExecute> m_sysRuleExecute;
    std::unique_ptr<systems::RuleSearch> m_sysRuleSearch;
    std::unique_ptr<systems::AnimatedSprite> m_sysAnimatedSprite;
    std::unique_ptr<systems::Hint> m_sysHint;
    std::unique_ptr<systems::Challenge> m_sysChallenge;

    std::unique_ptr<systems::RendererHexGridStaticSprites> m_sysRendererHexGridStaticSprites;
    std::unique_ptr<systems::RendererHexGridAnimatedSprites> m_sysRendererHexGridAnimatedSprites;
    std::unique_ptr<systems::RendererHexGridOutline> m_sysRendererHexGridOutline;
    std::unique_ptr<systems::RendererHexGridCoords> m_sysRendererHexGridCoords;
    std::unique_ptr<systems::RendererHexGridPhraseDirection> m_sysRendererPhraseDirection;
    std::unique_ptr<systems::RendererHexGridGoalHighlight> m_sysRendererHexGridGoalHighlight;
    std::unique_ptr<systems::RendererHexGridSendHighlight> m_sysRendererHexGridSendHighlight;
    std::unique_ptr<systems::RendererHexGridIHighlight> m_sysRendererHexGridIHighlight;
    std::unique_ptr<systems::RendererHint> m_sysRendererHint;
    std::unique_ptr<systems::RendererChallenge> m_sysRendererChallenge;
    std::unique_ptr<systems::RendererParticleSystem> m_sysRendererParticleSystem;

    std::mutex m_mutexEntities;
    entities::EntityMap m_allEntities;
    std::vector<entities::EntityPtr> m_newEntities;
    std::unordered_set<entities::Entity::IdType> m_removeEntities;
    std::unordered_set<entities::Entity::IdType> m_updatedEntities;
    bool m_ruleChangedSoundPlayed{ false };

    void addEntity(entities::EntityPtr entity);
    void removeEntity(entities::Entity::IdType id, systems::ParticleEffect::Effect effect);
    void clearEntitiesWithPosition();
    void clearAllEntities();
    void addNewEntities();
    void removeDeadEntities(bool undoAction);
    void notifyUpdatedEntities();
    void levelComplete(const Scoring::ChallengeGroup& score);

    void unregisterInputHandlers();
};
