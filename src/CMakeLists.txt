cmake_minimum_required(VERSION 3.10)

set(PROJECT_NAME "ThatMakesSense")
set(UNIT_TEST_RUNNER "UnitTestRunner")
project(${PROJECT_NAME})

# 
# Great git submodules Reference: https://www.vogella.com/tutorials/GitSubmodules/article.html
#

#
# ------------------------ Add SFML ------------------------
# This causes the SFML .dll files to be copied into the executable folder.
# Only need this for development time, for a production release statically link
# everything together: set BUILD_SHARED_LIBS to FALSE
# Reference: https://en.sfml-dev.org/forums/index.php?topic=24070.0
# Reference: https://stackoverflow.com/questions/10671916/how-to-copy-dll-files-into-the-same-folder-as-the-executable-using-cmake
#
set (CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})
set (CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR})

add_subdirectory(ThirdParty/SFML)
include_directories(ThirdParty/SFML/include)

#
# ------------------------ Add CTTI ------------------------
# Reference: https://github.com/Manu343726/ctti
#
add_subdirectory(ThirdParty/ctti)
include_directories(ThirdParty/ctti/include)

#
# ------------------------ Add RapidJSON ------------------------
# Reference: https://github.com/Tencent/rapidjson
#
add_subdirectory(ThirdParty/rapidjson)
include_directories(ThirdParty/rapidjson/include)

#
# ------------------------ Add the Client Project ------------------------
# Manually specifying all the source files.
#
set(CLIENT_HEADER_FILES
    GameModel.hpp
    Level.hpp
    Levels.hpp
    )
set(CLIENT_SOURCE_FILES
    main.cpp
    GameModel.cpp
    Level.cpp
    Levels.cpp
    )

set(UNIT_TEST_HEADER_FILES
    Level.hpp
    Levels.hpp
    components/Ability.hpp
    components/AnimatedSprite.hpp
    components/Audio.hpp
    components/Camera.hpp
    components/Component.hpp
    components/InputControlled.hpp
    components/Noun.hpp
    components/Object.hpp
    components/Position.hpp
    components/Property.hpp
    components/StaticSprite.hpp
    components/Verb.hpp
    entities/Entity.hpp
    entities/Factory.hpp
    misc/HexCoord.hpp
    misc/math.hpp
    misc/misc.hpp
    misc/sha512.hpp
    services/Configuration.hpp
    services/ConfigurationPath.hpp
    services/Content.hpp
    services/ContentKey.hpp
    services/ControllerInput.hpp
    services/Scoring.hpp
    services/ThreadPool.hpp
    services/concurrency/ConcurrentQueue.hpp
    services/concurrency/ConcurrentTaskGraph.hpp
    services/concurrency/Task.hpp
    services/concurrency/WorkerThread.hpp
    systems/parser/Parser.hpp
    systems/parser/PhraseSearch.hpp
    systems/parser/SemanticParser.hpp
    )
set(UNIT_TEST_SOURCE_FILES
    Level.cpp
    Levels.cpp
    entities/Entity.cpp
    entities/Factory.cpp
    misc/HexCoord.cpp
    misc/math.cpp
    misc/misc.cpp
    misc/sha512.cpp
    services/Configuration.cpp
    services/Content.cpp
    services/Scoring.cpp
    services/ControllerInput.cpp
    services/ThreadPool.cpp
    services/concurrency/ConcurrentTaskGraph.cpp
    services/concurrency/Task.cpp
    services/concurrency/WorkerThread.cpp
    systems/parser/Parser.cpp
    systems/parser/PhraseSearch.cpp
    systems/parser/SemanticParser.cpp
   )

set(UNIT_TEST_TESTING_HEADER_FILES
    )
set(UNIT_TEST_TESTING_SOURCE_FILES
    testing/TestMain.cpp
    testing/TestConcurrentQueue.cpp
    testing/TestConcurrentTaskGraph.cpp
    testing/TestHex.cpp
    testing/TestParser.cpp
    testing/TestPhraseSearch.cpp
    testing/TestSemanticParse.cpp
    )

set(CLIENT_COMPONENTS_HEADERS
    components/Ability.hpp
    components/AnimatedSprite.hpp
    components/Audio.hpp
    components/Camera.hpp
    components/Challenge.hpp
    components/Component.hpp
    components/Hint.hpp
    components/InputControlled.hpp
    components/Noun.hpp
    components/Object.hpp
    components/PhraseDirection.hpp
    components/Position.hpp
    components/Property.hpp
    components/StaticSprite.hpp
    components/Verb.hpp
    )
set(CLIENT_COMPONENTS_SOURCES
    )

set(CLIENT_ENTITIES_HEADERS
    entities/Entity.hpp
    entities/Factory.hpp
    )
set(CLIENT_ENTITIES_SOURCES
    entities/Entity.cpp
    entities/Factory.cpp
    )

set(CLIENT_MISC_HEADERS
    misc/HexCoord.hpp
    misc/math.hpp
    misc/misc.hpp
    misc/sha512.hpp
    )
set(CLIENT_MISC_SOURCES
    misc/HexCoord.cpp
    misc/math.cpp
    misc/misc.cpp
    misc/sha512.cpp
    )

set(CLIENT_SERVICES_HEADERS
    services/Audio.hpp
    services/Configuration.hpp
    services/ConfigurationPath.hpp
    services/Content.hpp
    services/ContentKey.hpp
    services/ControllerInput.hpp
    services/KeyboardInput.hpp
    services/MouseInput.hpp
    services/Scoring.hpp
    services/ThreadPool.hpp
    )
set(CLIENT_SERVICES_SOURCES
    services/Audio.cpp
    services/Configuration.cpp
    services/Content.cpp
    services/ControllerInput.cpp
    services/KeyboardInput.cpp
    services/MouseInput.cpp
    services/Scoring.cpp
    services/ThreadPool.cpp
    )

set(CLIENT_SERVICES_CONCURRENCY_HEADERS
    services/concurrency/ConcurrentQueue.hpp
    services/concurrency/ConcurrentTaskGraph.hpp
    services/concurrency/Task.hpp
    services/concurrency/WorkerThread.hpp
    )
set(CLIENT_SERVICES_CONCURRENCY_SOURCES
    services/concurrency/ConcurrentTaskGraph.cpp
    services/concurrency/Task.cpp
    services/concurrency/WorkerThread.cpp
    )

set(CLIENT_SYSTEMS_HEADERS
    systems/AnimatedSprite.hpp
    systems/Camera.hpp
    systems/Challenge.hpp
    systems/Completion.hpp
    systems/Hint.hpp
    systems/Movement.hpp
    systems/Particle.hpp
    systems/ParticleSystem.hpp
    systems/RendererChallenge.hpp
    systems/RendererHexGridAnimatedSprites.hpp
    systems/RendererHexGrid.hpp
    systems/RendererHexGridCoords.hpp
    systems/RendererHexGridGoalHighlight.hpp
    systems/RendererHexGridHighlight.hpp
    systems/RendererHexGridIHighlight.hpp
    systems/RendererHexGridOutline.hpp
    systems/RendererHexGridPhraseDirection.hpp
    systems/RendererHexGridSendHighlight.hpp
    systems/RendererHexGridStaticSprites.hpp
    systems/RendererHint.hpp
    systems/RendererParticleSystem.hpp
    systems/RuleExecute.hpp
    systems/RuleSearch.hpp
    systems/System.hpp
    systems/Undo.hpp
    )
set(CLIENT_SYSTEMS_SOURCES
    systems/AnimatedSprite.cpp
    systems/Camera.cpp
    systems/Challenge.cpp
    systems/Completion.cpp
    systems/Hint.cpp
    systems/Movement.cpp
    systems/ParticleSystem.cpp
    systems/RendererChallenge.cpp
    systems/RendererHexGridAnimatedSprites.cpp
    systems/RendererHexGrid.cpp
    systems/RendererHexGridCoords.cpp
    systems/RendererHexGridGoalHighlight.cpp
    systems/RendererHexGridHighlight.cpp
    systems/RendererHexGridIHighlight.cpp
    systems/RendererHexGridOutline.cpp
    systems/RendererHexGridPhraseDirection.cpp
    systems/RendererHexGridStaticSprites.cpp
    systems/RendererHexGridSendHighlight.cpp
    systems/RendererHint.cpp
    systems/RendererParticleSystem.cpp
    systems/RuleExecute.cpp
    systems/RuleSearch.cpp
    systems/System.cpp
    systems/Undo.cpp
    )

set(CLIENT_PARTICLE_EFFECTS_HEADERS
    systems/effects/BurnEffect.hpp
    systems/effects/LevelCompletedEffect.hpp
    systems/effects/NewPhraseEffect.hpp
    systems/effects/ParticleEffect.hpp
    systems/effects/RuleChangedEffect.hpp
    systems/effects/SinkEffect.hpp
    )
set(CLIENT_PARTICLE_EFFECTS_SOURCES
    systems/effects/BurnEffect.cpp
    systems/effects/LevelCompletedEffect.cpp
    systems/effects/NewPhraseEffect.cpp
    systems/effects/ParticleEffect.cpp
    systems/effects/RuleChangedEffect.cpp
    systems/effects/SinkEffect.cpp
    )

set(CLIENT_SYSTEMS_PARSING_HEADERS
    systems/parser/Parser.hpp
    systems/parser/PhraseSearch.hpp
    systems/parser/SemanticParser.hpp
    )
set(CLIENT_SYSTEMS_PARSING_SOURCES
    systems/parser/Parser.cpp
    systems/parser/PhraseSearch.cpp
    systems/parser/SemanticParser.cpp
    )

set(CLIENT_UIFRAMEWORK_HEADERS
    UIFramework/Activate.hpp
    UIFramework/Element.hpp
    UIFramework/KeyboardOption.hpp
    UIFramework/MenuItem.hpp
    UIFramework/Selection.hpp
    UIFramework/Text.hpp
    )
set(CLIENT_UIFRAMEWORK_SOURCES
    UIFramework/Activate.cpp
    UIFramework/KeyboardOption.cpp
    UIFramework/MenuItem.cpp
    UIFramework/Selection.cpp
    UIFramework/Text.cpp
    )

set(CLIENT_VIEWS_HEADERS
    views/About.hpp
    views/Credits.hpp
    views/Gameplay.hpp
    views/HelpControls.hpp
    views/HelpDisplay.hpp
    views/MainMenu.hpp
    views/MenuView.hpp
    views/Puzzles.hpp
    views/SettingsControls.hpp
    views/SettingsControlsCamera.hpp
    views/SettingsControlsMovement.hpp
    views/SettingsControlsOther.hpp
    views/SettingsGraphicsAudio.hpp
    views/View.hpp
    views/ViewState.hpp
    )
set(CLIENT_VIEWS_SOURCES
    views/About.cpp
    views/Credits.cpp
    views/Gameplay.cpp
    views/HelpControls.cpp
    views/HelpDisplay.cpp
    views/MainMenu.cpp
    views/MenuView.cpp
    views/Puzzles.cpp
    views/SettingsControls.cpp
    views/SettingsControlsCamera.cpp
    views/SettingsControlsMovement.cpp
    views/SettingsControlsOther.cpp
    views/SettingsGraphicsAudio.cpp
    )

#
# Organize the files into some logical groups
#
source_group("Main\\Header Files" FILES ${CLIENT_HEADER_FILES})
source_group("Main\\Source Files" FILES ${CLIENT_SOURCE_FILES})

source_group("ECS\\Components\\Header Files" FILES ${CLIENT_COMPONENTS_HEADERS})
source_group("ECS\\Components\\Source Files" FILES ${CLIENT_COMPONENTS_SOURCES})

source_group("ECS\\Entities\\Header Files" FILES ${CLIENT_ENTITIES_HEADERS})
source_group("ECS\\Entities\\Source Files" FILES ${CLIENT_ENTITIES_SOURCES})

source_group("ECS\\Systems\\Header Files" FILES ${CLIENT_SYSTEMS_HEADERS})
source_group("ECS\\Systems\\Source Files" FILES ${CLIENT_SYSTEMS_SOURCES})

source_group("ECS\\Systems\\Effects\\Header Files" FILES ${CLIENT_PARTICLE_EFFECTS_HEADERS})
source_group("ECS\\Systems\\Effects\\Source Files" FILES ${CLIENT_PARTICLE_EFFECTS_SOURCES})

source_group("ECS\\Systems\\Parser\\Header Files" FILES ${CLIENT_SYSTEMS_PARSING_HEADERS})
source_group("ECS\\Systems\\Parser\\Source Files" FILES ${CLIENT_SYSTEMS_PARSING_SOURCES})

source_group("Levels\\Header Files" FILES ${CLIENT_LEVEL_HEADERS})
source_group("Levels\\Source Files" FILES ${CLIENT_LEVEL_SOURCES})

source_group("Misc\\Header Files" FILES ${CLIENT_MISC_HEADERS})
source_group("Misc\\Source Files" FILES ${CLIENT_MISC_SOURCES})

source_group("Services\\Header Files" FILES ${CLIENT_SERVICES_HEADERS})
source_group("Services\\Source Files" FILES ${CLIENT_SERVICES_SOURCES})

source_group("Services\\Concurrency\\Header Files" FILES ${CLIENT_SERVICES_CONCURRENCY_HEADERS})
source_group("Services\\Concurrency\\Source Files" FILES ${CLIENT_SERVICES_CONCURRENCY_SOURCES})

source_group("UI\\Framework\\Header Files" FILES ${CLIENT_UIFRAMEWORK_HEADERS})
source_group("UI\\Framework\\Source Files" FILES ${CLIENT_UIFRAMEWORK_SOURCES})

source_group("UI\\Views\\Header Files" FILES ${CLIENT_VIEWS_HEADERS})
source_group("UI\\Views\\Source Files" FILES ${CLIENT_VIEWS_SOURCES})

#
# Need a list of all code files for convenience
#
set(CLIENT_CODE_FILES
    ${CLIENT_SOURCE_FILES}
    ${CLIENT_HEADER_FILES}
    ${CLIENT_COMPONENTS_HEADERS}
    ${CLIENT_COMPONENTS_SOURCES}
    ${CLIENT_ENTITIES_HEADERS}
    ${CLIENT_ENTITIES_SOURCES}
    ${CLIENT_LEVEL_HEADERS}
    ${CLIENT_LEVEL_SOURCES}
    ${CLIENT_MISC_HEADERS}
    ${CLIENT_MISC_SOURCES}
    ${CLIENT_PARTICLE_EFFECTS_HEADERS}
    ${CLIENT_PARTICLE_EFFECTS_SOURCES}
    ${CLIENT_SYSTEMS_PARSING_HEADERS}
    ${CLIENT_SYSTEMS_PARSING_SOURCES}
    ${CLIENT_SERVICES_HEADERS}
    ${CLIENT_SERVICES_SOURCES}
    ${CLIENT_SERVICES_CONCURRENCY_HEADERS}
    ${CLIENT_SERVICES_CONCURRENCY_SOURCES}
    ${CLIENT_SYSTEMS_HEADERS}
    ${CLIENT_SYSTEMS_SOURCES}
    ${CLIENT_UIFRAMEWORK_HEADERS}
    ${CLIENT_UIFRAMEWORK_SOURCES}
    ${CLIENT_VIEWS_HEADERS}
    ${CLIENT_VIEWS_SOURCES}
    )

set(UNIT_TEST_CODE_FILES
    ${UNIT_TEST_SOURCE_FILES}
    ${UNIT_TEST_HEADER_FILES}
    ${UNIT_TEST_TESTING_HEADER_FILES}
    ${UNIT_TEST_TESTING_SOURCE_FILES}
    )

set(UNIT_TEST_ONLY_CODE_FILES
    ${UNIT_TEST_TESTING_HEADER_FILES}
    ${UNIT_TEST_TESTING_SOURCE_FILES}
    )

#
# This defines the program and unit test executable targets
#
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    # For Windows, linking with sfml-main so SFML's main is used, allow subsystem type to
    # be Windows instead of Console, to prevent the display of a console window on startup.
    # Reference: https://en.sfml-dev.org/forums/index.php?topic=6278.0
    add_executable(${PROJECT_NAME} WIN32 ${CLIENT_CODE_FILES})
    add_executable(${UNIT_TEST_RUNNER} ${UNIT_TEST_CODE_FILES})
    target_link_libraries(${PROJECT_NAME} sfml-main sfml-graphics sfml-audio sfml-system sfml-window)
    target_link_libraries(${UNIT_TEST_RUNNER} sfml-main sfml-graphics sfml-audio sfml-system sfml-window)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    add_executable(${PROJECT_NAME} ${CLIENT_CODE_FILES})
    add_executable(${UNIT_TEST_RUNNER} ${UNIT_TEST_CODE_FILES})
    target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-system sfml-window)
    target_link_libraries(${UNIT_TEST_RUNNER} sfml-graphics sfml-audio sfml-system sfml-window)
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    add_executable(${PROJECT_NAME} ${CLIENT_CODE_FILES})
    add_executable(${UNIT_TEST_RUNNER} ${UNIT_TEST_CODE_FILES})
    target_link_libraries(${PROJECT_NAME} sfml-graphics sfml-audio sfml-system sfml-window)
    target_link_libraries(${UNIT_TEST_RUNNER} sfml-graphics sfml-audio sfml-system sfml-window)
endif()
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(${UNIT_TEST_RUNNER} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

#
# Want the C++ 20 standard for our project
#
set_property(TARGET ${PROJECT_NAME} PROPERTY CXX_STANDARD 20)
set_property(TARGET ${UNIT_TEST_RUNNER} PROPERTY CXX_STANDARD 20)

#
# Enable a lot of warnings, forcing better code to be written
# /JMC is for debugging "Just My Code"
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE /W4 /permissive- /MP /JMC)
    target_compile_options(${UNIT_TEST_RUNNER} PRIVATE /W4 /permissive- /MP /JMC)
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wall -Wextra -pedantic) # -Wconversion -Wsign-conversion
    target_compile_options(${UNIT_TEST_RUNNER} PRIVATE -O3 -Wall -Wextra -pedantic)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pthread)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++ -Wl,-R,\$ORIGIN")
elseif (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    target_compile_options(${PROJECT_NAME} PRIVATE -O3 -Wall -Wextra -pedantic)
    target_compile_options(${UNIT_TEST_RUNNER} PRIVATE -O3 -Wall -Wextra -pedantic)
    set(CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS} -pthread)
    set(CMAKE_EXE_LINKER_FLAGS "-static-libgcc -static-libstdc++")
endif()

#
# Enable static multithreaded library linking for MSVC
# Reference: https://cmake.org/cmake/help/latest/prop_tgt/MSVC_RUNTIME_LIBRARY.html#prop_tgt:MSVC_RUNTIME_LIBRARY
if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    target_compile_options(${PROJECT_NAME} PRIVATE "/MT$<$<CONFIG:Debug>:d>")
endif()

add_dependencies(${PROJECT_NAME} sfml-graphics sfml-audio sfml-system sfml-window)

#
# Copy the default configuration & scores files into the build folder so they are
# available at runtime
# 
set(DEFAULT_CONFIG_FILE ${CMAKE_CURRENT_SOURCE_DIR}/client.settings.json)
set(BUILD_CONFIG_FILE ${CMAKE_CURRENT_BINARY_DIR}/client.settings.json)
if (NOT EXISTS ${BUILD_CONFIG_FILE} OR ${DEFAULT_CONFIG_FILE} IS_NEWER_THAN ${BUILD_CONFIG_FILE})
   configure_file(${DEFAULT_CONFIG_FILE} ${BUILD_CONFIG_FILE} COPYONLY)
endif()

set(DEFAULT_SCORES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/client.scores.json)
set(BUILD_SCORES_FILE ${CMAKE_CURRENT_BINARY_DIR}/client.scores.json)
if (NOT EXISTS ${BUILD_SCORES_FILE} OR ${DEFAULT_SCORES_FILE} IS_NEWER_THAN ${BUILD_SCORES_FILE})
   configure_file(${DEFAULT_SCORES_FILE} ${BUILD_SCORES_FILE} COPYONLY)
endif()

#
# This shouldn't be included as part of a release.  Have only put it in here so that
# anyone who builds the probject can have it "just work" without having to do anything else.
#
set(DEFAULT_SCORES_FILE ${CMAKE_CURRENT_SOURCE_DIR}/client.developer.json)
set(BUILD_SCORES_FILE ${CMAKE_CURRENT_BINARY_DIR}/client.developer.json)
if (NOT EXISTS ${BUILD_SCORES_FILE} OR ${DEFAULT_SCORES_FILE} IS_NEWER_THAN ${BUILD_SCORES_FILE})
   configure_file(${DEFAULT_SCORES_FILE} ${BUILD_SCORES_FILE} COPYONLY)
endif()

#
# ------------------------ Google Test ------------------------
#
include(FetchContent)
FetchContent_Declare(
    googletest
    GIT_REPOSITORY    https://github.com/google/googletest.git
    GIT_TAG           release-1.11.0
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googleTest)

target_link_libraries(${UNIT_TEST_RUNNER} gtest_main)
target_link_libraries(${PROJECT_NAME} gtest)

#
# ------------------------ Clang Format ------------------------
#
#
# Prepare a pre-build step to run clang-format over all the [ch]pp source files.
# Start by finding the location of the clang-format executable.
#
find_program(CLANG_FORMAT "clang-format")
if (CLANG_FORMAT)
    #
    # Need to take the simple source file locations used for the project and get their full
    # file system locations for use in putting together the clang-format command line
    #
    unset(CODE_FILES_PATHS)
    foreach(CODE_FILE ${CLIENT_CODE_FILES} ${UNIT_TEST_ONLY_CODE_FILES})
        get_source_file_property(WHERE ${CODE_FILE} LOCATION)
        set(CODE_FILES_PATHS ${CODE_FILES_PATHS} ${WHERE})
    endforeach()

    #
    # This creates the clang-format target/command
    #
    add_custom_target(
        ClangFormat
        COMMAND ${CLANG_FORMAT}
        -i
        -style=file
        ${CODE_FILES_PATHS}
    )
else()
    message("Unable to find clang-format")
endif()


#
# Move the assets into the build folder so they load at runtime
#
set(ASSETS_DIR assets)
set(ASSETS_FONT_DIR ${ASSETS_DIR}/fonts)
set(ASSETS_IMAGE_DIR ${ASSETS_DIR}/images)
set(ASSETS_AUDIO_DIR ${ASSETS_DIR}/audio)
set(ASSETS_MUSIC_DIR ${ASSETS_DIR}/music)
set(ASSETS_LEVELS_DIR ${ASSETS_DIR}/levels)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_FONT_DIR}/Chewy-Regular.ttf
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_FONT_DIR}/Chewy-Regular.ttf
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_FONT_DIR}/Roboto-Regular.ttf
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_FONT_DIR}/Roboto-Regular.ttf
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_FONT_DIR}/SourceCodePro-Bold.ttf
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_FONT_DIR}/SourceCodePro-Bold.ttf
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/checkmark-empty.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/checkmark-empty.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/checkmark-pre-filled.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/checkmark-pre-filled.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/checkmark-bc-filled.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/checkmark-bc-filled.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-empty.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-empty.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-outline-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-outline-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-filled-animated-512.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-filled-animated-512.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-textured.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-textured.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-plastic.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-plastic.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-gravel.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-gravel.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-floor.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-floor.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-grass.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-grass.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/hex-flowers.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/hex-flowers.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/i-am-highlight-animated-512.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/i-am-highlight-animated-512.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/goal-highlight.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/goal-highlight.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/send-highlight.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/send-highlight.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/direction-chevron-1024.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/direction-chevron-1024.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/particle-general.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/particle-general.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/particle-burn.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/particle-burn.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/particle-bubble.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/particle-bubble.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/particle-new-phrase.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/particle-new-phrase.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/particle-checkmark-pre.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/particle-checkmark-pre.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/particle-checkmark-bc.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/particle-checkmark-bc.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-is-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-is-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-am-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-am-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-can-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-can-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-and-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-and-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-climb-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-climb-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-float-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-float-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-chill-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-chill-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-hot-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-hot-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-send-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-send-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-steep-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-steep-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-stop-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-stop-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-push-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-push-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-pull-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-pull-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-water-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-water-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-i-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-i-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-goal-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-goal-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-word-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-word-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-wall-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-wall-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-floor-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-floor-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-flowers-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-flowers-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-grass-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-grass-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-black-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-black-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-purple-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-purple-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-grey-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-grey-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-green-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-green-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-blue-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-blue-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-red-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-red-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-brown-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-brown-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/text-yellow-256.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/text-yellow-256.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/help-keyboard-movement.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/help-keyboard-movement.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/help-keyboard-camera.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/help-keyboard-camera.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/help-controller-movement-xbox.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/help-controller-movement-xbox.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/help-controller-movement-ps.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/help-controller-movement-ps.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/help-controller-camera-xbox.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/help-controller-camera-xbox.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_IMAGE_DIR}/help-controller-camera-ps.png
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_IMAGE_DIR}/help-controller-camera-ps.png
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_AUDIO_DIR}/menu-activate.wav
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_AUDIO_DIR}/menu-activate.wav
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_AUDIO_DIR}/menu-accept.wav
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_AUDIO_DIR}/menu-accept.wav
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_AUDIO_DIR}/step.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_AUDIO_DIR}/step.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_AUDIO_DIR}/fire.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_AUDIO_DIR}/fire.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_AUDIO_DIR}/watersplash.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_AUDIO_DIR}/watersplash.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_AUDIO_DIR}/rule-changed.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_AUDIO_DIR}/rule-changed.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_AUDIO_DIR}/level-complete.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_AUDIO_DIR}/level-complete.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_MUSIC_DIR}/desolate.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_MUSIC_DIR}/desolate.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_MUSIC_DIR}/evasion.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_MUSIC_DIR}/evasion.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_MUSIC_DIR}/soliloquy.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_MUSIC_DIR}/soliloquy.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_MUSIC_DIR}/wasteland-overdrive.ogg
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_MUSIC_DIR}/wasteland-overdrive.ogg
)

add_custom_command(
    TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_if_different
            ${CMAKE_CURRENT_SOURCE_DIR}/${ASSETS_LEVELS_DIR}/game.puzzles
            ${CMAKE_CURRENT_BINARY_DIR}/${ASSETS_LEVELS_DIR}/game.puzzles
)
