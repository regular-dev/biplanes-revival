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

#include <include/fwd.hpp>

#include <SDL.h>
#include <SDL_image.h>
#include <SDL_mixer.h>

#include <string>


extern int DISPLAY_INDEX;

extern SDL_Window* gWindow;
extern SDL_Renderer* gRenderer;
extern SDL_Event windowEvent;


bool SDL_init( const bool enableVSync, const bool enableSound );
void SDL_close();

void show_warning(
  const char*,
  const char* );

void setVSync( const bool enabled );

SDL_Texture* loadTexture( const std::string& );
Mix_Chunk* loadSound( const std::string& );

int playSound(
  Mix_Chunk* sound,
  const int channel = -1 );

int loopSound(
  Mix_Chunk* sound,
  const int channel );

void panSound(
  const int channel,
  const float pan );

int stopSound( const int channel );

void setSoundVolume( const float normalizedVolume );


void setRenderColor( const Color& );
void queryWindowSize();
void recalculateVirtualScreen();

SDL_FPoint toWindowSpace( const SDL_FPoint& );
float toWindowSpaceX( const float );
float toWindowSpaceY( const float );

SDL_FPoint scaleToScreen( const SDL_FPoint& );
float scaleToScreenX( const float );
float scaleToScreenY( const float );
