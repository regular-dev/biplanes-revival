# Biplanes Revival

It's a PC recreation of an old cellphone game "Bluetooth Biplanes"
(originally developed by Morpheme Ltd. in 2004)

Features:

  - Crossplatform
  - Easy matchmaking using peer-to-peer connection (currently down due to maintenance)
  - Play with your friend using secret password
  - Easy to learn, hard to master gameplay
  - Hardcore mode: one-shot kills + more clouds for cover
  - Automated statistics system keeps track of your performance

Gameplay video: https://www.youtube.com/watch?v=mIgMNh6gGXs

Written in C++ from scratch using sprites from original game

Graphics: SDL2

Netcode based on [Simple Network Library
from "Networking for Game Programmers" by Glenn Fiedler](http://www.gaffer.org/networking-for-game-programmers)

(author moved to other domain and reworked his site so the link is now broken)

The main purpose of this project was 
to recreate original feeling of game physics 
and make playable multiplayer, so singleplayer features aren't included.


### How to build on Linux : 

sudo apt install libsdl2-dev libsdl2-image-dev libsdl2-mixer-dev
cmake /path/to/sources
make
Assets folder should be placed next to executable


### Windows binaries available at our [website](https://regular-dev.org/biplanes-revival)



### How to build on Windows (Code::Blocks + MinGW ) : 


**1. Download SDL2 development libraries:**

 - [SDL2](https://www.libsdl.org/download-2.0.php)

 - [SDL2_image](https://www.libsdl.org/projects/SDL_image/)

 - [SDL2_mixer](https://www.libsdl.org/projects/SDL_mixer/)

If you already have SDL2, make sure it's 2.0.10 or newer.
Older versions have Windows-specific bug which may cause movement lag.

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

*Instead of compiling with default Code::Blocks MinGW compiler 
it's recommended to use newer 
[MinGW-w64](https://sourceforge.net/projects/mingw-w64/files/).*


**5. Copy these dlls from SDL2.../.../bin to game executable folder:**
    
    SDL2.dll
    SDL2_image.dll
    libpng16-16.dll
    zlib1.dll
    SDL2_mixer.dll

*If you want to launch game on different PC you also 
need to provide **libwinpthread-1.dll** from **mingw32/bin***


**6. Make sure that 'assets' folder is placed next to executable.**

**7. Launch game and try to defeat somebody!**
