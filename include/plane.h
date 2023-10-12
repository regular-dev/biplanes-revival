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

#include <include/utility.h>

#include <SDL.h>

#include <vector>


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
  bool type {};
  uint8_t score {};

  float x;
  float y;
  float dir;

  float speed;
  float max_speed_var;

  Timer fire_cooldown {0.0f};
  Timer pitch_cooldown {0.0f};

  unsigned char hp;
  bool dead;
  Timer dead_cooldown {0.0f};
  Timer protection {0.0f};

  bool onground;
  bool takeoff;
  bool jump;

  char smk_frame[5];
  SDL_Rect smk_destrect[5];

  Timer smk_anim {0.0f};
  Timer smk_period {0.0f};
  char smk_rect;

  char fire_frame;
  Timer fire_anim {0.0f};

  char expl_frame;
  Timer expl_anim {0.0f};

  SDL_Rect hitbox;

public:
  Plane( bool );
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
  void ResetSpawnProtection();
  void ResetScore();

  void ScoreChange( char );
  bool isHit( float, float );
  Plane_Data getData();
  void assignDataset( std::vector <float>& ) const;

  bool getType();
  unsigned char getScore();

//  Utility methods for dataset
  SDL_Point getClosestCollision() const;
  float getDistanceToPoint( const SDL_Point& ) const;
  float getAngleToPoint( const SDL_Point&, const SDL_Point& ) const;
  float getSpeedDir() const;
  float getSpeed() const;
  float getAngleRelative( const float, const float ) const;
//

  float getX();
  float getY();
  bool isDead() const;
  bool canShoot() const;
  bool canJump() const;

  void setCoords( Plane_Data );
  void setDir( float );


  class Input
  {
    Plane* plane;

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
    friend class Plane;

  public:
    Pilot( Plane* );
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

    bool isDead() const;
    bool isHit( float, float );
    bool ChuteisHit( float, float );
    float getX();
    float getY();

    void setX( float );
    void setY( float );

  private:
    Plane* plane;

    bool run {};
    bool chute {};
    bool dead {};

    float x {};
    float y {};
    int16_t dir {};

    float speed {};
    float vspeed {};
    float gravity {};

    int8_t fall_frame {};
    Timer fall_anim {0.0f};

    int8_t chute_state {};
    Timer chute_anim {0.0f};

    uint8_t run_frame {};
    Timer run_anim {0.0f};

    int8_t angel_frame {};
    int8_t angel_loop {};
    Timer angel_anim {0.0f};

    SDL_Rect pilot_hitbox;
    SDL_Rect chute_hitbox;
  };

  Input input {this};
  Pilot pilot {this};
};
