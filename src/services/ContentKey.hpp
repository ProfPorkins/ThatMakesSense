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

#include <string>

namespace content
{
    using namespace std::string_literals;

    static const auto KEY_FONT_TITLE = "font/title"s;
    static const auto KEY_FONT_HINT = "font/hint"s;
    static const auto KEY_FONT_MENU = "font/menu"s;
    static const auto KEY_FONT_CREDITS = "font/credits"s;
    static const auto KEY_FONT_SETTINGS = "font/settings"s;
    static const auto KEY_FONT_LEVEL_SELECT = "font/level-select";
    static const auto KEY_FONT_CHALLENGES = "font/challenges";
    static const auto KEY_FONT_DEVELOPER_HEX_COORDS = "font/developer";

    static const auto KEY_IMAGE_MENU_BACKGROUND = "image/menu-background"s;
    static const auto KEY_IMAGE_SCORING_CHECKMARK_EMPTY = "image/scoring-checkmark-empty"s;
    static const auto KEY_IMAGE_SCORING_CHECKMARK_FILLED = "image/scoring-checkmark-pre-filled"s;
    static const auto KEY_IMAGE_SCORING_CHECKMARK_BC_FILLED = "image/scoring-checkmark-bc-filled"s;

    static const auto KEY_IMAGE_HELP_KEYBOARD_MOVEMENT = "image/help-keyboard-movement"s;
    static const auto KEY_IMAGE_HELP_KEYBOARD_CAMERA = "image/help-keyboard-camera"s;
    static const auto KEY_IMAGE_HELP_CONTROLLER_MOVEMENT_XBOX = "image/help-controller-movement-xbox"s;
    static const auto KEY_IMAGE_HELP_CONTROLLER_CAMERA_XBOX = "image/help-controller-camera-xbox"s;
    static const auto KEY_IMAGE_HELP_CONTROLLER_MOVEMENT_PS = "image/help-controller-movement-ps"s;
    static const auto KEY_IMAGE_HELP_CONTROLLER_CAMERA_PS = "image/help-controller-camera-ps"s;

    static const auto KEY_MENU_ACTIVATE = "audio/menu-activate"s;
    static const auto KEY_MENU_ACCEPT = "audio/menu-accept"s;

    static const auto KEY_IMAGE_HEX_OUTLINE_256 = "image/hex-outline-256"s;
    static const auto KEY_IMAGE_I_AM_HIGHLIGHT_512 = "image/i-am-highlight-512"s;
    static const auto KEY_IMAGE_GOAL_HIGHLIGHT = "image/goal-highlight"s;
    static const auto KEY_IMAGE_SEND_HIGHLIGHT = "image/send-highlight"s;
    static const auto KEY_IMAGE_PHRASE_DIRECTION_1024 = "image/phrase-direction-1024"s;
    static const auto KEY_IMAGE_PARTICLE_GENERAL = "image/particle-general"s;
    static const auto KEY_IMAGE_PARTICLE_BURN = "image/particle-burn"s;
    static const auto KEY_IMAGE_PARTICLE_SINK = "image/particle-sink"s;
    static const auto KEY_IMAGE_PARTICLE_NEW_PHRASE = "image/particle-new-phrase"s;
    static const auto KEY_IMAGE_PARTICLE_LEVEL_COMPLETE_PRE = "image/particle-level-complete-pre"s;
    static const auto KEY_IMAGE_PARTICLE_LEVEL_COMPLETE_BC = "image/particle-level-complete-bc"s;

    static const auto KEY_IMAGE_ANIMATED_ENTITY_WALL = "image-animated/wall"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_FLOOR = "image-animated/floor"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_GRASS = "image-animated/grass"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_FLOWERS = "image-animated/flowers"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_PURPLE = "image-animated/purple"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_PURPLE = "image-animated/background_purple"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_GREY = "image-animated/grey"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_GREY = "image-animated/background_grey"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_GREEN = "image-animated/green"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_GREEN = "image-animated/background_green"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BLUE = "image-animated/blue"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_BLUE = "image-animated/background_blue"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_RED = "image-animated/red"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_RED = "image-animated/background_red"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BROWN = "image-animated/brown"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_BROWN = "image-animated/background_brown"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_YELLOW = "image-animated/yellow"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_YELLOW = "image-animated/background_yellow"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BACKGROUND_WHITE = "image-animated/background_white"s;
    static const auto KEY_IMAGE_ANIMATED_ENTITY_BLACK = "image-animated/black"s;

    static const auto KEY_TEXT_ANIMATED_I = "text-animated/i"s;
    static const auto KEY_TEXT_ANIMATED_IS = "text-animated/is"s;
    static const auto KEY_TEXT_ANIMATED_AM = "text-animated/am"s;
    static const auto KEY_TEXT_ANIMATED_CAN = "text-animated/can"s;
    static const auto KEY_TEXT_ANIMATED_AND = "text-animated/and"s;

    static const auto KEY_TEXT_ANIMATED_GOAL = "text-animated/goal"s;
    static const auto KEY_TEXT_ANIMATED_CLIMB = "text-animated/climb"s;
    static const auto KEY_TEXT_ANIMATED_FLOAT = "text-animated/float"s;
    static const auto KEY_TEXT_ANIMATED_CHILL = "test-animated/chill"s;
    static const auto KEY_TEXT_ANIMATED_PUSH = "text-animated/push"s;
    static const auto KEY_TEXT_ANIMATED_PULL = "text-animated/pull"s;
    static const auto KEY_TEXT_ANIMATED_STOP = "text-animated/stop"s;
    static const auto KEY_TEXT_ANIMATED_STEEP = "text-animated/steep"s;
    static const auto KEY_TEXT_ANIMATED_WATER = "text-animated/water"s;
    static const auto KEY_TEXT_ANIMATED_HOT = "text-animated/hot"s;
    static const auto KEY_TEXT_ANIMATED_SEND = "text-animated/send"s;

    static const auto KEY_TEXT_ANIMATED_WORD = "text-animated/word"s;
    static const auto KEY_TEXT_ANIMATED_WALL = "text-animated/wall"s;
    static const auto KEY_TEXT_ANIMATED_FLOOR = "text-animated/floor"s;
    static const auto KEY_TEXT_ANIMATED_FLOWERS = "text-animated/flowers"s;
    static const auto KEY_TEXT_ANIMATED_GRASS = "text-animated/grass"s;
    static const auto KEY_TEXT_ANIMATED_PURPLE = "text-animated/purple"s;
    static const auto KEY_TEXT_ANIMATED_GREY = "text-animated/grey"s;
    static const auto KEY_TEXT_ANIMATED_GREEN = "text-animated/green"s;
    static const auto KEY_TEXT_ANIMATED_BLUE = "text-animated/blue"s;
    static const auto KEY_TEXT_ANIMATED_RED = "text-animated/red"s;
    static const auto KEY_TEXT_ANIMATED_BROWN = "text-animated/brown"s;
    static const auto KEY_TEXT_ANIMATED_YELLOW = "text-animated/yellow"s;
    static const auto KEY_TEXT_ANIMATED_BLACK = "text-animated/black"s;
    static const auto KEY_TEXT_ANIMATED_WHITE = "text-animated/white"s;

    static const auto KEY_AUDIO_STEP = "audio/step"s;
    static const auto KEY_AUDIO_SINK = "audio/sink"s;
    static const auto KEY_AUDIO_BURN = "audio/burn"s;
    static const auto KEY_AUDIO_RULE_CHANGED = "audio/rule-changed"s;
    static const auto KEY_AUDIO_LEVEL_COMPLETE = "audio/level-complete"s;
    static const auto KEY_MUSIC_GAMEPLAY_1 = "music/background-1"s;
    static const auto KEY_MUSIC_GAMEPLAY_2 = "music/background-2"s;
    static const auto KEY_MUSIC_GAMEPLAY_3 = "music/background-3"s;
    static const auto KEY_MUSIC_GAMEPLAY_4 = "music/background-4"s;

    static const auto KEY_LEVELS = "levels"s;
} // namespace content
