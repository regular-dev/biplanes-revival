#ifndef CLOUD_H
#define CLOUD_H

#include "structures.h"

class Cloud
{
  float x;
  float y;
  bool dir;
  bool opaque;
  SDL_Rect collision_box;

public:
  Cloud();
  Cloud( bool );

  void Update();
  void UpdateCoordinates();
  void UpdateHeight();
  void UpdateCollisionBox();

  void Draw();
  void setTransparent();
  void setOpaque();
  void Respawn();

  bool isHit( float, float );
};


#endif //CLOUD_H
