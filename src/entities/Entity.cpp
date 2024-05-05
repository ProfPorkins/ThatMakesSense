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

#include "Entity.hpp"

#include <memory>

namespace entities
{
    std::atomic_uint32_t Entity::nextId = 0;

    // --------------------------------------------------------------
    //
    // Used by the undo system so it can make copies of changed entities
    // to place on an undo stack, etc.
    //
    // --------------------------------------------------------------
    entities::EntityPtr Entity::clone()
    {
        auto clone = std::make_shared<Entity>();

        //
        // Need the exact same id, and o boy, be careful with the clone, don't mix
        // it with the live entities!
        clone->m_id = m_id;

        for (auto&& [id, component] : m_components)
        {
            auto&& [typeId, ptr] = component->clone();
            clone->m_components[typeId] = std::move(ptr);
        }

        return clone;
    }

    bool Entity::operator==(const Entity& rhs)
    {
        // First test, if number of components are different, they aren't equal
        bool areEqual{ this->m_components.size() == rhs.m_components.size() };

        if (areEqual)
        {
            for (auto&& [id, lhsComponent] : m_components)
            {
                if (rhs.m_components.contains(id))
                {
                    areEqual = areEqual && *(lhsComponent.get()) == *(rhs.m_components.at(id).get());
                }
                else
                {
                    areEqual = false;
                }
            }
        }

        return areEqual;
    }

    bool Entity::operator!=(const Entity& rhs)
    {
        return !(*this == rhs);
    }

} // namespace entities
