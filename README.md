# Shockolate (System Shock) port for PS Vita

## Install

Data files from System Shock are required. This port was only tested with `System Shock - Classic Edition` from GoG, so I have no idea if Mac data from Enhanced Edition would work properly with it.

To install the data files, you'll have to create the `ux0:data/systemshock/res/` folder on your PS Vita and copy `DATA` and `SOUND` folders from the installed System Shock folder there.

## Building

### Prerequisites
- VitaSDK
- SDL2
- SDL2-mixer

### Build
```
mkdir build && cd build
cmake .. -DCMAKE_TOOLCHAIN_FILE=$VITASDK/share/vita.toolchain.cmake -DENABLE_OPENGL=OFF -DVITA=true -DENABLE_FLUIDSYNTH=OFF -DENABLE_SDL2=ON -DCMAKE_BUILD_TYPE=None
make
```

## Port info

### Controls

- Left analog stick - Movement
- Right analog stick - Aiming/Cursor movement
- × - Jump
- ○ - Cycle between Stand/Crouch/Prone
- □ - Quick action/item pickup (instantly picks up the item, opens the door, etc)
- △ - Next weapon
- D-Pad Up - Grab/Arm _selected_ grenade
- D-Pad Down - Toggle between free look and mouse movement
- D-Pad Left/Right - Lean left/right
- L1 - LMB (Use)
- R1 - RMB (Attack)
- SELECT - Esc
- START - Use _selected_ drug
- Rear touchpad - Next/previous MFD (you can switch them by swiping up or down on the left/right side of the touchpad)
- Front touchpad - Mouse emulation

Gyro aiming is active by default. You can turn it off or adjust analog/gyro look speed by selecting `Vita input` option in the game menu.

## Additional info

### Tip for the new players

System Shock is an old-school game. It can be hard, confusing and even obtuse. It does not hold your hand and invites you to explore and improvise. It is advisable to start with normal (or if prefered lowered) difficulties settings. Take things slow, read, listen to audiologs, pay attention, make notes if needed. Pay attention to security levels on each deck, lower the better and also pay attention to your current objective. If needed, refer to manual (included for example with GOG release or can be found on internet) or ask other players.

### Known issues

Audiologs can freeze the game for a few seconds before starting the playback. Cinematics can take some time to load too.

### Special thanks

- Taffer from Discord for the inspiration, control ideas, testing and help with the README.



Shockolate - System Shock, but cross platform!
============================
Based on the source code for PowerPC released by Night Dive Studios, Incorporated.

[![Build Status TravisCI](https://travis-ci.org/Interrupt/systemshock.svg?branch=master)](https://travis-ci.org/Interrupt/systemshock) [![Build Status AppVeyor](https://ci.appveyor.com/api/projects/status/5fmcswq8n7ni0o9j/branch/master?svg=true)](https://ci.appveyor.com/project/Interrupt/systemshock)

GENERAL NOTES
=============

Shockolate is a cross platform source port of System Shock, using SDL2. This runs well on OSX, Linux, and Windows right now, with some missing features that need reviving due to not being included in the source code that was released.

The end goal for this project is something like what Chocolate Doom is for Doom: an experience that closely mimics the original, but portable and with some quality of life improvements including an OpenGL renderer and mod support!

Join our Discord to follow along with development: https://discord.gg/m45xPan

![work so far](https://i.imgur.com/kbVWQj4.gif)

Prerequisites
=======
  - Original cd-rom or SS:EE assets in a `res/data` folder next to the executable
    - Floppy disk assets are an older version that we can't load currently


Running
=======

## From a prebuilt package

Find a list of [downloadable packages](https://github.com/Interrupt/systemshock/releases/) for Linux, Mac and Windows. 32 and 64 bit versions are available for Linux and Windows.

## From source code

Prerequisites: 
- [CMake](https://cmake.org/download/) installed

Step 1. Build the dependencies:
* Windows: `build_win32.sh` or `build_win64.sh` (Git Bash and MinGW recommended)
* Linux/Mac: `build_deps.sh` or the CI build scripts in `osx-linux`
* Other: `build_deps.sh` 

Step 2. Build and run the game itself
```
cmake .
make systemshock
./systemshock
```

The following CMake options are supported in the build process:
* `ENABLE_SDL2` - use system or bundled SDL2 (ON/BUNDLED, default BUNDLED)
* `ENABLE_SOUND` - enable sound support (requires SDL2_mixer, ON/BUNDLED/OFF, default is BUNDLED)
* `ENABLE_FLUIDSYNTH` - enable FluidSynth MIDI support (ON/BUNDLED/OFF, default is BUNDLED)
* `ENABLE_OPENGL` - enable OpenGL support (ON/OFF, default ON)

If you find yourself needing to modify the build script for Shockolate itself, `CMakeLists.txt` is the place to look into.


Command line parameters
============

`-nosplash` Disables the splash screens, causes the game to start straight to the main menu

Modding Support
============
Shockolate supports loading mods and full on fan missions. Just point the executable at a mod file or folder and the game will load it in. So far mod loading supports additional `.res` and `.dat` files for resources and missions respectively.

Run a fan mission from a folder:
```
./systemshock /Path/To/My/Mission
```

Run a fan mission from specific files:
```
./systemshock my-archive.dat my-strings.res
```

Control modifications
=======

## Movement

Shockolate replaces the original game's movement with WASD controls, and uses `F` as the mouselook toggle hotkey. This differs from the Enhanced Edition's usage of `E` as the mouselook hotkey, but allows us to keep `Q` and `E` available for leaning.

## Additional hotkeys

* `Ctrl+G` cycles between graphics rendering modes
* `Ctrl+F` to enable full screen mode
* `Ctrl+D` to disable full screen mode 

