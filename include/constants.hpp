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

#include <include/color.hpp>

#include <cstdint>


namespace constants
{
  static constexpr uint32_t tickRate {120};
  static constexpr uint32_t packetSendRate {60};
  static constexpr uint8_t defaultWinScore {10};


  static constexpr float baseWidth {256.f};
  static constexpr float baseHeight {208.f};
  static constexpr float aspectRatio {baseWidth / baseHeight};

  static constexpr int audioFadeDuration {500}; // milliseconds


  namespace colors
  {
    static constexpr Color background {0, 154, 239, 255};
    static constexpr Color letterbox {0, 0, 0, 255}; // TODO: choose suitable color

    static constexpr Color menuBox {254, 100, 99, 255};
    static constexpr Color menuHeader {102, 153, 204, 255};
    static constexpr Color menuBorder {99, 0, 181, 255};

    static constexpr Color explosionSpark[]
    {
      {0, 0, 0, 255},
      {246, 99, 0, 255},
      {255, 255, 255, 255},
      {253, 255, 108, 255},
    };

    namespace debug
    {
      namespace collisions
      {
        static constexpr Color planeToObstacles {255, 255, 0, 255};
        static constexpr Color planeToBullet {255, 0, 0, 255};

        static constexpr Color pilotToObstacles {255, 0, 255, 255};
        static constexpr Color pilotToBullet {255, 0, 0, 255};
        static constexpr Color pilotRescueZone {0, 255, 0, 255};

        static constexpr Color bulletToObstacles {255, 0, 0, 255};
        static constexpr Color bulletToChute {255, 0, 0, 255};

        static constexpr Color cloudToPlane {255, 255, 0, 255};
      }

      namespace ai
      {
        static constexpr Color planeSpeedVector {255, 255, 0, 255};
        static constexpr Color pilotSpeedVector {255, 255, 0, 255};

        static constexpr Color danger {255, 0, 0, 255};
      }
    }
  }


  namespace text
  {
    static constexpr float sizeX {8.f / baseWidth};
    static constexpr float sizeY {8.f / baseHeight};
  }


  namespace menu
  {
    static constexpr float originX {};
    static constexpr float originY {0.3f};

    static constexpr float sizeX {1.f};

    static constexpr double introAutoSkipTimeout {3.0};
    static constexpr double connectedMessageTimeout {3.0};

    namespace header
    {
      static constexpr float sizeX {1.f};
      static constexpr float sizeY {10.f / baseHeight};
    }

    namespace border
    {
      static constexpr float thicknessX {2.f / baseWidth};
      static constexpr float thicknessY {2.f / baseHeight};
    }

    namespace button
    {
      static constexpr float width {255.f};
      static constexpr float height {12.f};

      static constexpr float sizeX {menu::sizeX - 2.f * border::thicknessX};
      static constexpr float sizeY {height / baseHeight};
      static constexpr float speed {0.75f};

      static constexpr float originX {menu::originX + border::thicknessX};
      static constexpr float originY {menu::originY + header::sizeY};
    }
  }


//  PLANE
  namespace plane
  {
    static constexpr uint8_t maxHp {2};

    static constexpr float sizeX {24.f / baseWidth};
    static constexpr float sizeY {24.f / baseHeight};

    static constexpr float hitboxSizeX {sizeX / 3.f * 2.f};
    static constexpr float hitboxSizeY {sizeY / 3.f * 2.f};

    static constexpr float groundCollision {182.f / baseHeight};

    static constexpr float pitchStep {22.5f};

    static constexpr float acceleration {0.5f};
    static constexpr float deceleration {0.5f * acceleration};
    static constexpr float takeoffAcceleration {0.85f * acceleration};
    static constexpr float takeoffDeceleration {0.75f * acceleration};
    static constexpr float diveAcceleration {0.2f * acceleration};
    static constexpr float abandonedDeceleration {0.2f * acceleration};

    static constexpr float maxSpeedBase {0.303f};
    static constexpr float maxSpeedBoosted {0.43478f};
    static constexpr float maxSpeedAbandoned {0.5f * maxSpeedBase};

    static constexpr double deadCooldown {3.0};
    static constexpr double spawnProtectionCooldown {2.0};

    static constexpr double pitchCooldown {0.1};
    static constexpr double shootCooldown {0.65};

    static constexpr float spawnBlueX {16.f / baseWidth};
    static constexpr float spawnRedX {(baseWidth - 16.f) / baseWidth};
    static constexpr float spawnY {180.44f / baseHeight}; // TODO: verify

    static constexpr float spawnRotationBlue {67.5f};
    static constexpr float spawnRotationRed {292.5f};

    static constexpr float takeoffDirectionBlue {90.f};
    static constexpr float takeoffDirectionRed {270.f};
  }


//  SMOKE ANIM
  namespace smoke
  {
    static constexpr float sizeX {13.f / baseWidth};
    static constexpr float sizeY {13.f / baseHeight};

    static constexpr double frameTime {0.1};
    static constexpr uint8_t frameCount {5};
    static constexpr double cooldown {1.0};
  }

//  FIRE ANIM
  namespace fire
  {
    static constexpr float sizeX {13.f / baseWidth};
    static constexpr float sizeY {13.f / baseHeight};

    static constexpr double frameTime {0.075};
    static constexpr uint8_t frameCount {3};
  }


//  EXPLOSION ANIM
  namespace explosion
  {
    static constexpr float sizeX {40.f / baseWidth};
    static constexpr float sizeY {40.f / baseHeight};


    namespace spark
    {
      static constexpr uint8_t count {25};
      static constexpr uint8_t maxBounces {2};

      static constexpr float sizeX {2.f / baseWidth};
      static constexpr float sizeY {2.f / baseHeight};

      static constexpr float speedMin {0.4f};
      static constexpr float speedMax {0.6f};
      static constexpr float speedRange {speedMax - speedMin};
      static constexpr float speedBounce {0.1f};
      static constexpr float speedMask {count / 1.0123456789f};

      static constexpr float dirRange {75.f};
      static constexpr float dirOffset {dirRange * 0.2f};

      static constexpr float gravity {0.75f};
      static constexpr float groundCollision {195.f / baseHeight};
    }
  }


//  PILOT
  namespace pilot
  {
    static constexpr float sizeX {7.f / baseWidth};
    static constexpr float sizeY {7.f / baseHeight};
    static constexpr float groundCollision {185.64f / baseHeight};

    static constexpr float gravity {0.2f};
    static constexpr float ejectSpeed {0.45f};
    static constexpr float runSpeed {25.6f / baseWidth};
    static constexpr float maxFallSpeedX {2.048f / baseWidth};

    static constexpr double runFrameTime {0.075};
    static constexpr double fallFrameTime {0.1};

    namespace chute
    {
      static constexpr float sizeX {20.f / baseWidth};
      static constexpr float sizeY {12.f / baseHeight};
      static constexpr float offsetY {1.375f * chute::sizeY};

      static constexpr double frameTime {0.25};

      static constexpr float gravity {pilot::gravity};
      static constexpr float speed {10.24f / baseWidth};
    }

    namespace angel
    {
      static constexpr float sizeX {10.f / baseWidth};
      static constexpr float sizeY {8.f / baseHeight};
      static constexpr float ascentRate {7.28f / baseHeight};

      static constexpr double frameTime {0.138};
      static constexpr uint8_t frameCount {4};
      static constexpr uint8_t framePastLoopId {3};
      static constexpr uint8_t loopCount {6};
    }
  }


//  BULLET
  namespace bullet
  {
    static constexpr float sizeX {3.f / baseWidth};
    static constexpr float sizeY {3.f / baseHeight};
    static constexpr float speed {0.77f};

    static constexpr float groundCollision {186.16f / baseHeight};

    namespace hit
    {
      static constexpr float sizeX {9.f / baseWidth};
      static constexpr float sizeY {8.f / baseHeight};
    }
  }


//  BARN
  namespace barn
  {
    static constexpr float sizeX {35.f / baseWidth};
    static constexpr float sizeY {22.f / baseHeight};

    static constexpr float posX {0.5f - 0.5f * sizeX};
    static constexpr float posY {0.808f};

    static constexpr float planeCollisionX {0.5f - sizeX * 0.5f};
    static constexpr float planeCollisionY {163.904f / baseHeight}; // TODO: verify

    static constexpr float pilotCollisionLeftX {0.5f - sizeX * 0.4f};
    static constexpr float pilotCollisionRightX {0.5f + sizeX * 0.4f};

    static constexpr float bulletCollisionX {0.5f - sizeX * 0.475f};
    static constexpr float bulletCollisionY {168.48f / baseHeight}; // TODO: verify
    static constexpr float bulletCollisionSizeX {sizeX * 0.95f};

  }


//  CLOUD
  namespace cloud
  {
    static constexpr float sizeX {69.f / baseWidth};
    static constexpr float sizeY {32.f / baseHeight};

    static constexpr float speed {25.f / baseWidth};
    static constexpr float minSpeed {0.5f * speed};
    static constexpr float maxSpeed {1.5f * speed};
    static constexpr float speedRange {maxSpeed - minSpeed};

    static constexpr float spawnLeftX {25.6f / baseWidth};
    static constexpr float spawnRightX {153.6f / baseWidth};

    static constexpr float maxHeight {0.05f};
    static constexpr float minHeight {0.35f};
    static constexpr float heightRange {minHeight - maxHeight};
  }


//  ZEPPELIN
  namespace zeppelin
  {
    static constexpr float sizeX {51.f / baseWidth};
    static constexpr float sizeY {27.f / baseHeight};
    static constexpr float speed {3.584f / baseWidth};

    static constexpr float spawnX {0.5f};
    static constexpr float maxHeight {0.1f};
    static constexpr float minHeight {0.35f};


    namespace score
    {
      static constexpr float sizeX {5.f / baseWidth};
      static constexpr float sizeY {6.f / baseHeight};

      static constexpr float numOffsetY {sizeY * 0.95f};
      static constexpr float numOffsetBlue1X {sizeX * 2.1f};
      static constexpr float numOffsetBlue2X {sizeX * 1.1f};
      static constexpr float numOffsetRed1X {sizeX * 1.75f};
      static constexpr float numOffsetRed2X {sizeX * 0.75f};
    }
  }
}
