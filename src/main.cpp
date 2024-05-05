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

#include "Levels.hpp"
#include "misc/misc.hpp"
#include "services/Audio.hpp"
#include "services/Configuration.hpp"
#include "services/ConfigurationPath.hpp"
#include "services/Content.hpp"
#include "services/ContentKey.hpp"
#include "services/ControllerInput.hpp"
#include "services/KeyboardInput.hpp"
#include "services/MouseInput.hpp"
#include "services/Scoring.hpp"
#include "services/ThreadPool.hpp"
#include "views/About.hpp"
#include "views/Credits.hpp"
#include "views/Gameplay.hpp"
#include "views/HelpControls.hpp"
#include "views/HelpDisplay.hpp"
#include "views/MainMenu.hpp"
#include "views/Puzzles.hpp"
#include "views/SettingsControls.hpp"
#include "views/SettingsControlsCamera.hpp"
#include "views/SettingsControlsMovement.hpp"
#include "views/SettingsControlsOther.hpp"
#include "views/SettingsGraphicsAudio.hpp"
#include "views/View.hpp"
#include "views/ViewState.hpp"

#include <SFML/Graphics.hpp>
#include <SFML/Window.hpp>
#include <algorithm>
#include <atomic>
#include <chrono>
#include <fstream>
#include <iostream>
#include <latch>
#include <memory>
#include <sstream>
#include <string>
#include <unordered_map>

// --------------------------------------------------------------
//
// HOW TO ENABLE DEBUG CONSOLE
//
// 1. Select "properties" for the project
// 2. Select the Linker configuration
// 3. Select System
// 4. Set SubSystem to Console / SUBSYSTEM:CONSOLE
//
// Be sure to return it to Windows / SUBSYSTEM : WINDOWS when done
//
// --------------------------------------------------------------

//
// This should not be in the Configuration.hpp header because client/server
// will have different configuration files.
const std::string CONFIG_SETTINGS_FILENAME = "client.settings.json";
const std::string CONFIG_DEVELOPER_FILENAME = "client.developer.json";
const std::string CONFIG_SCORES_FILENAME = "client.scores.json";

// --------------------------------------------------------------
//
// Read the json config file, then hand it off to be parsed and
// made usable by the Configuration service.
//
// --------------------------------------------------------------
auto readConfiguration()
{
    // Reference: https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
    // Using Jerry's answer, because it was benchmarked to be quite fast, even though the config files are small.
    std::ifstream inSettings(CONFIG_SETTINGS_FILENAME);
    std::stringstream bufferSettings;
    bufferSettings << inSettings.rdbuf();
    inSettings.close();

    std::stringstream bufferDeveloper;
    std::ifstream inDeveloper(CONFIG_DEVELOPER_FILENAME);
    if (inDeveloper)
    {
        bufferDeveloper << inDeveloper.rdbuf();
        inDeveloper.close();
    }

    return Configuration::instance().initialize(bufferSettings.str(), bufferDeveloper.str());
}

// --------------------------------------------------------------
//
// Save the current configuration to the config file.
//
// --------------------------------------------------------------
void saveConfiguration()
{
    auto json = Configuration::instance().serialize();
    std::ofstream ofFile(CONFIG_SETTINGS_FILENAME);
    ofFile << json;
    ofFile.close();
}

// --------------------------------------------------------------
//
// Based on configuration settings and what the system is actually
// capable of doing, create the appropriate rendering window.
//
// --------------------------------------------------------------
std::shared_ptr<sf::RenderWindow> prepareWindow()
{
    //
    // Create the window : The settings parameter isn't necessary for what I'm doing, but leaving it here for reference
    sf::ContextSettings settings;

    // Need to track the current setting so that when the window is resized or changed to full-screen
    // we have this info for the code that handles those changes.
    Configuration::getGraphics().setFullScreen(Configuration::get<bool>(config::GRAPHICS_FULL_SCREEN));

    int style = sf::Style::None;
    if (Configuration::get<bool>(config::GRAPHICS_FULL_SCREEN))
    {
        style = sf::Style::Fullscreen;
        //
        // By definition, use whatever resolution the desktop is in
        auto desktop = sf::VideoMode::getDesktopMode();
        Configuration::getGraphics().setResolution({ desktop.width, desktop.height });
        Configuration::getGraphics().setBpp(desktop.bitsPerPixel);
    }
    else
    {
        style = sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
        Configuration::getGraphics().setResolution({ Configuration::get<std::uint16_t>(config::GRAPHICS_WIDTH),
                                                     Configuration::get<std::uint16_t>(config::GRAPHICS_HEIGHT) });
        Configuration::getGraphics().setBpp(Configuration::get<std::uint8_t>(config::GRAPHICS_BPP));
    }
    auto window = std::make_shared<sf::RenderWindow>(
        sf::VideoMode(
            Configuration::getGraphics().getResolution().width,
            Configuration::getGraphics().getResolution().height,
            Configuration::getGraphics().getBpp()),
        misc::GAME_NAME,
        style,
        settings);

    window->setVerticalSyncEnabled(Configuration::get<bool>(config::GRAPHICS_VSYNC));

    return window;
}

void prepareView(std::shared_ptr<sf::RenderWindow> window)
{
    //
    // The aspect ratio is needed in order to know how to organize the viewport
    // for the "game play" area used for the demonstrations.
    auto aspectRatio = static_cast<float>(window->getSize().x) / window->getSize().y;

    //
    // Get the view coordinates to be square, based on the aspect ratio of the window
    if (aspectRatio > 1.0)
    {
        Configuration::getGraphics().setViewCoordinates(
            { Configuration::getGraphics().getViewCoordinates().width * aspectRatio,
              Configuration::getGraphics().getViewCoordinates().height });
    }
    else
    {
        Configuration::getGraphics().setViewCoordinates(
            { Configuration::getGraphics().getViewCoordinates().width,
              Configuration::getGraphics().getViewCoordinates().height * (1.0f / aspectRatio) });
    }

    // Have to set the view after preparing it
    sf::View view({ 0.0f, 0.0f }, Configuration::getGraphics().getViewCoordinates());
    window->setView(view);
}

auto createUIViews()
{
    std::unordered_map<views::ViewState, std::shared_ptr<views::View>> views;

    views[views::ViewState::MainMenu] = std::make_shared<views::MainMenu>();
    views[views::ViewState::GamePlay] = std::make_shared<views::Gameplay>();
    views[views::ViewState::HelpControls] = std::make_shared<views::HelpControls>();
    views[views::ViewState::HelpKeyboardMovement] = std::make_shared<views::HelpDisplay>(views::ViewState::HelpKeyboardMovement, content::KEY_IMAGE_HELP_KEYBOARD_MOVEMENT);
    views[views::ViewState::HelpKeyboardCamera] = std::make_shared<views::HelpDisplay>(views::ViewState::HelpKeyboardCamera, content::KEY_IMAGE_HELP_KEYBOARD_CAMERA);
    views[views::ViewState::HelpControllerMovementXBox] = std::make_shared<views::HelpDisplay>(views::ViewState::HelpControllerMovementXBox, content::KEY_IMAGE_HELP_CONTROLLER_MOVEMENT_XBOX);
    views[views::ViewState::HelpControllerCameraXBox] = std::make_shared<views::HelpDisplay>(views::ViewState::HelpControllerCameraXBox, content::KEY_IMAGE_HELP_CONTROLLER_CAMERA_XBOX);
    views[views::ViewState::HelpControllerMovementPS] = std::make_shared<views::HelpDisplay>(views::ViewState::HelpControllerMovementPS, content::KEY_IMAGE_HELP_CONTROLLER_MOVEMENT_PS);
    views[views::ViewState::HelpControllerCameraPS] = std::make_shared<views::HelpDisplay>(views::ViewState::HelpControllerCameraPS, content::KEY_IMAGE_HELP_CONTROLLER_CAMERA_PS);
    views[views::ViewState::Puzzles] = std::make_shared<views::Puzzles>();
    views[views::ViewState::About] = std::make_shared<views::About>();
    views[views::ViewState::Credits] = std::make_shared<views::Credits>();
    views[views::ViewState::SettingsControls] = std::make_shared<views::SettingsControls>();
    views[views::ViewState::SettingsControlsCamera] = std::make_shared<views::SettingsControlsCamera>();
    views[views::ViewState::SettingsControlsMovement] = std::make_shared<views::SettingsControlsMovement>();
    views[views::ViewState::SettingsControlsOther] = std::make_shared<views::SettingsControlsOther>();
    views[views::ViewState::SettingsGraphicsAudio] = std::make_shared<views::SettingsGraphicsAudio>();

    return views;
}

bool loadMenuContent()
{
    std::atomic_bool success{ true };
    std::latch contentDone{ 1 };

    // IMPORTANT NOTE:  The last bit of content loaded below MUST invoke this on success, and none of the others
    auto onComplete = [&]([[maybe_unused]] std::string key)
    {
        contentDone.count_down();
    };
    auto onError = [&]([[maybe_unused]] std::string filename)
    {
        success = false;
        contentDone.count_down();
    };

    //
    // Get the levels loaded
    Content::load<Levels>(content::KEY_LEVELS, Configuration::get<std::string>(config::GAME_LEVELS), nullptr, onError);

    //
    // Get the fonts loaded
    Content::load<sf::Font>(content::KEY_FONT_TITLE, Configuration::get<std::string>(config::FONT_TITLE_FILENAME), nullptr, onError);
    Content::load<sf::Font>(content::KEY_FONT_HINT, Configuration::get<std::string>(config::FONT_HINT_FILENAME), nullptr, onError);
    Content::load<sf::Font>(content::KEY_FONT_MENU, Configuration::get<std::string>(config::FONT_MENU_FILENAME), nullptr, onError);
    Content::load<sf::Font>(content::KEY_FONT_CREDITS, Configuration::get<std::string>(config::FONT_CREDITS_FILENAME), nullptr, onError);
    Content::load<sf::Font>(content::KEY_FONT_SETTINGS, Configuration::get<std::string>(config::FONT_SETTINGS_FILENAME), nullptr, onError);
    Content::load<sf::Font>(content::KEY_FONT_LEVEL_SELECT, Configuration::get<std::string>(config::FONT_LEVEL_SELECT_FILENAME), nullptr, onError);
    Content::load<sf::Font>(content::KEY_FONT_CHALLENGES, Configuration::get<std::string>(config::FONT_CHALLENGES_FILENAME), nullptr, onError);

    //
    // Get the menu audio activate and accept clips loaded
    Content::load<sf::SoundBuffer>(content::KEY_MENU_ACTIVATE, Configuration::get<std::string>(config::AUDIO_MENU_ACTIVATE), nullptr, onError);
    Content::load<sf::SoundBuffer>(content::KEY_MENU_ACCEPT, Configuration::get<std::string>(config::AUDIO_MENU_ACCEPT), nullptr, onError);

    //
    // Get the help images loaded
    Content::load<sf::Texture>(content::KEY_IMAGE_HELP_KEYBOARD_MOVEMENT, Configuration::get<std::string>(config::IMAGE_HELP_KEYBOARD_MOVEMENT), nullptr, onError);
    Content::load<sf::Texture>(content::KEY_IMAGE_HELP_KEYBOARD_CAMERA, Configuration::get<std::string>(config::IMAGE_HELP_KEYBOARD_CAMERA), nullptr, onError);
    Content::load<sf::Texture>(content::KEY_IMAGE_HELP_CONTROLLER_MOVEMENT_XBOX, Configuration::get<std::string>(config::IMAGE_HELP_CONTROLLER_MOVEMENT_XBOX), nullptr, onError);
    Content::load<sf::Texture>(content::KEY_IMAGE_HELP_CONTROLLER_CAMERA_XBOX, Configuration::get<std::string>(config::IMAGE_HELP_CONTROLLER_CAMERA_XBOX), nullptr, onError);
    Content::load<sf::Texture>(content::KEY_IMAGE_HELP_CONTROLLER_MOVEMENT_PS, Configuration::get<std::string>(config::IMAGE_HELP_CONTROLLER_MOVEMENT_PS), nullptr, onError);
    Content::load<sf::Texture>(content::KEY_IMAGE_HELP_CONTROLLER_CAMERA_PS, Configuration::get<std::string>(config::IMAGE_HELP_CONTROLLER_CAMERA_PS), nullptr, onError);

    //
    // Get the challenge checkmark textures loaded
    Content::load<sf::Texture>(content::KEY_IMAGE_SCORING_CHECKMARK_EMPTY, Configuration::get<std::string>(config::IMAGE_SCORING_CHECKMARK_EMPTY), nullptr, onError);
    Content::load<sf::Texture>(content::KEY_IMAGE_SCORING_CHECKMARK_FILLED, Configuration::get<std::string>(config::IMAGE_SCORING_CHECKMARK_FILLED), nullptr, onError);
    // NOTE: The last item in the function must include the call to onComplete!!
    //       Why don't I have each one increment a count and have all call onComplete?  Because it is possible, but extremely unlikely, that all loading tasks
    //       complete before the remainder of the content loading tasks are added.
    Content::load<sf::Texture>(content::KEY_IMAGE_SCORING_CHECKMARK_BC_FILLED, Configuration::get<std::string>(config::IMAGE_SCORING_CHECKMARK_BC_FILLED), onComplete, onError);

    //
    // Use an efficient wait for the initial content to finish loading
    contentDone.wait();

    return success;
}

// ------------------------------------------------------------------
//
// All the game content loading is kicked off here.  The loading isn't
// completed here, just started loading.  Looading of assests happens
// asynchronously.  The game model when starting up, waits until all
// content is loaded, so no worry a level will start running before the
// content has completed loading.
//
// ------------------------------------------------------------------
void startLoadingGameContent()
{
    //
    // Textures
    std::vector<std::pair<std::string, config::config_path>> textures{
        { content::KEY_IMAGE_HEX_OUTLINE_256, config::IMAGE_HEX_OUTLINE_256 },
        { content::KEY_IMAGE_I_AM_HIGHLIGHT_512, config::IMAGE_I_AM_HIGHLIGHT_512 },
        { content::KEY_IMAGE_GOAL_HIGHLIGHT, config::IMAGE_GOAL_HIGHLIGHT },
        { content::KEY_IMAGE_SEND_HIGHLIGHT, config::IMAGE_SEND_HIGHLIGHT },
        { content::KEY_IMAGE_PHRASE_DIRECTION_1024, config::IMAGE_PHRASE_DIRECTION_1024 },
        { content::KEY_IMAGE_PARTICLE_GENERAL, config::IMAGE_PARTICLE_GENERAL },
        { content::KEY_IMAGE_PARTICLE_BURN, config::IMAGE_PARTICLE_BURN },
        { content::KEY_IMAGE_PARTICLE_SINK, config::IMAGE_PARTICLE_SINK },
        { content::KEY_IMAGE_PARTICLE_NEW_PHRASE, config::IMAGE_PARTICLE_NEW_PHRASE },
        { content::KEY_IMAGE_PARTICLE_LEVEL_COMPLETE_PRE, config::IMAGE_PARTICLE_LEVEL_COMPLETE_PRE },
        { content::KEY_IMAGE_PARTICLE_LEVEL_COMPLETE_BC, config::IMAGE_PARTICLE_LEVEL_COMPLETE_BC },

        { content::KEY_IMAGE_ANIMATED_ENTITY_WALL, config::IMAGE_ENTITY_ANIMATED_WALL },
        { content::KEY_IMAGE_ANIMATED_ENTITY_FLOOR, config::IMAGE_ENTITY_ANIMATED_FLOOR },
        { content::KEY_IMAGE_ANIMATED_ENTITY_GRASS, config::IMAGE_ENTITY_ANIMATED_GRASS },
        { content::KEY_IMAGE_ANIMATED_ENTITY_FLOWERS, config::IMAGE_ENTITY_ANIMATED_FLOWERS },
        { content::KEY_IMAGE_ANIMATED_ENTITY_PURPLE, config::IMAGE_ENTITY_ANIMATED_PURPLE },
        { content::KEY_IMAGE_ANIMATED_ENTITY_GREY, config::IMAGE_ENTITY_ANIMATED_GREY },
        { content::KEY_IMAGE_ANIMATED_ENTITY_GREEN, config::IMAGE_ENTITY_ANIMATED_GREEN },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BLUE, config::IMAGE_ENTITY_ANIMATED_BLUE },
        { content::KEY_IMAGE_ANIMATED_ENTITY_RED, config::IMAGE_ENTITY_ANIMATED_RED },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BROWN, config::IMAGE_ENTITY_ANIMATED_BROWN },
        { content::KEY_IMAGE_ANIMATED_ENTITY_YELLOW, config::IMAGE_ENTITY_ANIMATED_YELLOW },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BLACK, config::IMAGE_ENTITY_ANIMATED_BLACK },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_WHITE, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_WHITE },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_PURPLE, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_PURPLE },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_GREY, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_GREY },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_GREEN, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_GREEN },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_BLUE, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_BLUE },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_RED, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_RED },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_BROWN, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_BROWN },
        { content::KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_YELLOW, config::IMAGE_ENTITY_ANIMATED_BACKGROUND_YELLOW },

        { content::KEY_TEXT_ANIMATED_IS, config::IMAGE_ENTITY_TEXT_IS },
        { content::KEY_TEXT_ANIMATED_AM, config::IMAGE_ENTITY_TEXT_AM },
        { content::KEY_TEXT_ANIMATED_CAN, config::IMAGE_ENTITY_TEXT_CAN },
        { content::KEY_TEXT_ANIMATED_AND, config::IMAGE_ENTITY_TEXT_AND },

        { content::KEY_TEXT_ANIMATED_GOAL, config::IMAGE_ENTITY_TEXT_GOAL },
        { content::KEY_TEXT_ANIMATED_CLIMB, config::IMAGE_ENTITY_TEXT_CLIMB },
        { content::KEY_TEXT_ANIMATED_FLOAT, config::IMAGE_ENTITY_TEXT_FLOAT },
        { content::KEY_TEXT_ANIMATED_CHILL, config::IMAGE_ENTITY_TEXT_CHILL },
        { content::KEY_TEXT_ANIMATED_PUSH, config::IMAGE_ENTITY_TEXT_PUSH },
        { content::KEY_TEXT_ANIMATED_PULL, config::IMAGE_ENTITY_TEXT_PULL },
        { content::KEY_TEXT_ANIMATED_STOP, config::IMAGE_ENTITY_TEXT_STOP },
        { content::KEY_TEXT_ANIMATED_STEEP, config::IMAGE_ENTITY_TEXT_STEEP },
        { content::KEY_TEXT_ANIMATED_WATER, config::IMAGE_ENTITY_TEXT_WATER },
        { content::KEY_TEXT_ANIMATED_HOT, config::IMAGE_ENTITY_TEXT_HOT },
        { content::KEY_TEXT_ANIMATED_SEND, config::IMAGE_ENTITY_TEXT_SEND },

        { content::KEY_TEXT_ANIMATED_I, config::IMAGE_ENTITY_TEXT_I },
        { content::KEY_TEXT_ANIMATED_WORD, config::IMAGE_ENTITY_TEXT_WORD },
        { content::KEY_TEXT_ANIMATED_WALL, config::IMAGE_ENTITY_TEXT_WALL },
        { content::KEY_TEXT_ANIMATED_FLOOR, config::IMAGE_ENTITY_TEXT_FLOOR },
        { content::KEY_TEXT_ANIMATED_FLOWERS, config::IMAGE_ENTITY_TEXT_FLOWERS },
        { content::KEY_TEXT_ANIMATED_GRASS, config::IMAGE_ENTITY_TEXT_GRASS },
        { content::KEY_TEXT_ANIMATED_PURPLE, config::IMAGE_ENTITY_TEXT_PURPLE },
        { content::KEY_TEXT_ANIMATED_GREY, config::IMAGE_ENTITY_TEXT_GREY },
        { content::KEY_TEXT_ANIMATED_GREEN, config::IMAGE_ENTITY_TEXT_GREEN },
        { content::KEY_TEXT_ANIMATED_BLUE, config::IMAGE_ENTITY_TEXT_BLUE },
        { content::KEY_TEXT_ANIMATED_RED, config::IMAGE_ENTITY_TEXT_RED },
        { content::KEY_TEXT_ANIMATED_BROWN, config::IMAGE_ENTITY_TEXT_BROWN },
        { content::KEY_TEXT_ANIMATED_YELLOW, config::IMAGE_ENTITY_TEXT_YELLOW },
        { content::KEY_TEXT_ANIMATED_BLACK, config::IMAGE_ENTITY_TEXT_BLACK }
    };

    for (auto&& [keyContent, keyConfig] : textures)
    {
        if (!Content::has<sf::Texture>(keyContent))
        {
            Content::load<sf::Texture>(keyContent, Configuration::get<std::string>(keyConfig), nullptr, nullptr);
        }
    }

    //
    // Fonts
    std::vector<std::pair<std::string, config::config_path>> fonts{
        { content::KEY_FONT_DEVELOPER_HEX_COORDS, config::DEVELOPER_HEX_COORDS_FONT_FILENAME }
    };

    for (auto&& [keyContent, keyConfig] : fonts)
    {
        if (!Content::has<sf::Font>(keyContent))
        {
            Content::load<sf::Font>(keyContent, Configuration::get<std::string>(keyConfig), nullptr, nullptr);
        }
    }

    //
    // Sounds
    std::vector<std::pair<std::string, config::config_path>> sounds{
        { content::KEY_AUDIO_STEP, config::AUDIO_GAMEPLAY_STEP },
        { content::KEY_AUDIO_SINK, config::AUDIO_GAMEPLAY_SINK },
        { content::KEY_AUDIO_BURN, config::AUDIO_GAMEPLAY_BURN },
        { content::KEY_AUDIO_RULE_CHANGED, config::AUDIO_GAMEPLAY_RULE_CHANGED },
        { content::KEY_AUDIO_LEVEL_COMPLETE, config::AUDIO_GAMEPLAY_LEVEL_COMPLETE }
    };

    for (auto&& [keyContent, keyConfig] : sounds)
    {
        if (!Content::has<sf::SoundBuffer>(keyContent))
        {
            Content::load<sf::SoundBuffer>(keyContent, Configuration::get<std::string>(keyConfig), nullptr, nullptr);
        }
    }

    auto addSongToQueue = [](std::string key)
    {
        Audio::addMusic(key);
    };

    //
    // Background Music
    std::vector<std::pair<std::string, config::config_path>> music{
        { content::KEY_MUSIC_GAMEPLAY_1, config::MUSIC_BACKGROUND_1 },
        { content::KEY_MUSIC_GAMEPLAY_2, config::MUSIC_BACKGROUND_2 },
        { content::KEY_MUSIC_GAMEPLAY_3, config::MUSIC_BACKGROUND_3 },
        { content::KEY_MUSIC_GAMEPLAY_4, config::MUSIC_BACKGROUND_4 }
    };
    for (auto&& [keyContent, keyConfig] : music)
    {
        if (!Content::has<sf::Music>(keyContent))
        {
            Content::load<sf::Music>(
                keyContent, Configuration::get<std::string>(keyConfig),
                [addSongToQueue](std::string key)
                {
                    addSongToQueue(key);
                },
                nullptr);
        }
    }
}

// --------------------------------------------------------------
//
// This is my hack to reset the application window whenever the user
// changes the resolution or full/window options.
//
// The way I am having to completely reset the configuration completely is pretty bad,
// but I have struggled with SFML and getting the UI to render correctly based on
// different resolutions.  Completely resetting the configuration was the path of least
// resistance to accomplish the task at hand.  In a future program, I'll look into redoing
// how I use the SFML to get things rendered and hopefully also fix this at that time...or
// more likely, not use the SFML, just build from the ground up myself.
//
// --------------------------------------------------------------
void resizeWindow(sf::Vector2u size, std::shared_ptr<sf::RenderWindow>& window, std::unordered_map<views::ViewState, std::shared_ptr<views::View>>& views, std::shared_ptr<views::View>& view)
{
    // Enforce a min size and aspect ratio
    size = { std::max(640u, size.x), std::max(480u, size.y) };
    auto aspectRatio = static_cast<float>(size.x) / size.y;
    if (aspectRatio < 1)
    {
        size.y = size.x;
    }

    // As the window is resized, we want to remember this for the next time
    // the program is restarted.
    Configuration::set<std::uint16_t>(config::GRAPHICS_WIDTH, static_cast<std::uint16_t>(size.x));
    Configuration::set<std::uint16_t>(config::GRAPHICS_HEIGHT, static_cast<std::uint16_t>(size.y));

    // Have to grab this before resetting the configuration, so we know what state we are
    // currently in
    auto currentlyFullScreen = Configuration::getGraphics().isFullScreen();

    Configuration::getGraphics().setRestart(false);
    saveConfiguration();
    Configuration::instance().reset();
    readConfiguration();

    // Have to stop the view because it is using graphics assets scaled from the previous settings
    view->stop();

    Configuration::getGraphics().setResolution({ size.x, size.y });
    if (currentlyFullScreen && !Configuration::get<bool>(config::GRAPHICS_FULL_SCREEN))
    {
        int style = sf::Style::Titlebar | sf::Style::Resize | sf::Style::Close;
        window->close();
        window->create(sf::VideoMode(
                           Configuration::getGraphics().getResolution().width,
                           Configuration::getGraphics().getResolution().height,
                           Configuration::getGraphics().getBpp()),
                       misc::GAME_NAME,
                       style);
        Configuration::getGraphics().setFullScreen(false);
    }
    else if (Configuration::get<bool>(config::GRAPHICS_FULL_SCREEN))
    {
        int style = sf::Style::Fullscreen;
        window->close();
        window->create(sf::VideoMode(
                           Configuration::getGraphics().getResolution().width,
                           Configuration::getGraphics().getResolution().height,
                           Configuration::getGraphics().getBpp()),
                       misc::GAME_NAME,
                       style);
        Configuration::getGraphics().setFullScreen(true);
    }
    else
    {
        window->setSize({ Configuration::getGraphics().getResolution().width, Configuration::getGraphics().getResolution().height });
    }

    //
    // Create and activate the window for rendering on the main thread
    prepareView(window);

    //
    // Construct the different views the game may show, because they need to update themselves based on
    // the new graphics settings.
    views = createUIViews();
}

void handleWindowEvents(bool& running, const std::chrono::system_clock::time_point& currentTime, const std::chrono::microseconds& elapsedTime, const views::ViewState& viewState, std::shared_ptr<sf::RenderWindow> window, std::shared_ptr<views::View>& view, std::unordered_map<views::ViewState, std::shared_ptr<views::View>>& views)
{
    static bool anyResize{ false };
    static std::chrono::microseconds timeSinceResize{ 0 };
    static sf::Event lastResizeEvent{};

    timeSinceResize += elapsedTime;

    // Handle all pending Windows events
    sf::Event event;
    while (window->pollEvent(event))
    {
        if (event.type == sf::Event::Closed)
        {
            // end the program
            running = false;
        }

        //
        // This is essentially the process input stage of the game loop
        switch (event.type)
        {
            case sf::Event::KeyPressed:
            {
                KeyboardInput::instance().signalKeyPressed(event.key, elapsedTime, currentTime);
            }
            break;
            case sf::Event::KeyReleased:
            {
                KeyboardInput::instance().signalKeyReleased(event.key, elapsedTime, currentTime);
            }
            break;
            case sf::Event::MouseMoved:
            {
                auto viewCoords = window->mapPixelToCoords({ event.mouseMove.x, event.mouseMove.y }, window->getView());
                MouseInput::instance().signalMouseMoved({ viewCoords.x, viewCoords.y });
            }
            break;
            case sf::Event::MouseButtonPressed:
            {
                auto viewCoords = window->mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, window->getView());
                MouseInput::instance().signalMousePressed(event.mouseButton.button, { viewCoords.x, viewCoords.y });
            }
            break;
            case sf::Event::MouseButtonReleased:
            {
                auto viewCoords = window->mapPixelToCoords({ event.mouseButton.x, event.mouseButton.y }, window->getView());
                MouseInput::instance().signalMouseReleased(event.mouseButton.button, { viewCoords.x, viewCoords.y });
            }
            break;
            case sf::Event::MouseWheelScrolled:
            {
                // Converting the screen coordinates to view coords in order for all other code to work correctly.
                auto viewCoords = window->mapPixelToCoords({ event.mouseWheelScroll.x, event.mouseWheelScroll.y }, window->getView());
                event.mouseWheelScroll.x = static_cast<int>(viewCoords.x);
                event.mouseWheelScroll.y = static_cast<int>(viewCoords.y);
                MouseInput::instance().signalMouseWheelScroll(event.mouseWheelScroll);
            }
            break;
            case sf::Event::JoystickButtonPressed:
            {
                ControllerInput::instance().signalButtonPressed(event.joystickButton);
            }
            break;
            case sf::Event::JoystickButtonReleased:
            {
                ControllerInput::instance().signalButtonReleased(event.joystickButton);
            }
            break;
            case sf::Event::JoystickMoved:
            {
                ControllerInput::instance().signalJoystickMoved(event.joystickMove);
            }
            break;
            case sf::Event::Resized:
            {
                anyResize = true;
                lastResizeEvent = event;
                timeSinceResize = std::chrono::microseconds::zero();
            }
            break;
            case sf::Event::JoystickConnected:
            case sf::Event::JoystickDisconnected:
            {
                ControllerInput::instance().enumerateControllers();
            }
            break;
            default:
                // This is here to eliminate a bunch of compiler warnings related to events not handled in this switch statement
                break;
        }
    }

    // All of this stuff is to handle the way the Linux window manager issues resize events.
    // Windows and macOS (seem to) send one resize event through SFML and so only one resize
    // needs to be handled.  However, Linux will send resize events all while the user is dragging
    // the window around, which seems reasonable, it just makes adjusting the game internals
    // a bit more tricky.  Crossing all my fingers this continues to be the fix that allows it
    // to work for Linux users.
    if (anyResize && timeSinceResize > misc::msTous(std::chrono::milliseconds(500)))
    {
        if (viewState == views::ViewState::SettingsGraphicsAudio)
        {
            resizeWindow({ lastResizeEvent.size.width, lastResizeEvent.size.height }, window, views, view);
            view = views[viewState];
            view->start();
        }
        else
        {
            window->setSize({ Configuration::getGraphics().getResolution().width, Configuration::getGraphics().getResolution().height });
        }

        anyResize = false;
        timeSinceResize = std::chrono::microseconds::zero();
    }
}

int main()
{
    //
    // Read the configuration file so we can get things setup based on that
    if (!readConfiguration())
    {
        std::cout << "Failure in reading configuration file...\n";
        exit(0);
    }

    //
    // The Audio singleton needs to be specifically initialized
    Audio::instance().initialize();

    if (!loadMenuContent())
    {
        exit(0);
    }

    // This must be initialized after the menu content is loaded, so that all the levels
    // are known, because scoring initialization relies on that.
    if (!Scoring::instance().initialize(CONFIG_SCORES_FILENAME))
    {
        std::cout << "Failure in reading the scoring file...\n";
    }

    //
    // With the menu content loaded, let's go ahead and kickoff loading  all
    // the game assets so it is likely completed by the time the user starts playing.
    startLoadingGameContent();

    //
    // Create and activate the window for rendering on the main thread
    auto window = prepareWindow();
    prepareView(window);
    window->setActive(true);

    //
    // The various input singletons need to be specifically initialized
    // Note: These should be initialized after the window is prepared because
    //       the controller info isn't ready until after then.
    KeyboardInput::instance().initialize();
    MouseInput::instance().initialize(window);
    ControllerInput::instance().initialize(window);

    //
    // Construct the different views the game may show, then get
    // the initial view set and ready to run.
    std::unordered_map<views::ViewState, std::shared_ptr<views::View>> views = createUIViews();

    views::ViewState viewState = Configuration::get<bool>(config::DEVELOPER_MAIN_MENU) ? views::ViewState::MainMenu : views::ViewState::GamePlay;
    if (viewState == views::ViewState::GamePlay)
    {
        GameModel::selectLevel(Content::getLevels().get(0));
    }
    else
    {
        // Show the about view before the main menu until the user has completed at least one challenge
        if (!Scoring::instance().hasAnyScore())
        {
            viewState = views::ViewState::About;
        }
    }
    std::shared_ptr<views::View> view = views[viewState];
    if (!view->start())
    {
        std::cout << "Initial view failed to initialize, terminating..." << std::endl;
        exit(0);
    }

    //
    // Grab an initial time-stamp to get the elapsed time working
    auto previousTime = std::chrono::system_clock::now();

    //
    // Get the Window loop running.  The game loop runs inside of this loop
    bool running = true;
    while (running)
    {
        //
        // Figure out the elapsed time in microseconds.  Need this to pass on to
        // the game model.
        auto currentTime = std::chrono::system_clock::now();
        auto elapsedTime = std::chrono::duration_cast<std::chrono::microseconds>(currentTime - previousTime);
        previousTime = currentTime;

        // Let's handle all the SFML window events that we are interested in first, after that
        // go into the standard game loop processing.
        handleWindowEvents(running, currentTime, elapsedTime, viewState, window, view, views);

        //
        // Execute the standard game loop steps

        // Step 1: Process Input
        KeyboardInput::instance().update(elapsedTime);
        MouseInput::instance().update(elapsedTime);
        ControllerInput::instance().update(elapsedTime);

        // Step 2: Update
        auto nextViewState = view->update(elapsedTime, currentTime);

        // Step 3: Render
        view->render(*window, elapsedTime);

        //
        // BUT, we still wait until here to display the window...this is what actually
        // causes the rendering to occur.
        window->display();

        //
        // Constantly check to see if the view should change.
        if (nextViewState != viewState)
        {
            if (nextViewState == views::ViewState::Exit)
            {
                running = false;
            }
            else
            {
                view->stop();
                view = views[nextViewState];
                view->start();
                viewState = nextViewState;
            }
        }
        //
        // Constantly check to see if the window should be restarted, due to a graphics option having changed.
        if (Configuration::getGraphics().restart())
        {
            resizeWindow(
                { Configuration::get<std::uint16_t>(config::GRAPHICS_WIDTH),
                  Configuration::get<std::uint16_t>(config::GRAPHICS_HEIGHT) },
                window, views, view);

            view = views[viewState];
            if (!view->start())
            {
                std::cout << "Failed to restart in the newly selection screen/resolution settings, terminating..." << std::endl;
                exit(0);
            }
        }
    }

    // Gracefully shut things down
    saveConfiguration();
    Audio::instance().terminate();
    ThreadPool::terminate();
    Content::instance().terminate();

    // Do this after shutting down the Content singleton so that all textures
    // are released by the various shared pointers.  This cleans up some errors
    // that otherwise show up as SFML is shutting down.
    window->setActive(false);
    window->close();

    return 0;
}
