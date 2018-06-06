Shockolate - System Shock, but cross platform!
============================
Based on the source code for PowerPC released by Night Dive Studios, Incorporated.

GENERAL NOTES
=============

A cross platform source port of the System Shock source code that was released, using SDL2. This runs well on OSX and Linux right now, with some runtime issues to sort out.

The end goal for this project is something like what Chocolate Doom is for Doom: an experience that closely mimics the original, but portable and with some quality of life improvements - and mod support!

![work so far](https://i.imgur.com/kbVWQj4.gif)

# Working so far:
- Gameplay!
- Loads original resource files
- Starting up an SDL drawing context, including palette
- SDL input
- Bitmap rendering
- Level loading
- Starting a new game
- HUD rendering
- 3D rendering
- AI
- Physics
- Cyberspace
- Saving / Loading

# Not working:
- Sound & Music
  - SDL_Mixer should be able to play the VOC and MIDI files used for sfx and music
- Main Menu
  - the original main menu should be revived, instead of the Mac version
- Video Files
  - Need to revive the old movie rendering code in AFile
- Stability
  - Leaks memory, crashes often

Compiling / Running
============

# Prerequisites
  - SDL2, 32 bit
  - Original assets in a `res/data` folder next to the executable

# Build and run
```
cmake .
make systemshock
./systemshock
```
