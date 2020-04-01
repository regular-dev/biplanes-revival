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


#include <string.h>

#include "include/render.h"
#include "include/variables.h"


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
  SDL_RenderCopy( gRenderer, textures.texture_background, NULL, &textures.destrect );

  if ( show_hitboxes )
  {
    SDL_SetRenderDrawColor( gRenderer, 255, 255, 0, 1 );
    SDL_RenderDrawLine( gRenderer,  0,
                                    sizes.ground_y_collision,
                                    sizes.screen_width,
                                    sizes.ground_y_collision );
  }
}


// Draw barn
void draw_barn()
{
  textures.destrect.x = int(sizes.screen_width * 0.5f - sizes.barn_sizex * 0.5);
  textures.destrect.y = int(sizes.screen_height * 0.808);
  textures.destrect.w = sizes.barn_sizex;
  textures.destrect.h = sizes.barn_sizey;
  SDL_RenderCopy( gRenderer, textures.texture_barn, NULL, &textures.destrect );

  if ( show_hitboxes )
  {
    SDL_Rect barn_hit_box;
    barn_hit_box.x = sizes.barn_x_bullet_collision;
    barn_hit_box.y = sizes.barn_y_bullet_collision;
    barn_hit_box.w = sizes.barn_sizex * 0.95f;
    barn_hit_box.h = sizes.barn_sizey;

    SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &barn_hit_box );

    barn_hit_box.x = sizes.barn_x_collision;
    barn_hit_box.y = sizes.barn_y_collision;

    SDL_SetRenderDrawColor( gRenderer, 255, 255, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &barn_hit_box );

    barn_hit_box.y = sizes.screen_height * 0.908;
    barn_hit_box.x = sizes.barn_x_pilot_left_collision;
    barn_hit_box.w = sizes.barn_x_pilot_right_collision - sizes.barn_x_pilot_left_collision;

    SDL_SetRenderDrawColor( gRenderer, 0, 255, 0, 1 );
    SDL_RenderDrawRect( gRenderer, &barn_hit_box );
  }
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
