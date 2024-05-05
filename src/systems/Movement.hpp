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
#include "System.hpp"

#include <chrono>
#include <functional>
#include <optional>
#include <unordered_map>
#include <vector>

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system translates input events into entity movements.
    //
    // --------------------------------------------------------------
    class Movement : public System
    {
      public:
        Movement(std::shared_ptr<Level> level, std::function<void()> signalMovement, std::function<void(entities::Entity::IdType)> notifyUpdate);

        bool addEntity(entities::EntityPtr entity) override;
        void removeEntity(entities::Entity::IdType entityId) override;
        void updatedEntity(entities::EntityPtr entity) override;
        void update(std::chrono::microseconds elapsedTime) override;
        void shutdown() override;

      private:
        struct Move
        {
            entities::EntityPtr entity{ nullptr };
            misc::HexCoord position{ 0, 0 };
        };
        std::shared_ptr<Level> m_level;
        std::optional<misc::HexCoord::Direction> m_inputDirection;
        bool m_withPull{ false };
        std::function<void()> m_signalMovement;
        std::function<void(entities::Entity::IdType)> m_notifyUpdated;

        std::vector<std::uint32_t> m_keyboardInputHandlers;
        std::optional<std::uint32_t> m_controllerMoveHandler;

        float m_axisUpDown{ 0 };
        float m_axisLeftRight{ 0 };
        float m_axisLeftTrigger{ 0 };
        std::chrono::microseconds m_controllerInputWait{ 0 };

        bool m_anyMoved{ false };
        bool m_audioPlayed{ false };
        std::unordered_map<entities::Entity::IdType, Move> m_moves;
        std::unordered_map<entities::EntityPtr, misc::HexCoord> m_pulls;
        std::unordered_map<misc::HexCoord, entities::EntityPtr> m_sendByPosition;

        void registerKeyboardInput();
        void registerControllerInput();
        std::optional<misc::HexCoord::Direction> computeControllerDirection();

        void performMove(entities::EntityMap& entities);
        bool move(entities::EntityPtr entity, misc::HexCoord::Direction direction);
        void pull(misc::HexCoord position, misc::HexCoord::Direction toDirection, misc::HexCoord::Direction fromDirection);
        misc::HexCoord transformBySend(misc::HexCoord position, const misc::HexCoord::Direction& direction);
    };
} // namespace systems
