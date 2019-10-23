#include <math.h>

#include "../include/variables.h"


void BulletSpawner::SpawnBullet( float plane_x, float plane_y, float plane_dir, bool plane_type )
{
  instances.push_back( Bullet( plane_x, plane_y, plane_dir, plane_type ) );
}

void BulletSpawner::UpdateBullets()
{
  for ( unsigned int i = 0; i < instances.size(); i++ )
  {
    if ( instances[i].isDead() )
    {
      instances[i].Destroy();
      instances.erase( instances.begin() + i );
    }
    else
      instances[i].Update();
  }
}

void BulletSpawner::Clear()
{
  instances.clear();
}

void BulletSpawner::Draw()
{
  for ( Bullet &bullet : instances )
    bullet.Draw();
}


Bullet::Bullet( float plane_x, float plane_y, float plane_dir, bool plane_type )
{
  alive = true;
  x = plane_x;
  y = plane_y;
  dir = plane_dir;
  fired_by = plane_type;
  hit_frame = 0;
  hit_anim = new Timer( sizes.bullet_hit_frame_time );
  hit_destrect = {};
}

void Bullet::Update()
{
  UpdateCoordinates();
  HitAnimUpdate();
  hit_anim->Update();
}

void Bullet::UpdateCoordinates()
{
  if ( !alive )
    return;

  x += sizes.bullet_speed * sin( dir * PI / 180.0f ) * deltaTime;
  y -= sizes.bullet_speed * cos( dir * PI / 180.0f ) * deltaTime;

  if (  x > sizes.screen_width ||
        x < 0.0f ||
        y < 0.0f )
    Destroy();
  else if ( ( x > sizes.barn_x_bullet_collision &&
              x < sizes.barn_x_bullet_collision + sizes.barn_sizex &&
              y > sizes.barn_y_bullet_collision ) ||
              y > sizes.bullet_ground_collision )
    HitGround();
  else if ( opponent_connected )
  {
    if ( fired_by == (int) srv_or_cli )
      return;

    if ( srv_or_cli == SRV_CLI::SERVER )
    {
      // HIT PLANE
      if ( plane_blue.isHit( x, y ) )
      {
        Destroy();
        plane_blue.Hit( fired_by );
        local_data.hit = HIT_STATE::HIT_PLANE;
      }
      // HIT CHUTE
      if ( plane_blue.pilot->ChuteisHit( x, y ) )
      {
        Destroy();
        plane_blue.pilot->ChuteHit();
        local_data.hit = HIT_STATE::HIT_CHUTE;
      }
      // HIT PILOT
      if ( plane_blue.pilot->isHit( x, y ) )
      {
        Destroy();
        plane_blue.pilot->Kill( fired_by );
        local_data.hit = HIT_STATE::HIT_PILOT;
      }
    }
    else
    {
      // HIT PLANE
      if ( plane_red.isHit( x, y ) )
      {
        Destroy();
        plane_red.Hit( fired_by );
        local_data.hit = HIT_STATE::HIT_PLANE;
      }
      // HIT CHUTE
      if ( plane_red.pilot->ChuteisHit( x, y ) )
      {
        Destroy();
        plane_red.pilot->ChuteHit();
        local_data.hit = HIT_STATE::HIT_CHUTE;
      }
      // HIT PILOT
      if ( plane_red.pilot->isHit( x, y ) )
      {
        Destroy();
        plane_red.pilot->Kill( fired_by );
        local_data.hit = HIT_STATE::HIT_PILOT;
      }
    }
  }
}


bool Bullet::isDead()
{
  return ( !alive && hit_frame == 6 );
}

void Bullet::Draw()
{
  if ( hit_frame > 0 && hit_frame < 6 )
  {
    hit_destrect.x = x - sizes.hit_sizex / 2;
    hit_destrect.y = y - sizes.hit_sizey / 2;
    hit_destrect.w = sizes.hit_sizex;
    hit_destrect.h = sizes.hit_sizey;
    SDL_RenderCopy( gRenderer,
                    textures.anim_hit,
                    &textures.anim_hit_rect[ hit_frame - 1 ],
                    &hit_destrect );
  }
  else
  {
    if ( !alive )
      return;

    textures.destrect.x = x - sizes.bullet_sizex / 2;
    textures.destrect.y = y - sizes.bullet_sizey / 2;
    textures.destrect.w = sizes.bullet_sizex;
    textures.destrect.h = sizes.bullet_sizey;
    SDL_RenderCopy( gRenderer,
                    textures.texture_bullet,
                    NULL,
                    &textures.destrect );
  }
}

void Bullet::HitAnimUpdate()
{
  if ( alive )
    return;

  if ( hit_frame > 0 && hit_frame < 6 )
  {
    if ( hit_anim->isReady() )
    {
      hit_anim->Start();
      hit_frame++;
    }
  }
}

void Bullet::HitGround()
{
  playSound( sounds.hitMiss, -1, false );
  alive = false;
  hit_frame = 1;
}

void Bullet::Destroy()
{
  alive = false;
  hit_frame = 6;
}
