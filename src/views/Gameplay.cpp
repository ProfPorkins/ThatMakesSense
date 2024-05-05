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

#include "Gameplay.hpp"

#include "services//ControllerInput.hpp"
#include "services/Content.hpp"
#include "services/KeyboardInput.hpp"

namespace views
{
    bool Gameplay::start()
    {
        m_nextState = ViewState::GamePlay;
        m_levelComplete = false;

        //
        // Go ahead and get the game model created and initialize it so it can start
        // loading content immediately.
        m_model = std::make_unique<GameModel>(
            [this](const std::string& uuid)
            {
                m_levelUUID = uuid;
                m_levelComplete = true;
            });
        m_model->initialize();

        m_keyExitHandlerId = KeyboardInput::instance().registerKeyReleasedHandler(
            "escape",
            [this]()
            {
                m_nextState = ViewState::Puzzles;
            });
        m_keyStayHandlerId = KeyboardInput::instance().registerKeyReleasedHandler(
            "enter",
            [this]()
            {
                if (m_levelComplete)
                {
                    m_completeState = CompleteState::Stay;
                }
            });
        m_keyNextHandlerId = KeyboardInput::instance().registerKeyReleasedHandler(
            "space",
            [this]()
            {
                if (m_levelComplete)
                {
                    m_completeState = CompleteState::NextLevel;
                }
            });

        m_controllerExitHandlerId = ControllerInput::instance().registerButtonReleasedHandler(
            ControllerInput::Button::Right,
            [this](ControllerInput::Button, const std::chrono::microseconds)
            {
                m_nextState = ViewState::Puzzles;
            });

        m_controllerStayHandlerId = ControllerInput::instance().registerButtonReleasedHandler(
            ControllerInput::Button::Top,
            [this](ControllerInput::Button, const std::chrono::microseconds)
            {
                if (m_levelComplete)
                {
                    m_completeState = CompleteState::Stay;
                }
            });

        m_controllerNextHandlerId = ControllerInput::instance().registerButtonReleasedHandler(
            ControllerInput::Button::Bottom,
            [this](ControllerInput::Button, const std::chrono::microseconds)
            {
                if (m_levelComplete)
                {
                    m_completeState = CompleteState::NextLevel;
                }
            });

        return true;
    }

    void Gameplay::stop()
    {
        m_model->shutdown();
        KeyboardInput::instance().unregisterKeyReleasedHandler(m_keyExitHandlerId);
        KeyboardInput::instance().unregisterKeyReleasedHandler(m_keyStayHandlerId);
        KeyboardInput::instance().unregisterKeyReleasedHandler(m_keyNextHandlerId);

        ControllerInput::instance().unregisterButtonReleasedHandler(m_controllerExitHandlerId);
        ControllerInput::instance().unregisterButtonReleasedHandler(m_controllerStayHandlerId);
        ControllerInput::instance().unregisterButtonReleasedHandler(m_controllerNextHandlerId);
    }

    ViewState Gameplay::update(const std::chrono::microseconds elapsedTime, [[maybe_unused]] const std::chrono::system_clock::time_point now)
    {
        m_model->update(elapsedTime);

        if (m_levelComplete)
        {
            switch (m_completeState)
            {
                case CompleteState::None:
                    // Nothing needs to be done
                    break;
                case CompleteState::Stay:
                {
                    m_model->shutdown();
                    m_model = std::make_unique<GameModel>(
                        [this](const std::string& uuid)
                        {
                            m_levelUUID = uuid;
                            m_levelComplete = true;
                        });
                    m_model->initialize();

                    m_levelComplete = false;
                    m_completeState = CompleteState::None;
                }
                break;
                case CompleteState::NextLevel:
                {
                    auto level = Content::getLevels().getNextLevel(m_levelUUID);
                    if (level != nullptr)
                    {
                        GameModel::selectLevel(level);

                        m_model->shutdown();
                        m_model = std::make_unique<GameModel>(
                            [this](const std::string& uuid)
                            {
                                m_levelUUID = uuid;
                                m_levelComplete = true;
                            });
                        m_model->initialize();

                        m_levelComplete = false;
                        m_completeState = CompleteState::None;
                    }
                }
                break;
            }
        }

        return m_nextState;
    }

    void Gameplay::render(sf::RenderTarget& renderTarget, const std::chrono::microseconds elapsedTime)
    {
        m_model->render(renderTarget, elapsedTime);
    }
} // namespace views
