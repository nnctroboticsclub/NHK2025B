/**
 * @file N_steer.h
 * @author 津輕 健太 (googology.fan@gmail.com)
 * @brief サーボを動かす、のクラスのプログラム
 * @version 0.1
 * @date 2025-04-23
 *
 * @copyright Copyright (c) 2025
 *
 */

#ifndef NHK2025B_STEER_H
#define NHK2025B_STEER_H

#include <bits/stdc++.h>
using namespace std;

#include <mbed.h>
#include <definitions.h>
// #include <servo.h>

#ifndef PI
// #define PI ((double)3.141592653589793238)
#define PI M_PI
#endif
#ifndef rad2deg
#define rad2deg(rad) ((int)(rad / PI * 180))
#endif
#ifndef deg2rad
#define deg2rad(deg) (deg * PI / 180)
#endif
#ifndef SERVO_MAX_DEG
#define SERVO_MAX_DEG 270
#endif
#ifndef SERVO_LIMIT_DEG
#define SERVO_LIMIT_DEG 45
#endif
#ifndef SERVO_INITIAL_DEG
#define SERVO_INITIAL_DEG 90
#endif

class NHK2025B_Steer
{
public:
  /**
   * @brief constructor
   */
  NHK2025B_Steer() {}

  /**
   * @brief setup
   */
  void setup()
  {
    steer_data.parameter.initial_deg = SERVO_INITIAL_DEG;
    steer_data.parameter.servo_limit_deg = SERVO_LIMIT_DEG;
    steer_data.parameter.max_deg = SERVO_MAX_DEG;
    steer_data.output.dir.front = -1;
    steer_data.output.dir.back = -1;
  }

  /**
   * @brief set direction of all steers to dir[rad]
   *
   * @param dir [rad] no detail for range
   */
  void setDirection(float dir)
  {
    setDirFront(rad2deg(dir));
    setDirBack(rad2deg(dir));
  }

  /**
   * @brief set direction of steers for turning
   *
   * @param cw -1: turning right, 0: not turning, 1: turning left
   */
  void setTurn(int cw)
  {
    setDirFront(steer_data.parameter.servo_limit_deg * cw);
    setDirBack(-steer_data.parameter.servo_limit_deg * cw);
  }

  /**
   * @brief set velocity of servo
   *
   * @param vel [m/s] no detail for range
   */
  void setVelocity(float vel)
  {
    steer_data.output.velocity = vel;
  }

  /**
   * @brief get angle of front steer
   *
   * @return angle of front steer
   */
  float getFrontAngle()
  {
    return deg2rad(steer_data.output.dir.front);
  }
  /**
   * @brief get angle of back steer
   *
   * @return angle of back steer
   */
  float getBackAngle()
  {
    return deg2rad(steer_data.output.dir.back);
  }
  /**
   * @brief get velocity of steers
   *
   * @return velocity of steers
   */
  float getVelocity()
  {
    return steer_data.output.velocity;
  }
  /**
   * @brief get initial_angle of this structure
   *
   * @return initial_angle of this structure
   */
  float getInitAngle()
  {
    return deg2rad(steer_data.parameter.initial_deg);
  }
  /**
   * @brief get limit_deg of this structure
   *
   * @return limit_deg of this structure
   */
  float getMaxAngle()
  {
    return deg2rad(steer_data.parameter.initial_deg);
  }

  /**
   * @brief empty functions
   */
  void update() {}
  void print_debug() {}

private:
  struct
  {
    struct
    {
      struct
      {
      } cmd;
      struct
      {
        int front;
        int back;
      } dir;
      float velocity;
    } output;
    struct
    {
      int initial_deg;     // 0度の指定
      int servo_limit_deg; // 振れ幅の限界
      int max_deg;
      struct
      {
        int front;
        int back;
      } id;
    } parameter;
  } steer_data;

  void setDirFront(int dir)
  {
    if (!directionOutOfRange(dir))
      steer_data.output.dir.front = getDir(dir);
  }
  void setDirBack(int dir)
  {
    if (!directionOutOfRange(dir))
      steer_data.output.dir.back = getDir(dir);
  }

  int getDir(int dir)
  {
    return steer_data.parameter.initial_deg + dir;
  }
  bool directionOutOfRange(float dir)
  {
    bool f = abs(dir) > steer_data.parameter.servo_limit_deg;
    if (f)
    {
      cout << "direction(" << dir << ") is out of range. (current max degree is set to " << steer_data.parameter.servo_limit_deg << ')' << endl;
      printf("direction(%f) is out of range. (current max degree is set to %d)\n", dir, steer_data.parameter.servo_limit_deg);
    }
    return f;
  }
};

#endif // NHK2025B_STEER_H