#include "../include/variables.h"


Cloud::Cloud()
{
  x = 0.0f;
  y = 0.0f;
  dir = false;
  opaque = false;
}


Cloud::Cloud( bool new_dir )
{
  dir = new_dir;
  Respawn();
}


void Cloud::Update()
{
  UpdateCoordinates();
  UpdateCollisionBox();
}

void Cloud::UpdateCoordinates()
{
  float move_dir = dir == true ? 1.0f : -1.0f;
  x += sizes.cloud_speed * move_dir * deltaTime;

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
  if ( dir )
  {
    y += sizes.cloud_sizex / 2;
    if ( y > sizes.cloud_lowest_y )
      y = sizes.cloud_highest_y;
  }
  else
  {
    y -= sizes.cloud_sizex / 2;
    if ( y < sizes.cloud_highest_y )
      y = sizes.cloud_lowest_y;
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
  {
    x = sizes.cloud_right_spawn_x;
    y = sizes.cloud_highest_y + ( sizes.cloud_lowest_y - sizes.cloud_highest_y ) * 0.3f;
  }
  else
  {
    x = sizes.cloud_left_spawn_x;
    y = sizes.cloud_highest_y + ( sizes.cloud_lowest_y - sizes.cloud_highest_y ) * 0.6f;
  }
  opaque = true;
}
