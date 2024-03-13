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
#include <include/enums.hpp>
#include <include/timer.hpp>
#include <include/stats.hpp>

#include <SDL_rect.h>

#include <array>
#include <vector>
#include <map>


class Plane
{
  PLANE_TYPE mType {};
  uint8_t mScore {};

  bool mIsLocal {};
  bool mIsBot {};

  float mX {};
  float mY {};
  float mDir {};

  float mSpeed {};
  float mMaxSpeedVar {};
  SDL_FPoint mSpeedVec {};

  Timer mPitchCooldown {0.0f};
  Timer mShootCooldown {0.0f};

  uint8_t mHp {};
  bool mIsDead {};
  Timer mDeadCooldown {0.0f};
  Timer mProtection {0.0f};

  bool mIsOnGround {};
  bool mIsTakingOff {};
  bool mHasJumped {};

  uint8_t mSmokeFrame {};
  Timer mSmokeAnim {0.0f};
  Timer mSmokeCooldown {0.0f};

  int8_t mFireFrame {};
  Timer mFireAnim {0.0f};

  SDL_FRect mHitbox {};


public:
  Plane( const PLANE_TYPE );


  void Accelerate();
  void Decelerate();
  void Turn( const PLANE_PITCH );
  void Shoot();
  void Jump();

  void Update();

  void Draw() const;
  void DrawFire() const;

  void DrawCollisionLayer() const;

  void SpeedUpdate();
  void CoordinatesUpdate();
  void CoordinatesWrap();
  void CollisionsUpdate();
  void TakeOffUpdate();
  void AbandonedUpdate();
  void AnimationsUpdate();
  void AnimationsReset();
  void SmokeUpdate();
  void FireUpdate();
  void HitboxUpdate();

  void TakeOffStart();
  void TakeOffFinish();

  void Hit( Plane& attacker );
  void Explode();
  void Crash();

  void Respawn();
  void ResetSpawnProtection();

  void ResetScore();
  void ScoreChange( const int8_t delta );

  void ResetStats();
  const Statistics& stats() const;

  PLANE_TYPE type() const;
  uint8_t score() const;
  uint8_t hp() const;
  float protectionRemainder() const;

  void setLocal( const bool );
  bool isLocal() const;

  void setBot( const bool );
  bool isBot() const;


  float x() const;
  float y() const;
  float dir() const;
  float jumpDir() const;

  float speed() const;
  float maxSpeed() const;
  SDL_FPoint speedVector() const;


  bool isHit( const float, const float ) const;
  bool isDead() const;
  bool hasJumped() const;

  bool isAirborne() const;
  bool isTakingOff() const;

  bool canAccelerate() const;
  bool canDecelerate() const;
  bool canTurn() const;

  bool canShoot() const;
  bool canJump() const;


  PlaneNetworkData getNetworkData() const;
  void setCoords( const PlaneNetworkData& );
  void setDir( const float );


  class Input
  {
    Plane* plane {};


  public:
    Input() = default;

    void setPlane( Plane* );


    void Accelerate();
    void Decelerate();
    void TurnLeft();
    void TurnRight();
    void TurnIdle();
    void Shoot();
    void Jump();

    void ExecuteAiAction( const AiAction );
  };

  class Pilot
  {
    friend class Plane;

    Plane* plane {};

    bool mIsRunning {};
    bool mIsChuteOpen {};
    bool mIsDead {};

    float mX {};
    float mY {};
    int16_t mDir {};

    SDL_FPoint mSpeed {};
    float mMoveSpeed {};
    float mGravity {};
    SDL_FPoint mSpeedVec {};

    int8_t mFallFrame {};
    Timer mFallAnim {0.0f};

    CHUTE_STATE mChuteState {CHUTE_NONE};
    Timer mChuteAnim {0.0f};

    uint8_t mRunFrame {};
    Timer mRunAnim {0.0f};

    int8_t mAngelFrame {};
    int8_t mAngelLoop {};
    Timer mAngelAnim {0.0f};

    SDL_FRect mHitbox {};
    SDL_FRect mChuteHitbox {};

    int mAudioLoopChannel {-1};


  public:
    Pilot();

    void setPlane( Plane* );

    void Move( const PLANE_PITCH );
    void MoveIdle();
    void OpenChute();
    void ChuteUnlock();

    void Update();
    void Draw() const;
    void DrawCollisionLayer() const;

    void FallUpdate();
    void RunUpdate();
    void DeathUpdate();
    void CoordinatesWrap();
    void AnimationsUpdate();
    void AnimationsReset();
    void FallAnimUpdate();
    void ChuteAnimUpdate();
    void HitboxUpdate();
    void ChuteHitboxUpdate();

    void FadeLoopingSounds();

    void Bail( const float planeX, const float planeY, const float bailDir );
    void ChuteHit( Plane& attacker );
    void Death();
    void Kill( Plane& attacker );
    void HitGroundCheck();
    void FallSurvive();
    void Rescue();
    void Respawn();

    bool isDead() const;
    bool isChuteOpen() const;
    bool isChuteBroken() const;
    bool isRunning() const;

    bool isHit( const float, const float ) const;
    bool ChuteIsHit( const float, const float ) const;

    float x() const;
    float y() const;
    SDL_FPoint speedVec() const;

    void setX( const float );
    void setY( const float );
  };


  Input input {};
  Pilot pilot {};

  Statistics mStats {};
};

extern std::map <PLANE_TYPE, Plane> planes;
