#pragma once

#include "entities/Entity.hpp"

// Disable some compiler warnings that come from ctti
#if defined(_MSC_VER)
    #pragma warning(push)
    #pragma warning(disable : 4245)
#endif
#if defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wdeprecated-copy"
#endif
#include <ctti/type_id.hpp>
#if defined(__clang__)
    #pragma GCC diagnostic pop
#endif
#if defined(_MSC_VER)
    #pragma warning(pop)
#endif
#include <chrono>
#include <initializer_list>
#include <unordered_set>

namespace systems
{
    // --------------------------------------------------------------
    //
    // A system is where all logic associated with the game is handled.
    // Each system is specialized to operate over a particular set of
    // entities, handling things like movement, collision detection,
    // and rendering.
    //
    // --------------------------------------------------------------
    class System
    {
      public:
        System()
        {
        }

        System(const std::initializer_list<ctti::unnamed_type_id_t>& list) :
            m_interests(list)
        {
        }
        virtual ~System() {}

        virtual void clear() { m_entities.clear(); }
        virtual bool addEntity(entities::EntityPtr entity);
        virtual void removeEntity(entities::Entity::IdType entityId);
        virtual void updatedEntity(entities::EntityPtr entity);
        virtual void update([[maybe_unused]] std::chrono::microseconds elapsedTime) {}
        virtual void shutdown() {}

      protected:
        entities::EntityMap m_entities;

        virtual bool isInterested(const entities::EntityPtr& entity);

      private:
        std::unordered_set<ctti::unnamed_type_id_t> m_interests;
    };

} // namespace systems
