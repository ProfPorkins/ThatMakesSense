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

#include "Component.hpp"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace components
{
    enum class VerbType : std::uint8_t
    {
        Is,
        Am,
        Can,
        And
    };

    static const std::unordered_map<VerbType, std::string> VerbTypeToString{
        { VerbType::Is, "is" },
        { VerbType::Am, "am" },
        { VerbType::Can, "can" },
        { VerbType::And, "and" },
    };

    class Verb : public PolymorphicComparable<Component, Verb>
    {
      public:
        Verb(VerbType type) :
            m_type(type)
        {
        }

        auto get() { return m_type; }

        virtual std::tuple<ctti::unnamed_type_id_t, std::unique_ptr<Component>> clone() override
        {
            return { ctti::unnamed_type_id<Verb>(), std::make_unique<Verb>(m_type) };
        }

        bool operator==(Verb& rhs)
        {
            return m_type == rhs.m_type;
        }

      private:
        VerbType m_type;
    };
} // namespace components
