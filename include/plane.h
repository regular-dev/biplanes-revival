#ifndef STRUCTURES_H
#define STRUCTURES_H

#include "utility.h"

struct Plane_Data
{
  float x;
  float y;
  float dir;

  float pilot_x;
  float pilot_y;
};

class Plane
{
  bool type;
  unsigned char score;

  float x;
  float y;
  float dir;

  float speed;
  float max_speed_var;

  bool fire;
  Timer *fire_cooldown;
  Timer *pitch_cooldown;

  unsigned char hp;
  bool dead;
  Timer *dead_cooldown;

  bool onground;
  bool takeoff;
  bool jump;

  char smk_frame[5];
  SDL_Rect smk_destrect[5];

  Timer *smk_anim;
  Timer *smk_period;
  char smk_rect;

  char fire_frame;
  Timer *fire_anim;

  char expl_frame;
  Timer *expl_anim;

  SDL_Rect hitbox;

public:
  Plane( bool );
  ~Plane();
  void InitTimers();
  void Update();

  void Accelerate();
  void Decelerate();
  void Turn( unsigned char );
  void Shoot();
  void Jump();

  void SpeedUpdate();
  void CoordinatesUpdate();
  void CollisionsUpdate();
  void TakeOffUpdate();
  void AbandonedUpdate();
  void AnimationsUpdate();
  void AnimationsReset();
  void SmokeUpdate();
  void FireUpdate();
  void ExplosionUpdate();
  void HitboxUpdate();

  void TakeOffStart();
  void TakeOffFinish();
  void Hit( bool );
  void Explode();
  void Crash();
  void Respawn();
  void ResetScore();

  void ScoreChange( char );
  bool isHit( float, float );
  Plane_Data getData();

  unsigned char getType();
  unsigned char getScore();
  float getX();
  float getY();
  void setCoords( Plane_Data );
  void setDir( float );
  bool isDead();

  class Input
  {
    Plane *plane;

  public:
    Input( Plane* );
    void Accelerate();
    void Decelerate();
    void TurnLeft();
    void TurnRight();
    void TurnIdle();
    void Shoot();
    void Jump();
  };

  class Pilot
  {
  public:
    Pilot( Plane* );
    ~Pilot();
    void InitTimers();
    void Update();

    void Move( unsigned char );
    void MoveIdle();
    void OpenChute();
    void ChuteUnlock();

    void FallUpdate();
    void RunUpdate();
    void DeathUpdate();
    void AnimationsUpdate();
    void AnimationsReset();
    void FallAnimUpdate();
    void ChuteAnimUpdate();
    void RunAnimUpdate();
    void DeathAnimUpdate();
    void HitboxUpdate();
    void ChuteHitboxUpdate();

    void Bail( float, float, float );
    void ChuteHit();
    void Death();
    void Kill( bool );
    void HitGroundCheck();
    void FallSurvive();
    void Rescue();
    void Respawn();

    bool isDead();
    bool isHit( float, float );
    bool ChuteisHit( float, float );
    float getX();
    float getY();

    void setX( float );
    void setY( float );

  private:
    Plane *plane;
    bool run;
    bool chute;
    bool dead;

    float x;
    float y;
    short dir;

    float speed;
    float vspeed;
    float gravity;

    char fall_frame;
    Timer *fall_anim;

    char chute_state;
    Timer *chute_anim;

    unsigned char run_frame;
    Timer *run_anim;

    char angel_frame;
    char angel_loop;
    Timer *angel_anim;

    SDL_Rect pilot_hitbox;
    SDL_Rect chute_hitbox;
  };

  Input *input;
  Pilot *pilot;
};


#endif // PLANE_H
