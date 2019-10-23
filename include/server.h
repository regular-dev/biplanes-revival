#ifndef SERVER_H
#define SERVER_H

struct Packet
{
  char pitch      = 0;
  char throttle   = 0;
  bool fire       = 0;
  bool jump       = 0;
  bool disconnect = false;

  float x         = 0.0f;
  float y         = 0.0f;
  float dir       = 0.0f;
  float pilot_x   = 0.0f;
  float pilot_y   = 0.0f;

  unsigned char hit        = 0;
//  none/hit_plane/hit_chute/hit_pilot
  unsigned char death      = 0;
//  none/plane_death/pilot_death/resp
  unsigned char confirm    = 0;
};


#endif //SERVER_H
