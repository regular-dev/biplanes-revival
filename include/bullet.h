#ifndef BULLET_H
#define BULLET_H

#include "utility.h"
#include <vector>

class Bullet
{
  bool alive;
  float x;
  float y;
  float dir;
  bool fired_by;
  char hit_frame;
  Timer *hit_anim;
  SDL_Rect hit_destrect;

public:
  Bullet( float, float, float, bool );
  bool isDead();

  void Update();
  void UpdateCoordinates();
  void AnimUpdate();
  void HitAnimUpdate();
  void HitGround();
  void Draw();
  void Destroy();
};


class BulletSpawner
{
  std::vector <Bullet> instances;

public:
  void SpawnBullet( float, float, float, bool );
  void Clear();
  void UpdateBullets();
  void Draw();
};


#endif //BULLET_H
