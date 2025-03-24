# Developer Setup & Compiling Instructions

This page describes how to setup the development environment you need to compile and run the game.  Instructions for both Windows, Linux, and macOS are provided.  Familiarity with C++ development on these platforms is expected.

## Windows

The following development tools must be installed:

* Visual Studio 2022
* git (I also use GitExtensions)
* CMake
* clang-format (LLVM tools)

The next step is to clone and build the code.  The following steps can be used to do this.

1. Clone the repository.
1. Navigate into the `/src` folder
1. Initialize the submodules: `git submodule update --init --recursive`
1. Run the CMake GUI
   1. For the **Where is the source code** field navigate to the /src folder.
   1. For the **Where to build the binaries** use the same pathname from above, but add a `/build` folder to it.
   1. Press the **Configure** button.
   1. For each of the following projects in the CMake GUI, only the following settings should be set to checked, everything else should be unchecked:
      1. SFML
         * SFML_BUILD_AUDIO
         * SFML_BUILD_GRAPHICS
         * SFML_BUILD_WINDOW
         * SFML_USE_STATIC_STD_LIBS
      1. RAPIDJSON
         * RAPIDJSON_BUILD_CXX20
      1. CTTI
         * None should be checked
      1. BUILD
         * None should be checked
      1. Ungrouped Entries
         * None should be checked
   1. Press **Configure** again; all should be well this time.
   1. Press the **Generate** button.
1. Navigate to the `/build` folder
1. Double-click on the `ThatMakesSense.sln` file.  Alternatively select the 'Open Project' button within the CMake GUI.
1. Build the solution.
1. From the `/src` folder copy the `client.settings.json` file into the `/build/Debug` folder.
1. From the `/src` folder copy the `client.developer.json` file into the `/build/Debug` folder.
1. From the `/src` folder copy the `/assets` folder into the `/build/Debug` folder.
1. Inside the `/build/Debug` folder, run `ThatMakesSense.exe`
1. Enjoy!

## Linux

I am working on an Ubuntu distribution.  Therefore these instructions are based on that platform.  Other Linux distributions may require different steps, especially those not Debian based and without the apt package manager.

The following development tools must be installed:

* git
* g++ - A version that supports C++ 20 is required
* CMake (including the CMake GUI)
* (optional) clang-format
* The following probably need to be installed: `sudo apt install ...`
  * libx11-dev
  * xorg-dev
  * libgl1-mesa-dev
  * libopenal-dev
  * libvorbis-dev
  * libflac-dev
  * libudev-dev

The next step is to clone and build the code.  The following steps can be used to do this.

1. Clone the repository: `git clone https://github.com/ProfPorkins/ThatMakesSense.git`
1. Navigate into the `/src` folder
1. Initialize the submodules: `git submodule update --init --recursive`
1. Run the CMake GUI
   1. For the **Where is the source code** field navigate to the /src folder.
   1. For the **Where to build the binaries** use the same pathname from above, but add a `/build` folder to it.
   1. Press the **Configure** button.
   1. For each of the following projects in the CMake GUI, only the following settings should be set to checked, everything else should be unchecked:
      1. SFML
         * SFML_BUILD_AUDIO
         * SFML_BUILD_GRAPHICS
         * SFML_BUILD_WINDOW
      1. RAPIDJSON
         * RAPIDJSON_BUILD_CXX20
      1. CTTI
         * None should be checked
      1. BUILD
         * None should be checked
      1. Ungrouped Entries
         * None should be checked
   1. Press **Configure** again; all should be well this time.
   1. Press the **Generate** button.
1. Navigate to the `/build` folder
1. Build the solution by typing `make`.
1. From the `/src` folder copy the `client.settings.json` file into the `/build/Debug` folder.
1. From the `/src` folder copy the `client.developer.json` file into the `/build/Debug` folder.
1. From the `/src` folder copy the `/assets` folder into the `/build/Debug` folder.
1. Inside the `/build` folder, run `ThatMakesSense`
1. Enjoy!

## macOS

The following development tools must be installed:

* XCode
* git
* CMake

1. Clone the repository: `git clone https://github.com/ProfPorkins/ThatMakesSense.git`
1. Navigate into the `/src` folder
1. Initialize the submodules: `git submodule update --init --recursive`
1. Using git, checkout the `macOS` branch: `git checkout macOS`
1. Run the CMake GUI
   1. For the **Where is the source code** field navigate to the /src folder.
   1. For the **Where to build the binaries** use the same pathname from above, but add a `/build` folder to it.
   1. Press the **Configure** button.
   1. For each of the following projects in the CMake GUI, only the following settings should be set to checked, everything else should be unchecked:
      1. SFML
         * SFML_BUILD_AUDIO
         * SFML_BUILD_GRAPHICS
         * SFML_BUILD_WINDOW
      1. RAPIDJSON
         * RAPIDJSON_BUILD_CXX20
      1. CTTI
         * None should be checked
      1. BUILD
         * None should be checked
      1. Ungrouped Entries
         * None should be checked
   1. Press **Configure** again; all should be well this time.
   1. Press the **Generate** button.
1. Navigate to the `/build` folder
1. Build the solution by typing `make`.
1. From the `/src` folder copy the `client.settings.json` file into the `/build/Debug` folder.
1. From the `/src` folder copy the `client.developer.json` file into the `/build/Debug` folder.
1. From the `/src` folder copy the `/assets` folder into the `/build/Debug` folder.
1. Inside the `/build` folder, run `ThatMakesSense`
1. Enjoy!
