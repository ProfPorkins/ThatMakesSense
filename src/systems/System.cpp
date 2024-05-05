#include "System.hpp"

#include <algorithm>

namespace systems
{
    // --------------------------------------------------------------
    //
    // Each system maintains a list of entities it has a responsibility
    // to perform an update on.
    //
    // --------------------------------------------------------------
    bool System::addEntity(entities::EntityPtr entity)
    {
        if (isInterested(entity))
        {
            m_entities[entity->getId()] = entity;
            return true;
        }

        return false;
    }

    // --------------------------------------------------------------
    //
    // All systems must be given a chance to remove an entity.
    //
    // --------------------------------------------------------------
    void System::removeEntity(entities::Entity::IdType entityId)
    {
        m_entities.erase(entityId);
    }

    // --------------------------------------------------------------
    //
    // All systems are given a chance to review any entity that has
    // changes made to its components.
    //
    // --------------------------------------------------------------
    void System::updatedEntity(entities::EntityPtr entity)
    {
        if (m_entities.contains(entity->getId()))
        {
            if (!isInterested(entity))
            {
                m_entities.erase(entity->getId());
            }
        }
        else if (isInterested(entity))
        {
            m_entities[entity->getId()] = entity;
        }
    }

    // --------------------------------------------------------------
    //
    // All systems are asked if they are interested in an entity.  This
    // is to allow each system to have its own set of entities, making
    // traversal of them during update more efficient.
    //
    // --------------------------------------------------------------
    bool System::isInterested(const entities::EntityPtr& entity)
    {
        auto iCareIfAll = std::all_of(
            m_interests.begin(), m_interests.end(),
            [&entity](auto interest)
            {
                return entity->getComponents().contains(interest);
            });

        return iCareIfAll;
    }
} // namespace systems
