# Biplanes Revival

[![Windows (MSVC)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msvc.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msvc.yml)
[![Windows (MSYS2)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msys2.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/windows-build-msys2.yml)
[![Ubuntu](https://github.com/regular-dev/biplanes-revival/actions/workflows/ubuntu-build.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/ubuntu-build.yml)
[![macOS](https://github.com/regular-dev/biplanes-revival/actions/workflows/macos-build.yml/badge.svg)](https://github.com/regular-dev/biplanes-revival/actions/workflows/macos-build.yml)
[![GitHub Releases](https://img.shields.io/github/release/regular-dev/biplanes-revival.svg)](https://github.com/regular-dev/biplanes-revival/releases/latest)

<a href="https://regular-dev.itch.io/biplanes-revival">
<img width="150" alt="Available on itch.io" src="https://static.itch.io/images/badge-color.svg" />
</a>
<a href='https://flathub.org/apps/org.regular_dev.biplanes_revival'>
<img width='150' alt='Download on Flathub' src='https://flathub.org/api/badge?locale=en'/>
</a>

---

An old cellphone arcade "BlueTooth BiPlanes"
recreated for PC
(originally developed by Morpheme Ltd. in 2004)


## Gameplay videos:

[Singleplayer gameplay](https://github.com/regular-dev/biplanes-revival/assets/67646403/4f7d6371-6c9f-4271-a6c7-d17902a5ed2f)

- [Singleplayer gameplay pt 1](https://youtu.be/FYtIZ7ptaSo)
- [Singleplayer gameplay pt 2](https://youtu.be/4pWHn85Ez0o)
- [Multiplayer gameplay (legacy version)](https://youtu.be/mIgMNh6gGXs)


## Download the game:

- From [our website](https://regular-dev.org/biplanes-revival)
- From our [itch.io page](https://regular-dev.itch.io/biplanes-revival)
- From [Flathub](https://flathub.org/apps/org.regular_dev.biplanes_revival)
- From this repository's [Releases page](https://github.com/regular-dev/biplanes-revival/releases)

Custom soundpacks are available
on our [website](https://regular-dev.org/biplanes-revival) 
and on our [itch.io page](https://regular-dev.itch.io/biplanes-revival)


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

### If you're interested in history of this project, you can read our [devlog](https://regular-dev.itch.io/biplanes-revival/devlog/714967/5th-year-anniversary-update)


## Modding:

- Windows users can modify 
resources in the ```assets``` directory.

- AppImage users will have to 
download & extract [game assets](https://github.com/regular-dev/biplanes-revival/releases/download/v1.1/assets.zip) 
next to the executable, 
so the game will load resources 
from there instead. 

- Flatpak users will have to
download & extract [game assets](https://github.com/regular-dev/biplanes-revival/releases/download/v1.1/assets.zip) 
anywhere inside ```~/.var/app/org.regular_dev.biplanes_revival``` 
and run the game supplied with ***full*** path to parent directory 
of the extracted assets like so:
```flatpak run --env=BIPLANES_ASSETS_ROOT=/home/{YOUR_USERNAME}/.var/app/org.regular_dev.biplanes_revival org.regular_dev.biplanes_revival```


## Credits:

Linux port, matchmaking and AI would never come to life without [xion's](https://github.com/xxxxxion) help.
Kudos to this person!

Big thanks to [punchingdig](https://www.youtube.com/user/punchingdig) 
for his artistic help (upscaling the splash screen,
making custom soundpacks & cozy Christmas theme, redrawing some of the sprites & frames, etc.).
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

It's [confirmed](https://github.com/regular-dev/biplanes-revival/issues/2#issuecomment-2501199862) the game works on macOS 12.

```bash
brew install git gcc cmake
brew install jpeg-xl
brew install sdl2 sdl2_image sdl2_mixer
git clone --recurse-submodules https://github.com/regular-dev/biplanes-revival
cd biplanes-revival
cmake .
cmake --build .
```

### Windows (MSVC)

You'll need to help CMake find SDL2 development libraries.
It's best to follow [MSVC workflow steps](https://github.com/regular-dev/biplanes-revival/blob/master/.github/workflows/windows-build-msvc.yml#L63-L65) for reference

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

