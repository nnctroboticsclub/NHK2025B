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

// #include <mbed.h>
#include <definitions.h>
// #include <servo.h>

#ifndef PI
// #define PI ((double)3.141592653589793238)
#define PI M_PI
#endif
#ifndef RAD2DEG
#define RAD2DEG(rad) ((int)(rad / M_PI * 180))
#endif
#ifndef DEG2RAD
#define DEG2RAD(deg) (deg * M_PI / 180)
#endif
#ifndef SERVO_MAX_DEG
#define SERVO_MAX_DEG 270
#endif
#ifndef SERVO_LIMIT_DEG
#define SERVO_LIMIT_DEG 35
#endif
#ifndef SERVO_INITIAL_DEG
#define SERVO_INITIAL_DEG 120
#endif

class SteerParameter
{
public:
  float wheel_base = 900e-3; // [m]
  float track_width = 400e-3; // [m]
  float wheel_radius = 76e-3; // [m]
  float steer_max_angle = DEG2RAD(25); // [rad]
  float servo_initial_angle = DEG2RAD(120); // [rad]
  float max_velocity = 5.0; // [m/s]
};

class NHK2025B_Steer
{
public:
  /**
   * @brief constructor
   */
  NHK2025B_Steer(SteerParameter param){
    steer_data.parameter = param;
  }

  /**
   * @brief setup
   */
  void setup()
  {
  }

  /**
   * @brief set direction of all steers to dir[rad]
   *
   * @param dir [rad] no detail for range
   */
  void setDirection(float dir)
  {
    steer_data.cmd.direction = dir;
    std::max(
      -steer_data.parameter.steer_max_angle,
      std::min(
        steer_data.parameter.steer_max_angle,
        dir
      )
    );
    steer_data.cmd.turn = TurnDirection::NT;
  }

  /**
   * @brief set turn of all steers to angle[rad]
   * 
   * @param ang [rad]
   */
  void setTurnAngle(float ang)
  {
    steer_data.cmd.turn_angle = std::max(
      -steer_data.parameter.steer_max_angle,
      std::min(
        steer_data.parameter.steer_max_angle,
        ang
      )
    );
    if(steer_data.cmd.turn_angle > 0){
      steer_data.cmd.turn = TurnDirection::CCW;
    }
    if(steer_data.cmd.turn_angle < 0){
      steer_data.cmd.turn = TurnDirection::CW;
    }
  }

  /**
   * @brief set direction of steers for turning
   *
   * @param ccw -1: turning right, 0: not turning, 1: turning left
   */
  void setTurn(TurnDirection turn)
  {
    steer_data.cmd.turn = turn;
    switch(steer_data.cmd.turn){
      case TurnDirection::CCW:
        steer_data.cmd.turn_angle = steer_data.parameter.steer_max_angle;
        break;
      case TurnDirection::CW:
        steer_data.cmd.turn_angle = -steer_data.parameter.steer_max_angle;
        break;
      case TurnDirection::NT:
      default:
        break;
    };
  }

  /**
   * @brief set velocity of servo
   *
   * @param vel [m/s] no detail for range
   */
  void setVelocity(float vel)
  {
    steer_data.cmd.velocity = std::max(
      -steer_data.parameter.max_velocity,
      std::min(
        steer_data.parameter.max_velocity,
        vel
      )
    );
  }

  /**
   * @brief get angle of front steer
   *
   * @param dir1 front or back
   * @return angle of steer
   */
  float getAngle(Direction1 dir1)
  {
    return steer_data.output.angle[(int)dir1];
  }

  /**
   * @brief get velocity of steers
   *
   * @param wheel front_right, front_left, back_left, back_right
   * 
   * @return velocity of steers[m/s]
   */
  float getVelocity(Wheel wheel)
  {
    return steer_data.output.velocity[(int)wheel];
  }

  /**
   * @brief get omega of steers
   *
   * @return omega of steers[rad/s]
   */
  float getOmega(Wheel wheel)
  {
    return steer_data.output.omega[(int)wheel];
  }

  /**
   * @brief get initial_angle of this structure
   *
   * @return initial_angle of this structure
   */
  float getInitAngle()
  {
    return steer_data.parameter.servo_initial_angle;
  }

  /**
   * @brief get limit_deg of this structure
   *
   * @return limit_deg of this structure
   */
  float getMaxAngle()
  {
    return steer_data.parameter.steer_max_angle;
  }

  /**
   * @brief get parameter of this structure
   * 
   * @return parameter of this structure
   */
  SteerParameter getParameter()
  {
    return steer_data.parameter;
  }

  /**
   * @brief update function
   */
  void update()
  {
    steer_data.state.turning_radius = steer_data.parameter.wheel_base / (sin(steer_data.cmd.turn_angle));
    steer_data.state.diff_velocity = steer_data.cmd.velocity * ((steer_data.parameter.track_width / 2) / steer_data.state.turning_radius);
    for(int i=0;i<4;i++){
      steer_data.output.velocity[i] = steer_data.cmd.velocity;
    }
    steer_data.output.velocity[(int)Wheel::FRONT_RIGHT] -= steer_data.state.diff_velocity;
    steer_data.output.velocity[(int)Wheel::FRONT_LEFT] += steer_data.state.diff_velocity;
    steer_data.output.velocity[(int)Wheel::BACK_LEFT] -= steer_data.state.diff_velocity;
    steer_data.output.velocity[(int)Wheel::BACK_RIGHT] += steer_data.state.diff_velocity;
    steer_data.output.omega[(int)Wheel::FRONT_RIGHT] = steer_data.output.velocity[(int)Wheel::FRONT_RIGHT] / steer_data.parameter.wheel_radius;
    steer_data.output.omega[(int)Wheel::FRONT_LEFT] = steer_data.output.velocity[(int)Wheel::FRONT_LEFT] / steer_data.parameter.wheel_radius;
    steer_data.output.omega[(int)Wheel::BACK_LEFT] = steer_data.output.velocity[(int)Wheel::BACK_LEFT] / steer_data.parameter.wheel_radius;
    steer_data.output.omega[(int)Wheel::BACK_RIGHT] = steer_data.output.velocity[(int)Wheel::FRONT_RIGHT] / steer_data.parameter.wheel_radius;
    
    for(int i=0;i<2;i++){
      steer_data.output.angle[i] = steer_data.parameter.servo_initial_angle;
    }
    if(steer_data.cmd.turn == TurnDirection::NT){
      steer_data.output.angle[(int)Direction1::FRONT] += steer_data.cmd.direction;
      steer_data.output.angle[(int)Direction1::BACK] -= steer_data.cmd.direction;
    }else{
      steer_data.output.angle[(int)Direction1::FRONT] += steer_data.cmd.turn_angle;
      steer_data.output.angle[(int)Direction1::BACK] += steer_data.cmd.turn_angle;
    }
  }
  /**
   * @brief empty functions
   */
  void print_debug() {}

  /**
   * @brief debug parameters once
   */
  void debug_once()
  {
    printf("dir: {front: %d, back: %d}, velocity: {%f, %f, %f, %f}\n",
      RAD2DEG(steer_data.output.angle[(int)Direction1::FRONT]),
      RAD2DEG(steer_data.output.angle[(int)Direction1::BACK]),
      steer_data.output.velocity[(int)Wheel::FRONT_RIGHT],
      steer_data.output.velocity[(int)Wheel::FRONT_LEFT],
      steer_data.output.velocity[(int)Wheel::BACK_LEFT],
      steer_data.output.velocity[(int)Wheel::BACK_RIGHT]
    );
  }

private:
  struct
  {
    struct
    {
      float angle[2]; // [rad]
      float velocity[4]; // [m/s]
      float omega[4]; // [rad/s]
    } output;
    struct{
      float velocity; // [m/s]
      float direction; // [rad]
      float turn_angle; // [rad]
      TurnDirection turn;
    }cmd;
    struct{
      float turning_radius; // [m]
      float diff_velocity; // [m/s]
    }state;
    SteerParameter parameter;
  } steer_data;
};

#endif // NHK2025B_STEER_H