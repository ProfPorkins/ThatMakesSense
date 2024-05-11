# That Makes Sense

This repository contains the source code for the [_That Makes Sense_](https://store.steampowered.com/app/2166500/That_Makes_Sense/) game.  The source doesn't include the Steam specifics like the leader boards, achievements, and Steam overlay, but it is otherwise the complete source code.  In fact, while working on the documentation, I've made revisions to the code, that aren't in the release available on Steam.  None of the changes affect the game play, they are improvements to the implementation.  The reason for making it publicly available is to provide an example of a complete game.

## Developer Setup and Build Instructions

The code is written in C++, uses the [SFML](https://www.sfml-dev.org/), [CTTI](https://github.com/Manu343726/ctti), and [RapidJson](https://github.com/Tencent/rapidjson) libraries.  It builds and runs on Windows using MSVC, Linux using g++, and macOS using llvm.  Additionally, the project utilizes [CMake](https://cmake.org/) to generate platform specific build systems.

Instructions for Windows, Linux, and macOS are found at [this page](https://github.com/ProfPorkins/ThatMakesSense/blob/main/Developer-Setup.md).
