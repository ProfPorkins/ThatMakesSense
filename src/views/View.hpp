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

#include "ViewState.hpp"
#include "misc/math.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window/Event.hpp>
#include <chrono>
#include <memory>

namespace views
{
    // --------------------------------------------------------------
    //
    // A View represents the state and rendering into the main window.
    // Each view is responsible for its own input handling, updating,
    // and rendering.
    //
    // --------------------------------------------------------------
    class View
    {
      public:
        virtual ~View() = default;
        virtual bool start() { return true; }
        virtual void stop() {}

        virtual ViewState update(const std::chrono::microseconds elapsedTime, const std::chrono::system_clock::time_point now) = 0;
        virtual void render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime) = 0;
    };
} // namespace views
