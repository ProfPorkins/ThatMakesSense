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
#include "entities/Entity.hpp"

#include <optional>
#include <stack>

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system handles the ability to perform undo during gameplay
    //
    // --------------------------------------------------------------
    class Undo : public System
    {
      public:
        Undo(std::function<void()> signalReset, std::function<void(entities::EntityPtr)> addEntity, std::function<void(entities::Entity::IdType)> removeEntity);

        void removeEntity(entities::Entity::IdType entityId) override;
        void update(std::chrono::microseconds elapsedTime, bool& actionTaken);
        void shutdown() override;

        void signalStateChange() { m_takeSnapshot = true; }

      protected:
        virtual bool isInterested(const entities::EntityPtr& entity) override;

      private:
        using System::update; // disables compiler warning from clang

        class StackFrame
        {
          public:
            entities::EntityMap entitiesNew;
            entities::EntityMap entitiesPrevious;
            entities::EntityVector entitiesRemoved;
        };

        std::stack<StackFrame> m_stack;
        entities::EntityMap m_surface;

        std::function<void()> funcSignalReset;
        std::function<void(entities::EntityPtr)> funcAddEntity;
        std::function<void(entities::Entity::IdType)> funcRemoveEntity;
        bool m_performUndo{ false };
        bool m_takeSnapshot{ false };
        bool m_performReset{ false };
        entities::EntityVector m_entitiesRemoved;
        std::optional<std::uint32_t> m_keyboardUndoHandlerId{ 0 };
        std::optional<std::uint32_t> m_keyboardResetHandlerId{ 0 };
        std::optional<std::uint32_t> m_controllerUndoHandlerId{ 0 };
        std::optional<std::uint32_t> m_controllerResetHandlerId{ 0 };

        void performUndo();
        void performReset();
    };
} // namespace systems
