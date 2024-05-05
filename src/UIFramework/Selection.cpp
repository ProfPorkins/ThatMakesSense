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

#include "Selection.hpp"

#include "services/Content.hpp"

#include <algorithm> // std::max

namespace ui
{
    Selection::Selection(const Settings& settings) :
        Element({ { settings.left, settings.top, 0, 0 } }),
        m_options(settings.options),
        m_selectedOption(settings.selectedOption),
        m_handler(settings.handler),
        m_inactiveLabel({ false, 0, 0, settings.label, settings.font, settings.fillColor, settings.outlineColor, settings.fontSize }),
        m_activeLabel({ false, 0, 0, "~" + settings.label + "~", settings.font, settings.activeColor, settings.activeOutline, settings.fontSize }),
        m_displayOption({ false, 0, 0, "", settings.font, settings.selectionColor, settings.selectionOutline, settings.fontSize })
    {
        m_inactiveLabel.setPosition({ -m_inactiveLabel.getRegion().width / 2.0f, settings.top });
        m_activeLabel.setPosition({ -m_activeLabel.getRegion().width / 2.0f, settings.top });
        m_displayLabel = &m_inactiveLabel;

        updateSelectionText();
        setRegion({
            m_displayOption.getRegion().left,
            settings.top,
            std::max(m_inactiveLabel.getRegion().width, m_displayOption.getRegion().width),
            (m_inactiveLabel.getRegion().height + m_displayOption.getRegion().height) * 1.5f // TODO: That dang 1.5f
        });
    }

    void Selection::onKeyPressed(sf::Keyboard::Key key)
    {
        switch (key)
        {
            case sf::Keyboard::Left:
                selectLeft();
                break;
            case sf::Keyboard::Right:
                selectRight();
                break;
            default: // Needed to eliminate unused enumeration compiler warning
                break;
        }
    }

    void Selection::onMouseReleased([[maybe_unused]] sf::Mouse::Button button, math::Point2f point, [[maybe_unused]] std::chrono::microseconds elapsedTime)
    {
        //
        // If left half, move left, if right half, move right
        if (this->getRegion().contains(point))
        {
            auto middleX = this->getRegion().left + this->getRegion().width * 0.5f;
            if (point.x <= middleX)
            {
                selectLeft();
            }
            else
            {
                selectRight();
            }
        }
    }

    void Selection::render(sf::RenderTarget& renderTarget)
    {
        if (this->isVisible())
        {
            m_displayLabel->render(renderTarget);
            m_displayOption.render(renderTarget);
        }
    }

    void Selection::setActive()
    {
        Activate::setActive();
        m_displayLabel = &m_activeLabel;
    }
    void Selection::setInactive()
    {
        Activate::setInactive();
        m_displayLabel = &m_inactiveLabel;
    }

    void Selection::selectLeft()
    {
        m_selectedOption = (m_selectedOption == 0) ? m_options.size() - 1 : m_selectedOption - 1;
        updateSelectionText();
        m_handler(std::get<1>(m_options[m_selectedOption]));
    }

    void Selection::selectRight()
    {
        m_selectedOption = (m_selectedOption + 1) % m_options.size();
        updateSelectionText();
        m_handler(std::get<1>(m_options[m_selectedOption]));
    }

    void Selection::updateSelectionText()
    {
        m_displayOption.setText("< " + std::get<0>(m_options[m_selectedOption]) + " >");
        m_displayOption.setPosition({
            -m_displayOption.getRegion().width / 2.0f,
            m_region.top + m_inactiveLabel.getRegion().height * 1.5f,
        });
    }
} // namespace ui