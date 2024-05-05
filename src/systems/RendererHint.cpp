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

#include "RendererHint.hpp"

#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"

#include <cassert>
#include <cmath>

namespace systems
{
    RendererHint::RendererHint() :
        System({ ctti::unnamed_type_id<components::Hint>() })
    {
        ui::Text::Settings settings{
            false,
            0, 0,
            "",
            Content::get<sf::Font>(content::KEY_FONT_HINT),
            Configuration::get<sf::Color>(config::FONT_HINT_COLOR_FILL),
            Configuration::get<sf::Color>(config::FONT_HINT_COLOR_OUTLINE),
            Configuration::get<std::uint8_t>(config::FONT_HINT_SIZE)
        };
        m_textHint = std::make_unique<ui::Text>(settings);

        m_showHints = Configuration::get<bool>(config::OPTIONS_SHOW_HINTS);
    }

    // --------------------------------------------------------------
    //
    // We are only interested in rendering the active hint, and there
    // better only be one!
    //
    // --------------------------------------------------------------
    bool RendererHint::isInterested(const entities::EntityPtr& entity)
    {
        return System::isInterested(entity) && entity->getComponent<components::Hint>()->isActive();
    }

    void RendererHint::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, sf::RenderTarget& renderTarget)
    {
        assert(m_entities.size() == 0 || m_entities.size() == 1);

        if (m_showHints && m_entities.size() > 0)
        {
            auto hint = m_entities.begin()->second->getComponent<components::Hint>();
            switch (hint->getState())
            {
                case components::Hint::State::In:
                    m_textHint->setScale(std::lerp(0.0f, 1.0f, static_cast<float>(hint->getTimeInState().count()) / components::Hint::HINT_TRANSITION_TIME.count()));
                    break;
                case components::Hint::State::Steady:
                    // Nothing to do, purposely left blank
                    break;
                case components::Hint::State::Out:
                    m_textHint->setScale(std::lerp(1.0f, 0.0f, static_cast<float>(hint->getTimeInState().count()) / components::Hint::HINT_TRANSITION_TIME.count()));
                    break;
            }

            m_textHint->setText(hint->get());
            // Center in the window, near the top
            m_textHint->setPosition(
                { -(m_textHint->getRegion().width / 2.0f),
                  -(Configuration::getGraphics().getViewCoordinates().height * 0.5f) + m_textHint->getRegion().height * 0.5f });
            m_textHint->render(renderTarget);
        }
    }

} // namespace systems