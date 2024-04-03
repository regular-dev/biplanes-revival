/*
  Biplanes Revival
  Copyright (C) 2019-2023 Regular-dev community
  https://regular-dev.org
  regular.dev.org@gmail.com

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include <include/sdl.hpp>
#include <include/canvas.hpp>
#include <include/constants.hpp>
#include <include/game_state.hpp>
#include <include/sounds.hpp>
#include <include/utility.hpp>

#include <cmath>


int32_t DISPLAY_INDEX {};

SDL_Window* gWindow {};
SDL_Renderer* gRenderer {};
SDL_Event windowEvent {};

static bool soundInitialized {};
static bool vsyncEnabled {};


bool
SDL_init(
  const bool enableVSync,
  const bool enableSound )
{
//  Setup SDL
  log_message( "SDL Startup: Initializing SDL..." );

  if ( SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0 )
  {
    log_message( "\nSDL Startup: SDL video subsystem failed to initialize! SDL Error: ", SDL_GetError(), "\n" );
    show_warning( "SDL: Failed to initialize!", SDL_GetError() );

    return 1;
  }

  log_message( "Done!\n" );


//  Set texture filtering to linear
  if ( SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) == false )
  {
    log_message( "\nWarning: Failed to enable nearest texture sampling! Graphics will be blurry.\n" );
    show_warning( "SDL: Failed to enable nearest texture sampling!", "Graphics will be blurry" );
  }


//  Get screen resolution
  SDL_DisplayMode dm {};
  SDL_GetDesktopDisplayMode( DISPLAY_INDEX, &dm );

  canvas.windowWidth = std::min(dm.w * 0.75f, dm.h * 0.75f);
  canvas.windowHeight = canvas.windowWidth / constants::aspectRatio;

  canvas.windowWidthNew = canvas.windowWidth;
  canvas.windowHeightNew = canvas.windowHeight;

  recalculateVirtualScreen();


//  Create window
  log_message( "SDL Startup: Creating SDL window..." );

  gWindow = SDL_CreateWindow(
    "Biplanes Revival v" BIPLANES_VERSION,
    SDL_WINDOWPOS_UNDEFINED,
    SDL_WINDOWPOS_UNDEFINED,
    canvas.windowWidth,
    canvas.windowHeight,
    SDL_WINDOW_RESIZABLE );

  if ( gWindow == nullptr )
  {
    log_message( "\nSDL Startup: Window could not be created! SDL Error: ", SDL_GetError() );
    show_warning( "SDL: Window could not be created!", SDL_GetError() );

    return 1;
  }

  SDL_SetWindowPosition(
    gWindow,
    dm.w * 0.5f - canvas.windowWidth * 0.5f,
    dm.h * 0.5f - canvas.windowHeight * 0.5f );

  SDL_SetWindowMinimumSize(
    gWindow,
    dm.w * 0.2f,
    dm.h * 0.2f );

  log_message( "Done!\n" );


//  Create renderer for window
  log_message( "SDL Startup: Creating SDL renderer for window..." );

  gRenderer = SDL_CreateRenderer(
    gWindow, -1,
    SDL_RENDERER_ACCELERATED );

  if ( gRenderer == nullptr )
  {
    log_message( "\nSDL Startup: Failed to create renderer in accelerated mode! SDL Error: ", SDL_GetError(), "\n\nCreating SDL renderer in software mode..." );

    gRenderer = SDL_CreateRenderer(
      gWindow, -1,
      SDL_RENDERER_SOFTWARE );

    if ( gRenderer == nullptr )
    {
      log_message( "\nSDL Startup: Renderer in software mode could not be created! SDL Error: ", SDL_GetError() );
      show_warning( "SDL: Failed to create renderer in software mode!", SDL_GetError() );

      return 1;
    }
  }
  log_message( "Done!\n" );

  setVSync(enableVSync);


//  Initialize renderer color
  SDL_SetRenderDrawColor( gRenderer, 0, 0, 0, 0 );


//  Initialize PNG loading

  log_message( "SDL Startup: Initializing texture loading..." );
  int imgFlags = IMG_INIT_PNG;

  if ( ( IMG_Init(imgFlags) & imgFlags ) == false )
  {
    log_message( "\nSDL Startup: SDL_image could not initialize! SDL_image Error: ", IMG_GetError(), "\n" );
    show_warning( "SDL_image: Can't initialize!", IMG_GetError() );

    return 1;
  }

  log_message( "Done!\n" );


  if ( enableSound == true )
  {
//    Initialize SDL_Mixer
    log_message( "SDL Startup: Initializing audio..." );

    if ( SDL_InitSubSystem( SDL_INIT_AUDIO ) != 0 )
    {
      log_message( "\nSDL Startup: SDL audio subsystem failed to initialize! SDL Error: ", SDL_GetError(), "\n" );
      show_warning( "SDL: Failed to initialize audio subsystem!", SDL_GetError() );
    }

    else if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) != 0 )
      log_message( "\nSDL Startup: SDL_Mixer failed to initialize! SDL_Mixer Error: %s", Mix_GetError(), "\n" );

    else
    {
      Mix_ReserveChannels(2);
      setSoundVolume(gameState().audioVolume);
      soundInitialized = true;
    }

    log_message( "Done!\n" );
  }


  log_message( "\n\nSDL Startup: SDL startup finished!\n\n" );

  return 0;
}


void
SDL_close()
{
//  Destroy window
  log_message( "EXIT: Destroying SDL renderer..." );
  SDL_DestroyRenderer( gRenderer );
  log_message( "Done!\n" );

  log_message( "EXIT: Destroying SDL window..." );
  SDL_DestroyWindow(gWindow);
  log_message( "Done!\n" );

  gWindow = {};
  gRenderer = {};


//  Quit SDL subsystems
  log_message( "EXIT: Closing audio..." );
  Mix_CloseAudio();
  log_message( "Done!\n" );

  log_message( "EXIT: Closing SDL mixer..." );
  Mix_Quit();
  log_message( "Done!\n" );

  log_message( "EXIT: Closing SDL image..." );
  IMG_Quit();
  log_message( "Done!\n" );

  log_message( "EXIT: Closing SDL..." );
  SDL_Quit();
  log_message( "Done!\n" );
}


void
show_warning(
  const char* title,
  const char* message )
{
  if ( SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_WARNING, title, message, nullptr ) < 0 )
    log_message( "SDL Error: Unable to show warning window : ", SDL_GetError(), "\n" );
}


void
setVSync(
  const bool enabled )
{
  if ( enabled == vsyncEnabled )
    return;

  vsyncEnabled = enabled;

  log_message("SDL: Setting V-Sync to " + std::to_string(enabled) + "\n");

  if ( SDL_SetHint( SDL_HINT_RENDER_VSYNC, std::to_string(enabled).c_str() ) == false )
    log_message( "Warning: Failed to set V-Sync!\n" );

  if ( SDL_RenderSetVSync(gRenderer, enabled) != 0 )
    log_message( "Warning: Failed to set renderer V-Sync!\n");
}


SDL_Texture*
loadTexture(
  const std::string& path )
{
//  The final texture
  SDL_Texture* loadedTexture {};


//  Load image at specified path
  SDL_Surface* textureSurface = IMG_Load( path.c_str() );

  if ( textureSurface == nullptr )
  {
    log_message( "\n\nResources: Unable to load image ( ", path.c_str(), " )\nSDL_image Error: ", IMG_GetError() );
    show_warning( "Unable to load texture!", path.c_str() );
  }
  else
  {
//    Create texture from surface pixels
    loadedTexture = SDL_CreateTextureFromSurface( gRenderer, textureSurface );

    if ( loadedTexture == nullptr )
    {
      log_message( "\n\nSDL Error: Unable to create texture from file: (", path.c_str(), ")\nSDL_image Error: ", SDL_GetError() );
      show_warning( "Unable to create texture from file!", path.c_str() );
    }

//    Get rid of old loaded surface
    SDL_FreeSurface( textureSurface );
  }

  return loadedTexture;
}

Mix_Chunk*
loadSound(
  const std::string& path )
{
  if ( soundInitialized == false )
    return {};

  Mix_Chunk* soundBuf = Mix_LoadWAV( path.c_str() );

  if ( soundBuf == nullptr )
  {
    log_message( "\n\nResources: Unable to load sound from file: ( ", path.c_str(), " )\nSDL_mixer Error: ", Mix_GetError() );
    show_warning( "Unable to load sound!", path.c_str() );
  }

  return soundBuf;
}

int
playSound(
  Mix_Chunk* sound,
  const int channel )
{
  if ( soundInitialized == false || sound == nullptr )
    return -1;


  if ( channel == -1 )
    return Mix_PlayChannel(-1, sound, 0);

  if ( Mix_Playing(channel) == false )
    return Mix_PlayChannel(channel, sound, 0);

  return -1;
}

int
loopSound(
  Mix_Chunk* sound,
  const int channel )
{
  if ( soundInitialized == false || sound == nullptr )
    return -1;


  if ( channel == -1 )
    return Mix_PlayChannel(-1, sound, 0);

  if ( Mix_Playing(channel) == false )
    return Mix_PlayChannel(channel, sound, 0);

  return channel;
}

void
panSound(
  const int channel,
  const float pan )
{
  if ( soundInitialized == false || channel == -1 )
    return;

  if ( Mix_Playing(channel) == false )
    return;

  const auto panDepth = gameState().stereoDepth;

  const uint8_t left = 255 - 255 * pan * panDepth;
  const uint8_t right = 255 - 255 * (1.0f - pan) * panDepth;

  Mix_SetPanning(channel, left, right);
}

int
stopSound(
  const int channel )
{
  if ( soundInitialized == false || channel == -1 )
    return -1;

  return Mix_HaltChannel(channel);
}

void
setSoundVolume(
  const float normalizedVolume )
{
  const double newVolume = std::pow(
    normalizedVolume, M_E ) * MIX_MAX_VOLUME;

#if SDL_MIXER_VERSION_ATLEAST(2, 6, 0)
  Mix_MasterVolume(newVolume);
#else
  Mix_HaltChannel(-1);
  Mix_Volume(-1, newVolume);
#endif
}


void
setRenderColor(
  const Color& color )
{
  SDL_SetRenderDrawColor(
    gRenderer,
    color.r,
    color.g,
    color.b,
    color.a );
}

void
queryWindowSize()
{
  if ( windowEvent.type != SDL_WINDOWEVENT )
    return;

  if (  windowEvent.window.event != SDL_WINDOWEVENT_RESIZED &&
        windowEvent.window.event != SDL_WINDOWEVENT_SIZE_CHANGED &&
        windowEvent.window.event != SDL_WINDOWEVENT_MOVED )
    return;


  SDL_GetRendererOutputSize(
    gRenderer,
    &canvas.windowWidthNew,
    &canvas.windowHeightNew );

  if (  canvas.windowWidthNew == canvas.windowWidth &&
        canvas.windowHeightNew == canvas.windowHeight &&
        SDL_GetWindowDisplayIndex(gWindow) == DISPLAY_INDEX )
    return;

  canvas.windowWidth = canvas.windowWidthNew;
  canvas.windowHeight = canvas.windowHeightNew;

  recalculateVirtualScreen();

  SDL_RenderClear(gRenderer);
}

void
recalculateVirtualScreen()
{
  const float ratio = std::min(
    canvas.windowWidth / constants::aspectRatio, (float) canvas.windowHeight );

  canvas.width = constants::aspectRatio * ratio;
  canvas.height = ratio;
  canvas.originX = (canvas.windowWidth - constants::aspectRatio * ratio) * 0.5f;
  canvas.originY = (canvas.windowHeight - ratio) * 0.5f;
}


SDL_FPoint
toWindowSpace(
  const SDL_FPoint& point )
{
  return
  {
    toWindowSpaceX(point.x),
    toWindowSpaceY(point.y),
  };
}

float
toWindowSpaceX(
  const float x )
{
  return canvas.originX + scaleToScreenX(x);
}

float
toWindowSpaceY(
  const float y )
{
  return canvas.originY + scaleToScreenY(y);
}

SDL_FPoint
scaleToScreen(
  const SDL_FPoint& point )
{
  return
  {
    scaleToScreenX(point.x),
    scaleToScreenY(point.y),
  };
}

float
scaleToScreenX(
  const float x )
{
  return x * canvas.width;
}

float
scaleToScreenY(
  const float y )
{
  return y * canvas.height;
}
