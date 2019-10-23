#include <string.h>

#include "../include/render.h"
#include "../include/variables.h"


// Render text
void draw_text( const char text[], int x, int y )
{
  for ( unsigned int i = 0; i < strlen(text); i++ )
  {
    textures.destrect.x = x + sizes.text_sizex * i;
    textures.destrect.y = y;
    textures.destrect.w = sizes.text_sizex;
    textures.destrect.h = sizes.text_sizey;
    SDL_RenderCopy( gRenderer, textures.main_font, &textures.font_rect[ text[i] - 32 ], &textures.destrect  );
  }
}


// Render menu button
void draw_menu_button()
{
  SDL_Rect srcrect = { sizes.button_x, 0, 127, 12 };
  textures.destrect.x = sizes.screen_width * 0.008;
  textures.destrect.y = sizes.screen_height * 0.34755 + menu.getSelectedButton() * sizes.button_sizey;
  textures.destrect.w = sizes.button_sizex;
  textures.destrect.h = sizes.button_sizey;
  SDL_RenderCopy( gRenderer, textures.menu_moving_button, &srcrect, &textures.destrect );
}


// Draw background
void draw_background()
{
  // Clear buffer
  SDL_RenderClear( gRenderer );


  // Draw background
  textures.destrect = { 0, 0, sizes.screen_width, sizes.screen_height };
  SDL_RenderCopy( gRenderer, textures.texture_background, NULL, &textures.destrect);
}


// Draw barn
void draw_barn()
{
  textures.destrect = { int(sizes.screen_width * 0.426), int(sizes.screen_height * 0.808), sizes.barn_sizex, sizes.barn_sizey };
  SDL_RenderCopy( gRenderer, textures.texture_barn, NULL, &textures.destrect );
}



// Draw score
void draw_score()
{
  char text [5];
  sprintf( text, "%d-%d", (int) plane_blue.getScore(), (int) plane_red.getScore() );
  draw_text ( text, sizes.screen_width * 0.45, sizes.screen_height * 0.5 );
}


// Update screen
void display_update()
{
  SDL_RenderPresent( gRenderer );
  //SDL_UpdateWindowSurface( gWindow );
}
