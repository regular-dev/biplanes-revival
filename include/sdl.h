#ifndef SDL_H
#define SDL_H

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;
extern int DISPLAY_INDEX;

extern SDL_Window *gWindow;
extern SDL_Renderer *gRenderer;

bool SDL_init();
void SDL_close();
void show_warning( const char*, const char* );
SDL_Texture *loadTexture( std::string );
Mix_Chunk *loadSound( std::string );
void playSound( Mix_Chunk *sound, unsigned char channel, bool repeating );


#endif //SDL_H
