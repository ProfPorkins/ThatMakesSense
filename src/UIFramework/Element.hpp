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

#include "misc/math.hpp"

#include <SFML/Graphics.hpp>
#include <chrono>
#include <memory>

namespace ui
{
    // --------------------------------------------------------------
    //
    // This is the base class for any GUI thing in the user interface.
    // It holds basic things such as postion, visibility, update, and
    // rendering functionality.
    //
    // --------------------------------------------------------------
    class Element
    {
      public:
        class Settings
        {
          public:
            math::Rectangle2f region;
            bool visible = true;
        };

        Element(const Settings& settings) :
            m_region(settings.region),
            m_visible(settings.visible)
        {
        }
        virtual ~Element() { stop(); }

        virtual void start() {}
        virtual void stop() {}

        virtual void onKeyPressed([[maybe_unused]] sf::Keyboard::Key key){};
        virtual void onMouseMoved([[maybe_unused]] math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime){};
        virtual void onMouseReleased([[maybe_unused]] sf::Mouse::Button button, [[maybe_unused]] math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime){};

        virtual void update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now){};
        virtual void render([[maybe_unused]] sf::RenderTarget& renderTarget){};

        virtual void setActive() {}
        virtual void setInactive() {}
        virtual bool isActive() { return false; }

        inline void show() { m_visible = true; }
        inline void hide() { m_visible = false; }
        inline bool isVisible() { return m_visible; }

        const math::Rectangle2f& getRegion() { return m_region; }
        virtual void setRegion(const math::Rectangle2f& region) { m_region = region; }
        virtual void setPosition(const math::Point2f& corner)
        {
            m_region.left = corner.x;
            m_region.top = corner.y;
        }

      protected:
        math::Rectangle2f m_region;
        bool m_visible;
    };
} // namespace ui