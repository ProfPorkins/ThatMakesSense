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
#include <vector>

namespace config
{
    using namespace std::string_literals;
    using config_path = std::vector<std::string>;
    // --------------------------------------------------------------
    //
    // Commonly used settings names
    //
    // --------------------------------------------------------------
    static const auto DOM_FILENAME = "filename"s;
    static const auto DOM_FILENAME_ANIMATED = "filename-animated"s;
    static const auto DOM_FILENAME_STATIC = "filename-static"s;
    static const auto DOM_AUDIO = "audio"s;
    static const auto DOM_IMAGE = "image"s;
    static const auto DOM_FONT = "font"s;
    static const auto DOM_SIZE = "size"s;
    static const auto DOM_SETTINGS = "settings"s;
    static const auto DOM_LEVEL_SELECT = "level-select"s;
    static const auto DOM_CHALLENGES = "challenges"s;
    static const auto DOM_SPRITE_COUNT = "sprite-count"s;
    static const auto DOM_SPRITE_TIME = "sprite-time"s;
    static const auto DOM_SPRITE_COLOR = "sprite-color"s;

    // --------------------------------------------------------------
    //
    // Developer configuration names
    //
    // --------------------------------------------------------------
    static const auto DOM_DEVELOPER = "developer"s;
    static const auto DOM_HEX_COORDS = "hex-coords";
    static const config_path DEVELOPER_MAIN_MENU = { DOM_DEVELOPER, "main-menu"s };                      // true if main menu do be displayed, otherwise directly join game
    static const config_path DEVELOPER_VALIDATE_LEVEL_HASH = { DOM_DEVELOPER, "validate-level-hash"s };  // true if level has should be validated
    static const config_path DEVELOPER_HEX_COORDS_RENDER = { DOM_DEVELOPER, DOM_HEX_COORDS, "render"s }; // true if to render the hex coords
    static const config_path DEVELOPER_HEX_COORDS_FONT_FILENAME = { DOM_DEVELOPER, DOM_HEX_COORDS, DOM_FONT, DOM_FILENAME };
    static const config_path DEVELOPER_HEX_COORDS_FONT_SIZE = { DOM_DEVELOPER, DOM_HEX_COORDS, DOM_FONT, DOM_SIZE };

    // --------------------------------------------------------------
    //
    // Developer configuration names
    //
    // --------------------------------------------------------------
    static const auto DOM_CAMERA = "camera"s;
    static const config_path CAMERA_RANGE_MIN = { DOM_CAMERA, "min-range"s };
    static const config_path CAMERA_RANGE_MAX = { DOM_CAMERA, "max-range"s };

    // --------------------------------------------------------------
    //
    // Graphics configuration names
    //
    // --------------------------------------------------------------
    static const auto DOM_GRAPHICS = "graphics"s;
    static const auto DOM_GRAPHICS_RESOLUTION = "resolution"s;
    static const config_path GRAPHICS_WIDTH = { DOM_GRAPHICS, DOM_GRAPHICS_RESOLUTION, "width"s };
    static const config_path GRAPHICS_HEIGHT = { DOM_GRAPHICS, DOM_GRAPHICS_RESOLUTION, "height"s };
    static const config_path GRAPHICS_BPP = { DOM_GRAPHICS, "bpp"s };                 // bits per pixel
    static const config_path GRAPHICS_FULL_SCREEN = { DOM_GRAPHICS, "full-screen"s }; // true if it should run full screen
    static const config_path GRAPHICS_VSYNC = { DOM_GRAPHICS, "vsync"s };             // true if v-sync should be on

    // --------------------------------------------------------------
    //
    // Keyboard/mouse/controller control configuration names
    //
    // --------------------------------------------------------------
    static const auto DOM_KEYBOARD = "keyboard"s;
    static const auto DOM_MOUSE = "mouse"s;
    static const auto DOM_CONTROLLER = "controller"s;

    static const config_path KEYBOARD_REPEAT_DELAY = { DOM_KEYBOARD, "repeat-delay"s };

    static const config_path KEYBOARD_UP_LEFT = { DOM_KEYBOARD, "up-left"s };
    static const config_path KEYBOARD_UP_RIGHT = { DOM_KEYBOARD, "up-right"s };
    static const config_path KEYBOARD_DOWN_LEFT = { DOM_KEYBOARD, "down-left"s };
    static const config_path KEYBOARD_DOWN_RIGHT = { DOM_KEYBOARD, "down-right"s };
    static const config_path KEYBOARD_LEFT = { DOM_KEYBOARD, "left"s };
    static const config_path KEYBOARD_RIGHT = { DOM_KEYBOARD, "right"s };

    static const config_path KEYBOARD_UNDO = { DOM_KEYBOARD, "undo"s };
    static const config_path KEYBOARD_RESET = { DOM_KEYBOARD, "reset"s };
    static const config_path KEYBOARD_TOGGLE_PHRASE_RENDERING = { DOM_KEYBOARD, "toggle-phrase-rendering"s };

    static const config_path KEYBOARD_CAMERA_LEFT = { DOM_KEYBOARD, "pan-left"s };
    static const config_path KEYBOARD_CAMERA_RIGHT = { DOM_KEYBOARD, "pan-right"s };
    static const config_path KEYBOARD_CAMERA_UP = { DOM_KEYBOARD, "pan-up"s };
    static const config_path KEYBOARD_CAMERA_DOWN = { DOM_KEYBOARD, "pan-down"s };
    static const config_path KEYBOARD_CAMERA_ZOOM_IN = { DOM_KEYBOARD, "zoom-in"s };
    static const config_path KEYBOARD_CAMERA_ZOOM_OUT = { DOM_KEYBOARD, "zoom-out"s };

    static const config_path MOUSE_CAMERA_INVERT_PAN_HORZ = { DOM_MOUSE, "invert-pan-horz"s };
    static const config_path MOUSE_CAMERA_INVERT_PAN_VERT = { DOM_MOUSE, "invert-pan-vert"s };
    static const config_path MOUSE_CAMERA_INVERT_ZOOM = { DOM_MOUSE, "invert-zoom"s };

    static const config_path CONTROLLER_REPEAT_DELAY = { DOM_CONTROLLER, "repeat-delay"s };

    // --------------------------------------------------------------
    //
    // Music configuration names
    //
    // --------------------------------------------------------------
    static const auto DOM_MUSIC = "music"s;
    static const config_path PLAY_BACKGROUND_MUSIC = { DOM_MUSIC, "play-background"s };

    // --------------------------------------------------------------
    //
    // Other configuration names
    //
    // --------------------------------------------------------------
    static const auto DOM_OTHER = "other"s;
    static const config_path OPTIONS_SHOW_HINTS = { DOM_OTHER, "show-hints"s };

    // --------------------------------------------------------------
    //
    // Content configuration names
    //
    // --------------------------------------------------------------
    static const auto DOM_CONTENT = "content"s;
    static const auto DOM_TITLE = "title"s;
    static const auto DOM_HINT = "hint"s;
    static const auto DOM_MENU = "menu"s;
    static const auto DOM_CREDITS = "credits"s;
    static const auto DOM_GAME = "game"s;
    static const auto DOM_HEADING_SIZE = "heading-size"s;
    static const auto DOM_TITLE_SIZE = "title-size"s;
    static const auto DOM_ITEM_SIZE = "item-size"s;
    static const auto DOM_VALUE_SIZE = "value-size"s;
    static const auto DOM_HEADER_SIZE = "header-size"s;
    static const auto DOM_COLOR_FILL = "color-fill"s;
    static const auto DOM_COLOR_OUTLINE = "color-outline"s;
    static const auto DOM_COLOR_ITEM_FILL = "color-item-fill"s;
    static const auto DOM_COLOR_ITEM_OUTLINE = "color-item-outline"s;
    static const auto DOM_COLOR_ACTIVE_FILL = "color-active-fill"s;
    static const auto DOM_COLOR_ACTIVE_OUTLINE = "color-active-outline"s;
    static const auto DOM_COLOR_SELECTION_FILL = "color-selection-fill"s;
    static const auto DOM_COLOR_SELECTION_OUTLINE = "color-selection-outline"s;
    static const auto DOM_COLOR_TITLE_FILL = "color-title-fill"s;
    static const auto DOM_COLOR_TITLE_OUTLINE = "color-title-outline"s;
    static const auto DOM_COLOR_HEADER_FILL = "color-header-fill"s;
    static const auto DOM_COLOR_HEADER_OUTLINE = "color-header-outline"s;
    static const auto DOM_COLOR_ITEM_ACTIVE_FILL = "color-item-active-fill"s;
    static const auto DOM_COLOR_ITEM_ACTIVE_OUTLINE = "color-item-active-outline"s;
    static const auto DOM_LEVELS = "levels"s;
    static const auto DOM_IMAGES = "images"s;
    static const auto DOM_IMAGES_STATIC = "images-static"s;
    static const auto DOM_IMAGES_ANIMATED = "images-animated"s;
    static const auto DOM_IMAGES_OBJECTS = "images-objects"s;

    static const config_path FONT_TITLE_FILENAME = { DOM_CONTENT, DOM_FONT, DOM_TITLE, DOM_FILENAME };
    static const config_path FONT_TITLE_SIZE = { DOM_CONTENT, DOM_FONT, DOM_TITLE, DOM_SIZE };
    static const config_path FONT_TITLE_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_TITLE, DOM_COLOR_FILL };
    static const config_path FONT_TITLE_COLOR_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_TITLE, DOM_COLOR_OUTLINE };

    static const config_path FONT_HINT_FILENAME = { DOM_CONTENT, DOM_FONT, DOM_HINT, DOM_FILENAME };
    static const config_path FONT_HINT_SIZE = { DOM_CONTENT, DOM_FONT, DOM_HINT, DOM_SIZE };
    static const config_path FONT_HINT_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_HINT, DOM_COLOR_FILL };
    static const config_path FONT_HINT_COLOR_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_HINT, DOM_COLOR_OUTLINE };

    static const config_path FONT_MENU_FILENAME = { DOM_CONTENT, DOM_FONT, DOM_MENU, DOM_FILENAME };
    static const config_path FONT_MENU_SIZE = { DOM_CONTENT, DOM_FONT, DOM_MENU, DOM_SIZE };
    static const config_path FONT_MENU_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_MENU, DOM_COLOR_FILL };
    static const config_path FONT_MENU_COLOR_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_MENU, DOM_COLOR_OUTLINE };
    static const config_path FONT_MENU_COLOR_ACTIVE_FILL{ DOM_CONTENT, DOM_FONT, DOM_MENU, DOM_COLOR_ACTIVE_FILL };
    static const config_path FONT_MENU_COLOR_ACTIVE_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_MENU, DOM_COLOR_ACTIVE_OUTLINE };

    static const config_path FONT_CREDITS_FILENAME = { DOM_CONTENT, DOM_FONT, DOM_CREDITS, DOM_FILENAME };
    static const config_path FONT_CREDITS_TITLE_SIZE = { DOM_CONTENT, DOM_FONT, DOM_CREDITS, DOM_TITLE_SIZE };
    static const config_path FONT_CREDITS_ITEM_SIZE = { DOM_CONTENT, DOM_FONT, DOM_CREDITS, DOM_ITEM_SIZE };
    static const config_path FONT_CREDITS_COLOR_ITEM_FILL = { DOM_CONTENT, DOM_FONT, DOM_CREDITS, DOM_COLOR_ITEM_FILL };
    static const config_path FONT_CREDITS_COLOR_ITEM_OUTLINE = { DOM_CONTENT, DOM_FONT, DOM_CREDITS, DOM_COLOR_ITEM_OUTLINE };
    static const config_path FONT_CREDITS_COLOR_TITLE_FILL = { DOM_CONTENT, DOM_FONT, DOM_CREDITS, DOM_COLOR_TITLE_FILL };
    static const config_path FONT_CREDITS_COLOR_TITLE_OUTLINE = { DOM_CONTENT, DOM_FONT, DOM_CREDITS, DOM_COLOR_TITLE_OUTLINE };

    static const config_path FONT_SETTINGS_FILENAME = { DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_FILENAME };
    static const config_path FONT_SETTINGS_HEADING_SIZE = { DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_HEADING_SIZE };
    static const config_path FONT_SETTINGS_ITEM_SIZE = { DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_ITEM_SIZE };
    static const config_path FONT_SETTINGS_MESSAGE_SIZE = { DOM_CONTENT, DOM_FONT, DOM_SETTINGS, "message-size"s };
    static const config_path FONT_SETTINGS_TITLE_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_TITLE_FILL };
    static const config_path FONT_SETTINGS_TITLE_COLOR_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_TITLE_OUTLINE };
    static const config_path FONT_SETTINGS_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_FILL };
    static const config_path FONT_SETTINGS_COLOR_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_OUTLINE };
    static const config_path FONT_SETTINGS_COLOR_ACTIVE_FILL{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_ACTIVE_FILL };
    static const config_path FONT_SETTINGS_COLOR_ACTIVE_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_ACTIVE_OUTLINE };
    static const config_path FONT_SETTINGS_COLOR_SELECTION_FILL{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_SELECTION_FILL };
    static const config_path FONT_SETTINGS_COLOR_SELECTION_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_SETTINGS, DOM_COLOR_SELECTION_OUTLINE };

    static const config_path FONT_LEVEL_SELECT_FILENAME = { DOM_CONTENT, DOM_FONT, DOM_LEVEL_SELECT, DOM_FILENAME };
    static const config_path FONT_LEVEL_SELECT_ITEM_SIZE = { DOM_CONTENT, DOM_FONT, DOM_LEVEL_SELECT, DOM_ITEM_SIZE };

    static const config_path FONT_CHALLENGES_FILENAME = { DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_FILENAME };
    static const config_path FONT_CHALLENGES_TITLE_SIZE = { DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_TITLE_SIZE };
    static const config_path FONT_CHALLENGES_TITLE_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_TITLE_FILL };
    static const config_path FONT_CHALLENGES_TITLE_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_TITLE_OUTLINE };
    static const config_path FONT_CHALLENGES_HEADER_SIZE = { DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_HEADER_SIZE };
    static const config_path FONT_CHALLENGES_HEADER_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_HEADER_FILL };
    static const config_path FONT_CHALLENGES_HEADER_COLOR_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_HEADER_OUTLINE };

    static const config_path FONT_CHALLENGES_ITEM_SIZE = { DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_ITEM_SIZE };
    static const config_path FONT_CHALLENGES_ITEM_COLOR_FILL{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_ITEM_FILL };
    static const config_path FONT_CHALLENGES_ITEM_COLOR_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_ITEM_OUTLINE };
    static const config_path FONT_CHALLENGES_ITEM_COLOR_ACTIVE_FILL{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_ITEM_ACTIVE_FILL };
    static const config_path FONT_CHALLENGES_ITEM_COLOR_ACTIVE_OUTLINE{ DOM_CONTENT, DOM_FONT, DOM_CHALLENGES, DOM_COLOR_ITEM_ACTIVE_OUTLINE };

    static const config_path AUDIO_MENU_ACTIVATE = { DOM_CONTENT, DOM_AUDIO, DOM_MENU, "activate"s };
    static const config_path AUDIO_MENU_ACCEPT = { DOM_CONTENT, DOM_AUDIO, DOM_MENU, "accept"s };
    static const config_path AUDIO_GAMEPLAY_STEP = { DOM_CONTENT, DOM_AUDIO, DOM_GAME, "step"s };
    static const config_path AUDIO_GAMEPLAY_RULE_CHANGED = { DOM_CONTENT, DOM_AUDIO, DOM_GAME, "rule-changed"s };
    static const config_path AUDIO_GAMEPLAY_LEVEL_COMPLETE = { DOM_CONTENT, DOM_AUDIO, DOM_GAME, "level-complete"s };
    static const config_path AUDIO_GAMEPLAY_SINK = { DOM_CONTENT, DOM_AUDIO, DOM_GAME, "sink"s };
    static const config_path AUDIO_GAMEPLAY_BURN = { DOM_CONTENT, DOM_AUDIO, DOM_GAME, "burn"s };

    static const config_path MUSIC_BACKGROUND_1 = { DOM_CONTENT, DOM_MUSIC, "background-1"s };
    static const config_path MUSIC_BACKGROUND_2 = { DOM_CONTENT, DOM_MUSIC, "background-2"s };
    static const config_path MUSIC_BACKGROUND_3 = { DOM_CONTENT, DOM_MUSIC, "background-3"s };
    static const config_path MUSIC_BACKGROUND_4 = { DOM_CONTENT, DOM_MUSIC, "background-4"s };

    static const config_path IMAGE_MENU_BACKGROUND = { DOM_CONTENT, DOM_IMAGE, "menu-background"s };
    static const config_path IMAGE_SCORING_CHECKMARK_EMPTY = { DOM_CONTENT, DOM_IMAGE, "checkmark-empty"s };
    static const config_path IMAGE_SCORING_CHECKMARK_FILLED = { DOM_CONTENT, DOM_IMAGE, "checkmark-pre-filled"s };
    static const config_path IMAGE_SCORING_CHECKMARK_BC_FILLED = { DOM_CONTENT, DOM_IMAGE, "checkmark-bc-filled"s };

    static const config_path IMAGE_HELP_KEYBOARD_MOVEMENT = { DOM_CONTENT, DOM_IMAGE, "help-keyboard-movement"s };
    static const config_path IMAGE_HELP_KEYBOARD_CAMERA = { DOM_CONTENT, DOM_IMAGE, "help-keyboard-camera"s };
    static const config_path IMAGE_HELP_CONTROLLER_MOVEMENT_XBOX = { DOM_CONTENT, DOM_IMAGE, "help-controller-movement-xbox"s };
    static const config_path IMAGE_HELP_CONTROLLER_CAMERA_XBOX = { DOM_CONTENT, DOM_IMAGE, "help-controller-camera-xbox"s };
    static const config_path IMAGE_HELP_CONTROLLER_MOVEMENT_PS = { DOM_CONTENT, DOM_IMAGE, "help-controller-movement-ps"s };
    static const config_path IMAGE_HELP_CONTROLLER_CAMERA_PS = { DOM_CONTENT, DOM_IMAGE, "help-controller-camera-ps"s };

    static const config_path IMAGE_PARTICLE_GENERAL = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-general"s, DOM_FILENAME };
    static const config_path IMAGE_PARTICLE_GENERAL_SPRITE_COUNT = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-general"s, DOM_SPRITE_COUNT };
    static const config_path IMAGE_PARTICLE_GENERAL_SPRITE_TIME = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-general"s, DOM_SPRITE_TIME };
    static const config_path IMAGE_PARTICLE_BURN = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-burn"s, DOM_FILENAME };
    static const config_path IMAGE_PARTICLE_BURN_SPRITE_COUNT = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-burn"s, DOM_SPRITE_COUNT };
    static const config_path IMAGE_PARTICLE_BURN_SPRITE_TIME = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-burn"s, DOM_SPRITE_TIME };
    static const config_path IMAGE_PARTICLE_SINK = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-sink"s, DOM_FILENAME };
    static const config_path IMAGE_PARTICLE_SINK_SPRITE_COUNT = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-sink"s, DOM_SPRITE_COUNT };
    static const config_path IMAGE_PARTICLE_SINK_SPRITE_TIME = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-sink"s, DOM_SPRITE_TIME };
    static const config_path IMAGE_PARTICLE_NEW_PHRASE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-new-phrase"s, DOM_FILENAME };
    static const config_path IMAGE_PARTICLE_NEW_PHRASE_SPRITE_COUNT = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-new-phrase"s, DOM_SPRITE_COUNT };
    static const config_path IMAGE_PARTICLE_NEW_PHRASE_SPRITE_TIME = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-new-phrase"s, DOM_SPRITE_TIME };
    static const config_path IMAGE_PARTICLE_LEVEL_COMPLETE_PRE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-level-complete-pre"s, DOM_FILENAME };
    static const config_path IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_COUNT_PRE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-level-complete-pre"s, DOM_SPRITE_COUNT };
    static const config_path IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_TIME_PRE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-level-complete-pre"s, DOM_SPRITE_TIME };
    static const config_path IMAGE_PARTICLE_LEVEL_COMPLETE_BC = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-level-complete-bc"s, DOM_FILENAME };
    static const config_path IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_COUNT_BC = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-level-complete-bc"s, DOM_SPRITE_COUNT };
    static const config_path IMAGE_PARTICLE_LEVEL_COMPLETE_SPRITE_TIME_BC = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "particle-level-complete-bc"s, DOM_SPRITE_TIME };

    static const config_path IMAGE_HEX_OUTLINE_256 = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_STATIC, "hex-outline-256"s, DOM_FILENAME };
    static const config_path IMAGE_HEX_OUTLINE_256_COLOR = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_STATIC, "hex-outline-256"s, DOM_SPRITE_COLOR };
    static const config_path IMAGE_PHRASE_DIRECTION_1024 = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_STATIC, "phrase-direction-1024"s, DOM_FILENAME };
    static const config_path IMAGE_PHRASE_DIRECTION_1024_COLOR = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_STATIC, "phrase-direction-1024"s, DOM_SPRITE_COLOR };
    static const config_path IMAGE_I_AM_HIGHLIGHT_512 = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "i-am-highlight-512"s, DOM_FILENAME };
    static const config_path IMAGE_GOAL_HIGHLIGHT = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "goal-highlight"s, DOM_FILENAME };
    static const config_path IMAGE_SEND_HIGHLIGHT = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_ANIMATED, "send-highlight"s, DOM_FILENAME };

    static const config_path IMAGE_ENTITY_ANIMATED_WALL = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "wall"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_FLOOR = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "floor"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_GRASS = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "grass"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_FLOWERS = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "flowers"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_PURPLE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "purple"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_PURPLE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_purple"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_GREY = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "grey"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_GREY = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_grey"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_GREEN = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "green"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_GREEN = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_green"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BLUE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "blue"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_BLUE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_blue"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_RED = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "red"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_RED = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_red"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BROWN = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "brown"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_BROWN = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_brown"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_YELLOW = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "yellow"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_YELLOW = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_yellow"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BACKGROUND_WHITE = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "background_white"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_ANIMATED_BLACK = { DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "black"s, DOM_FILENAME_ANIMATED };

    static const config_path IMAGE_ENTITY_TEXT_I{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-i", DOM_FILENAME_ANIMATED };

    static const config_path IMAGE_ENTITY_TEXT_IS{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-is"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_AM{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-am"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_CAN{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-can"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_AND{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-and"s, DOM_FILENAME_ANIMATED };

    static const config_path IMAGE_ENTITY_TEXT_GOAL{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-goal"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_CLIMB{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-climb"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_FLOAT{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-float"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_CHILL{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-chill"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_PUSH{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-push"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_PULL{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-pull"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_STOP{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-stop"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_STEEP{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-steep"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_WATER{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-water"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_HOT{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-hot"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_SEND{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-send"s, DOM_FILENAME_ANIMATED };

    static const config_path IMAGE_ENTITY_TEXT_WORD{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-word"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_WALL{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-wall"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_FLOOR{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-floor"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_FLOWERS{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-flowers"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_GRASS{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-grass"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_PURPLE{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-purple"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_GREY{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-grey"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_GREEN{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-green"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_BLUE{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-blue"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_RED{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-red"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_BROWN{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-brown"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_YELLOW{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-yellow"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_BLACK{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-black"s, DOM_FILENAME_ANIMATED };
    static const config_path IMAGE_ENTITY_TEXT_WHITE{ DOM_CONTENT, DOM_LEVELS, DOM_IMAGES_OBJECTS, "text-white"s, DOM_FILENAME_ANIMATED };

    static const config_path GAME_LEVELS = { DOM_CONTENT, DOM_LEVELS, "file"s };

} // namespace config
