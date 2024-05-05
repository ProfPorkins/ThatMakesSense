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

#include "HelpDisplay.hpp"

#include "services/Configuration.hpp"

namespace views
{
    HelpDisplay::HelpDisplay(ViewState viewState, std::string keyHelpImage) :
        m_viewState(viewState),
        m_keyHelpImage(keyHelpImage)
    {
    }

    bool HelpDisplay::start()
    {
        MenuView::start();
        m_nextState = m_viewState;

        auto texFilled = Content::get<sf::Texture>(m_keyHelpImage);
        m_helpImage.setTexture(*texFilled);
        m_helpImage.setOrigin({ texFilled->getSize().x / 2.0f, texFilled->getSize().y / 2.0f });
        m_helpImage.setScale(math::getViewScale({ 60.0f, 80.0f }, m_helpImage.getTexture()));
        m_helpImage.setPosition({ 0, 0 });

        return true;
    }

    void HelpDisplay::stop()
    {
        MenuView::stop();
    }

    ViewState HelpDisplay::update([[maybe_unused]] const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        MenuView::update(elapsedTime, now);

        return m_nextState;
    }

    void HelpDisplay::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        MenuView::render(renderTarget, elapsedTime);

        renderTarget.draw(m_helpImage);
    }

    void HelpDisplay::navigateBack()
    {
        m_nextState = ViewState::HelpControls;
    }
} // namespace views
