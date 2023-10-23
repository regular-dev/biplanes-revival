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

#pragma once

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>


extern int SCREEN_HEIGHT;
extern int SCREEN_WIDTH;
extern int DISPLAY_INDEX;

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern SDL_Event windowEvent;


bool SDL_init();
void SDL_close();

void show_warning(
  const char*,
  const char* );

SDL_Texture* loadTexture( const std::string& );
Mix_Chunk* loadSound( const std::string& );

void playSound(
  Mix_Chunk* sound,
  const uint8_t channel,
  const bool repeating );
