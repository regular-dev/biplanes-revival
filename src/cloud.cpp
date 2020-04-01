//    Biplanes Revival
//    Copyright (C) 2019-2020 Regular-dev community
//    http://regular-dev.org/
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


#include "include/variables.h"


Cloud::Cloud()
{
  x = 0.0f;
  y = 0.0f;
  dir = false;
  id = 0;
  opaque = false;
}


Cloud::Cloud( bool new_dir, unsigned char new_id )
{
  dir = new_dir;
  id = new_id;
  Respawn();
}


void Cloud::Update()
{
  UpdateCoordinates();
  UpdateCollisionBox();
}

void Cloud::UpdateCoordinates()
{
  const float move_dir  = dir == true ? 1.0f : -1.0f;

  const float speed_min = sizes.cloud_speed * 0.5f;
  const float speed_max = sizes.cloud_speed * 1.5f;
  const float speed_dif = speed_max - speed_min;

  x += ( speed_min + speed_dif * id / clouds.size() ) * move_dir * deltaTime;

  if ( dir )
  {
    if ( x - sizes.cloud_sizex / 2 > sizes.screen_width )
    {
      x = -sizes.cloud_sizex / 2;
      UpdateHeight();
    }
  }
  else
  {
    if ( x + sizes.cloud_sizex / 2 < 0.0f )
    {
      x = sizes.screen_width + sizes.cloud_sizex / 2;
      UpdateHeight();
    }
  }
}

void Cloud::UpdateHeight()
{
  float cloud_height = sizes.cloud_lowest_y - sizes.cloud_highest_y;
  if ( dir )
  {
    y += sizes.cloud_sizex / 2;
    if ( y > sizes.cloud_lowest_y )
      y = sizes.cloud_highest_y + id * cloud_height / clouds.size();
  }
  else
  {
    y -= sizes.cloud_sizex / 2;
    if ( y < sizes.cloud_highest_y )
      y = sizes.cloud_lowest_y - id * cloud_height / clouds.size();
  }
}

void Cloud::UpdateCollisionBox()
{
  collision_box.x = x - sizes.cloud_sizex / 2;
  collision_box.y = y - sizes.cloud_sizey / 2;
  collision_box.w = sizes.cloud_sizex;
  collision_box.h = sizes.cloud_sizey;
}

bool Cloud::isHit( float check_x, float check_y )
{
  return (  check_x > collision_box.x &&
            check_x < collision_box.x + collision_box.w &&
            check_y > collision_box.y &&
            check_y < collision_box.y + collision_box.h );
}

void Cloud::Draw()
{
  if ( HARDCORE_MODE || ( !HARDCORE_MODE && id < 2 ) )
  {
    textures.destrect.x = x - sizes.cloud_sizex / 2;
    textures.destrect.y = y - sizes.cloud_sizey / 2;
    textures.destrect.w = sizes.cloud_sizex;
    textures.destrect.h = sizes.cloud_sizey;

    if ( opaque )
      SDL_RenderCopy( gRenderer,
                      textures.texture_cloud_opaque,
                      NULL,
                      &textures.destrect );
    else
      SDL_RenderCopy( gRenderer,
                      textures.texture_cloud,
                      NULL,
                      &textures.destrect );

    if ( show_hitboxes )
    {
      SDL_SetRenderDrawColor( gRenderer, 255, 0, 0, 1 );
      SDL_RenderDrawRect( gRenderer, &collision_box );
    }
  }
}

void Cloud::setTransparent()
{
  opaque = false;
}

void Cloud::setOpaque()
{
  opaque = true;
}

void Cloud::Respawn()
{
  if ( dir )
    x = sizes.cloud_right_spawn_x + ( clouds.size() - id ) * sizes.cloud_sizey;
  else
    x = sizes.cloud_left_spawn_x + ( clouds.size() - id ) * sizes.cloud_sizey;

  if ( id % 2 )
    y = sizes.cloud_highest_y + ( clouds.size() + id ) / (float) clouds.size() * sizes.cloud_sizey;
  else
    y = sizes.cloud_lowest_y - ( clouds.size() + id ) / (float) clouds.size() * sizes.cloud_sizey;
  opaque = true;
}
