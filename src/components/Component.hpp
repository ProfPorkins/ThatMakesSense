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
#include <memory>
#include <tuple>
#include <typeinfo>

// --------------------------------------------------------------
//
// The purpose of this class is to provide a common base type that
// can be referenced by the `Entity` class for any derived type.
//
// --------------------------------------------------------------
namespace components
{
    //
    // Reference: https://stackoverflow.com/questions/24249327/how-to-see-if-two-polymorphic-objects-are-equal
    //
    // Using this technique to avoid having to use dynamic_cast, which is relatively slow
    template <typename Base, typename Derived>
    struct PolymorphicComparable : public Base
    {
        bool operator==(Base& rhs)
        {
            if (typeid(rhs) != typeid(Derived))
            {
                return false;
            }

            Derived& a = static_cast<Derived&>(*this);
            Derived& b = static_cast<Derived&>(rhs);

            // clang prefer to have this, instead of "a == b", it throws a warning otherwise
            return a.operator==(b);
        }
    };

    class Component
    {
      public:
        // Components have to be able to clone themselves - needed for the undo capability
        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() = 0;

        virtual ~Component() {}

        virtual bool operator==([[maybe_unused]] Component& rhs) = 0;
    };

} // namespace components
