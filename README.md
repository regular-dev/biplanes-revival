# Biplanes Revival

It's a PC recreation of an old cellphone game "Bluetooth Biplanes"
(originally developed by Morpheme Ltd. in 2004)

## Features:

  - Crossplatform
  - Easy matchmaking using peer-to-peer connection
  - Play with your friend using secret password
  - Easy to learn, hard to master gameplay
  - Hardcore mode: one-shot kills + more clouds for cover
  - Automated statistics system keeps track of your performance
  - Moddable sounds and sprites

[Gameplay video] (https://youtu.be/mIgMNh6gGXs)

Written in C++ from scratch using sprites from original game

Graphics: SDL2

Netcode based on [Simple Network Library
from "Networking for Game Programmers" by Glenn Fiedler](http://www.gaffer.org/networking-for-game-programmers)

(author moved to other domain and reworked his site so the link is now broken)

The main purpose of this project was
to recreate original feeling of game physics
and make playable multiplayer, so singleplayer features aren't included.

## Credits:

Splash screen upscaled by [punchingdig](https://www.youtube.com/user/punchingdig)


### Windows binaries & AppImage for Linux available at 
[our website](https://regular-dev.org/biplanes-revival)


## Building:

### Ubuntu/Debian-based:

```bash
sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
cd biplanes-revival
cmake .
make
```

### Arch Linux:

```bash
sudo pacman -S sdl2 sdl2_image sdl2_mixer
cd biplanes-revival
cmake .
make
```

### Mac (M1):

```bash
brew install sdl2 sdl2_image sdl2_mixer
cd biplanes-revival
cmake .
make
```

### Windows (MSYS2 mingw32 environment):

```bash
sudo pacman -S mingw-w64-i686-sdl2 mingw-w64-i686-sdl2_image mingw-w64-i686-sdl2_mixer
cd biplanes-revival
cmake .
make
```

### Windows (Code::Blocks + MinGW, the hard way): 


**1. Download SDL2 development libraries:**

 - [SDL2](https://www.libsdl.org/download-2.0.php)

 - [SDL2_image](https://www.libsdl.org/projects/SDL_image/)

 - [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)

If you already have SDL2 installed, make sure it's 2.0.10 or newer.
Older versions have Windows-specific [bug](https://discourse.libsdl.org/t/sdl2-lag-with-sdl-getticks/25538/) which may cause movement lag.


**2. Download and install [CMake](https://cmake.org/download/).**

**3. In CMake:**

  a. Specify paths to BiPlanes sources and build folder.
  b. Click "Configure"
  c. CMake will show error "Could NOT find SDL2"
  d. In SDL2_PATH variable specify path to your unpacked SDL2:
    for x86: SDL2-x.x.x/i686-w64-mingw32
    for x64: SDL2-x.x.x/x86_64-w64-mingw32
  e. Repeat steps 'b' to 'd' for SDL2_image and SDL2_mixer
  f. After configuring is finished, click "Generate"


**4. Open and compile generated Code::Blocks project in build folder.**

* It's recommended to use [up-to-date MinGW-w64](https://sourceforge.net/projects/mingw-w64/files/).*


**5. Copy these dlls from SDL2 bin directory next to game executable:**

    SDL2.dll
    SDL2_image.dll
    libpng16-16.dll
    zlib1.dll
    SDL2_mixer.dll
    libogg-0.dll
    libvorbis-0.dll
    libvorbisfile-3.dll


**6. Copy 'assets' directory next to executable in case it wasn't done automatically.**

**7. Launch game and try to defeat somebody!**

