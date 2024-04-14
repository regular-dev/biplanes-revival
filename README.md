# Biplanes Revival

[![Windows (MSVC)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msvc.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msvc.yml)
[![Windows (MSYS2)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msys2.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msys2.yml)
[![Ubuntu](https://github.com/regular-dev/biplanes-revival/actions/workflows/ubuntu-build.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/ubuntu-build.yml)
[![macOS](https://github.com/regular-dev/biplanes-revival/actions/workflows/macos-build.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/macos-build.yml)
[![GitHub Releases](https://img.shields.io/github/release/regular-dev/BiplanesRevival.svg)](https://github.com/regular-dev/biplanes-revival/releases/latest)

An old cellphone arcade "Bluetooth Biplanes"
recreated for PC
(originally developed by Morpheme Ltd. in 2004)


## Gameplay videos:

[Singleplayer gameplay](https://github.com/regular-dev/biplanes-revival/assets/67646403/4f7d6371-6c9f-4271-a6c7-d17902a5ed2f)

- [Singleplayer gameplay pt 1](https://youtu.be/FYtIZ7ptaSo)
- [Singleplayer gameplay pt 2](https://youtu.be/4pWHn85Ez0o)
- [Multiplayer gameplay (on legacy version)](https://youtu.be/mIgMNh6gGXs)


### Windows binaries & Linux AppImage are available for download:
- On [our website](https://regular-dev.org/biplanes-revival)
- On this repository's [Releases page](https://github.com/regular-dev/biplanes-revival/releases)
- On our [itch.io page](https://regular-dev.itch.io/biplanes-revival)

## Features:

  - Easy to learn, hard to master gameplay
  - Immerse yourself in quick and intense dogfights
  - Shoot & dodge, bail out & respawn to outsmart your rival
  - Challenging AI with 4 difficulty levels
  - Easy peer-to-peer matchmaking with private sessions support
  - Optional gameplay modifiers:
    - one-shot kills
    - extra clouds for cover
    - alternative hitboxes for challenge
  - Verbose statistics system
  - Moddable sounds & sprites

Written in C++ from scratch
using reworked sprites
from the original game.

Graphics: SDL2

Netcode based on [Simple Network Library
from "Networking for Game Programmers" by Glenn Fiedler](http://www.gaffer.org/networking-for-game-programmers)
(author moved to other domain
and reworked his site,
so the link is now broken)


## Credits:

Linux port, matchmaking and AI would never come to life without [xion's](https://github.com/xxxxxion) help.
Kudos to this person!

Big thanks to [punchingdig](https://www.youtube.com/user/punchingdig) for upscaling the splash screen,
making custom soundpacks and redrawing some of the sprites & frames.
Not to mention countless hours of patient playtesting...


## Building:

### Ubuntu/Debian-based:

```bash
sudo apt install git g++ cmake
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
git clone --recurse-submodules https://github.com/regular-dev/biplanes-revival
cd biplanes-revival
cmake .
cmake --build .
```

### Arch Linux:

```bash
sudo pacman -S git gcc cmake
sudo pacman -S sdl2 sdl2_image sdl2_mixer
git clone --recurse-submodules https://github.com/regular-dev/biplanes-revival
cd biplanes-revival
cmake .
cmake --build .
```

### macOS (Sierra 10.12 and newer):

[macOS workflow](https://github.com/regular-dev/biplanes-revival/actions/workflows/macos-build.yml) builds fine, but we don't have any
MacBooks for runtime testing unfortunately,
so feel free to report any issues (or successes)

```bash
brew install git gcc cmake
brew install sdl2 sdl2_image sdl2_mixer
git clone --recurse-submodules https://github.com/regular-dev/biplanes-revival
cd biplanes-revival
cmake .
cmake --build .
```

### Windows (MSVC)

You'll need to help CMake find SDL2 development libraries.
It's best to follow [MSVC workflow steps](https://github.com/regular-dev/biplanes-revival/blob/master/.github/workflows/windows-build-msvc.yml#L62-L64) for reference

### Windows (MSYS2 mingw32 environment):

```bash
sudo pacman -S git
sudo pacman -S mingw-w64-i686-cmake mingw-w64-i686-gcc mingw-w64-i686-sdl2 mingw-w64-i686-sdl2_image mingw-w64-i686-sdl2_mixer
git clone --recurse-submodules https://github.com/regular-dev/biplanes-revival
cd biplanes-revival
cmake .
cmake --build .
```

For sharing static Windows builds with other people
you should provide the following win32 releases of SDL2 as well:

- [SDL2.dll](https://github.com/libsdl-org/SDL/releases)
- [SDL2_image.dll](https://github.com/libsdl-org/SDL_image/releases)
- [SDL2_mixer.dll](https://github.com/libsdl-org/SDL_mixer/releases)

