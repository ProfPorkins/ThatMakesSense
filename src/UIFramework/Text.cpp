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

#include "Text.hpp"

#include "services/Configuration.hpp"

namespace ui
{
    Text::Text(const Settings& options) :
        Element({ { options.left, options.top, 0, 0 } }),
        m_fontSize(options.fontSize),
        m_center(options.center)
    {
        m_text.setPosition({ options.left, options.top });
        m_text.setFont(*options.font);
        m_text.setCharacterSize(options.fontSize);
        m_text.setFillColor(options.fillColor);
        if (options.outlineColor.has_value())
        {
            m_text.setOutlineColor(options.outlineColor.value());
            m_text.setOutlineThickness(1.1f);
        }
        m_text.scale(Configuration::getGraphics().getScaleUI());
        //
        // Set the text string last, so the bounds of the element are set based on
        // the font, font size, etc settings.
        setText(options.label);
    }

    void Text::render(sf::RenderTarget& renderTarget)
    {
        if (m_visible)
        {
            renderTarget.draw(m_text);
        }
    }

    void Text::setPosition(const math::Point2f& corner)
    {
        Element::setPosition(corner);
        m_text.setPosition({ corner.x, corner.y });
    }

    void Text::setText(const std::string text)
    {
        m_text.setString(text);
        setRegion({ m_region.left, m_region.top, m_text.getGlobalBounds().width, m_text.getGlobalBounds().height });

        // After all that is done, it is okay to center it, if requested
        if (m_center)
        {
            this->setPosition({ -this->getRegion().width / 2.0f, this->getRegion().top });
        }
    }

    void Text::setScale(float scale)
    {
        m_text.setCharacterSize(static_cast<unsigned int>(m_fontSize * scale));
    }
} // namespace ui
