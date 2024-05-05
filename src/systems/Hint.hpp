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

#include "System.hpp"
#include "components/Hint.hpp"
#include "entities/Entity.hpp"

#include <chrono>
#include <deque>
#include <functional>

namespace systems
{
    // --------------------------------------------------------------
    //
    // This system displays hints to help the user
    //
    // --------------------------------------------------------------
    class Hint : public System
    {
      public:
        Hint(std::function<void(entities::Entity::IdType)> notifyUpdated, std::function<void(entities::Entity::IdType)> notifyRemove) :
            System({ ctti::unnamed_type_id<components::Hint>() }),
            m_notifyUpdated(notifyUpdated),
            m_notifyRemove(notifyRemove)
        {
        }

        void clear() override;
        bool addEntity(entities::EntityPtr entity) override;
        void update(std::chrono::microseconds elapsedTime) override;
        void shutdown() override;

      private:
        std::function<void(entities::Entity::IdType)> m_notifyUpdated;
        std::function<void(entities::Entity::IdType)> m_notifyRemove;
        std::deque<entities::EntityPtr> m_hints;
    };
} // namespace systems
