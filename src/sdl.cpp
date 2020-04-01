//    Biplanes Revival
//    Copyright (C) 2019-2020 Regular-dev community
//    https://regular-dev.org/
//    regular.dev.org@gmail.com
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.


#include "include/sdl.h"
#include "include/utility.h"
#include "include/variables.h"


int SCREEN_HEIGHT;
int SCREEN_WIDTH;
int DISPLAY_INDEX = 0;


SDL_Window *gWindow = NULL;
SDL_Renderer *gRenderer = NULL;


bool SDL_init()
{
	// Setup SDL
  log_message( "SDL Startup: Initializing SDL..." );

	if ( SDL_InitSubSystem( SDL_INIT_VIDEO ) != 0 )
  {
    log_message( "\nSDL Startup: SDL video system failed to initialize! SDL Error: ", SDL_GetError() );
    show_warning( "SDL: Failed to initialize!", SDL_GetError() );
    return 1;
  }
  log_message( "Done!\n" );


  // Set texture filtering to linear
  if ( !SDL_SetHint( SDL_HINT_RENDER_SCALE_QUALITY, "0" ) )
    log_message( "\nWarning: Linear texture filtering failed to enable!\n" );


  // Disable Always-on-Top
  if ( !SDL_SetHint( SDL_HINT_ALLOW_TOPMOST, "0" ) )
    log_message( "Warning: Always-on-Top not disabled!\n" );

  // Get screen resolution
  SDL_DisplayMode dm;
  SDL_GetDesktopDisplayMode( DISPLAY_INDEX, &dm );
  SCREEN_HEIGHT = dm.h;
  SCREEN_WIDTH = dm.w;

  sizes.screen_height = SCREEN_HEIGHT * 0.75f;
  sizes.screen_width = sizes.screen_height * 1.23f;


  //Create window
  log_message( "SDL Startup: Creating SDL window..." );
  gWindow = SDL_CreateWindow( "Bluetooth Biplanes", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, sizes.screen_width, sizes.screen_height, SDL_WINDOW_RESIZABLE );
  if ( gWindow == NULL )
  {
    log_message( "\nSDL Startup: Window could not be created! SDL Error: ", SDL_GetError() );
    show_warning( "SDL: Window could not be created!", SDL_GetError() );
    return 1;
  }
  log_message( "Done!\n" );

  SDL_SetWindowPosition( gWindow, SCREEN_WIDTH * 0.5f - sizes.screen_width * 0.5f, SCREEN_HEIGHT * 0.1 );
  SDL_SetWindowMinimumSize( gWindow, SCREEN_HEIGHT * 1.23 * 0.2, SCREEN_HEIGHT * 0.2 );
  SDL_SetWindowMaximumSize( gWindow, SCREEN_HEIGHT * 1.23, SCREEN_HEIGHT );


  //Create renderer for window
  log_message( "SDL Startup: Creating SDL renderer for window..." );
  gRenderer = SDL_CreateRenderer( gWindow, DISPLAY_INDEX, SDL_RENDERER_ACCELERATED );
  if ( gRenderer == NULL )
  {
    log_message( "\nSDL Startup: Failed to create renderer in accelerated mode! SDL Error: ", SDL_GetError(), "\n\nCreating SDL renderer in software mode..." );
    gRenderer = SDL_CreateRenderer( gWindow, 0, SDL_RENDERER_SOFTWARE );
    if ( gRenderer == NULL )
    {
      log_message( "\nSDL Startup: Renderer in software mode could not be created! SDL Error: ", SDL_GetError() );
      show_warning( "SDL: Failed to create renderer in software mode!", SDL_GetError() );
      return 1;
    }
  }
  log_message( "Done!\n" );


  // Initialize renderer color
  SDL_SetRenderDrawColor( gRenderer, 0x00, 0x00, 0x00, 0x00 );


  // Initialize PNG loading
  log_message( "SDL Startup: Initializing texture loading..." );
  int imgFlags = IMG_INIT_PNG;
  if ( !( IMG_Init( imgFlags ) & imgFlags ) )
  {
    log_message( "\nSDL Startup: SDL_image could not initialize! SDL_image Error: ", IMG_GetError() );
    show_warning( "SDL_image: Can't initialize!", IMG_GetError() );
    return 1;
  }
  log_message( "Done!\n" );


  // Initialize SDL_Mixer
  log_message( "SDL Startup: Initializing audio..." );
  if ( SDL_InitSubSystem( SDL_INIT_AUDIO ) != 0 )
  {
    log_message( "\nSDL Startup: SDL audio system failed to initialize! SDL Error: ", SDL_GetError() );
    show_warning( "SDL: Failed to initialize audio system!", SDL_GetError() );
    return 1;
  }
  if ( Mix_OpenAudio( 44100, MIX_DEFAULT_FORMAT, 2, 2048 ) != 0 )
    log_message( "\nSDL Startup: SDL_Mixer could not initialize! SDL_Mixer Error: %s", IMG_GetError() );
  else
  {
    Mix_ReserveChannels( 2 );
    sound_initialized = true;
  }
  log_message( "Done!\n" );


  log_message( "\n\nSDL Startup: SDL startup finished!\n\n" );
	return 0;
}


// Show warning
void show_warning( const char *title, const char *message )
{
  if ( SDL_ShowSimpleMessageBox( SDL_MESSAGEBOX_WARNING, title, message , NULL ) < 0 )
    log_message( "SDL Error: Unable to show warning window : ", SDL_GetError() );
}


void SDL_close()
{
	//Destroy window
	log_message( "EXIT: Destroying SDL renderer..." );
	SDL_DestroyRenderer( gRenderer );
	log_message( "Done!\n" );
	log_message( "EXIT: Destroying SDL window..." );
	SDL_DestroyWindow( gWindow );
	log_message( "Done!\n" );
	gWindow = NULL;
	gRenderer = NULL;


	//Quit SDL subsystems
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


// Load texture from file
SDL_Texture *loadTexture( std::string path )
{
	//The final texture
	SDL_Texture *newTexture = NULL;


	//Load image at specified path
	SDL_Surface *loadedSurface = IMG_Load( path.c_str() );
	if( loadedSurface == NULL )
	{
    log_message( "\n\nResources: Unable to load image ( ", path.c_str(), " )\nSDL_image Error: ", IMG_GetError() );
    show_warning( "Unable to load texture!", path.c_str() );
	}
	else
	{
		//Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( gRenderer, loadedSurface );
		if( newTexture == NULL )
		{
      log_message( "\n\nSDL Error: Unable to create texture from file: (", path.c_str(), ")\nSDL_image Error: ", SDL_GetError() );
        show_warning( "Unable to create texture from file!", path.c_str() );
		}


		//Get rid of old loaded surface
		SDL_FreeSurface( loadedSurface );
	}

	return newTexture;
}


// Load sound from file
Mix_Chunk *loadSound( std::string path )
{
  Mix_Chunk *Soundbuf = Mix_LoadWAV( path.c_str() );
  if ( Soundbuf == NULL )
  {
    log_message( "\n\nResources: Unable to load sound from file: ( ", path.c_str(), " )\nSDL_mixer Error: ", Mix_GetError() );
    show_warning( "Unable to load sound!", path.c_str() );
  }
  return Soundbuf;
}


void playSound( Mix_Chunk *sound, unsigned char channel, bool repeating )
{
  if ( !sound_initialized )
    return;

  if ( repeating )
  {
    if ( !Mix_Playing( channel ) )
        Mix_PlayChannel( channel, sound, 0 );
  }
  else
    Mix_PlayChannel( -1, sound, 0 );
}
