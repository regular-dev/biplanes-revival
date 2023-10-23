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

[Gameplay video](https://youtu.be/mIgMNh6gGXs)

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


### Windows binaries & AppImage for Linux are available at [our website](https://regular-dev.org/biplanes-revival)


## Building:

You'll need SDL2 to build this project. Make sure your SDL2 version is 2.0.10 or newer.
Older versions have Windows-specific [bug](https://discourse.libsdl.org/t/sdl2-lag-with-sdl-getticks/25538) which may cause movement lag.

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

For sharing static builds with other people 
you should provide the following SDL2 win32 releases as well:

    [SDL2.dll](https://github.com/libsdl-org/SDL/releases)
    [SDL2_image.dll](https://github.com/libsdl-org/SDL_image/releases)
    [SDL2_mixer.dll](https://github.com/libsdl-org/SDL_mixer/releases)

