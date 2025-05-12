/**
 * @file difinitions.h
 * @author 五十嵐　幸多 (kotakota925ika@gmail.com)
 * @brief 複数のファイル間で共有したい項目を定義する。多分、グローバル変数大量になるから可能な限りコメント書いて
 * @version 0.1
 * @date 2024-05-30
 *
 * @copyright Copyright (c) 2024
 *
 */
#ifndef NHK2025B_DEFINITIONS_H
#define NHK2025B_DEFINITIONS_H

#include <mbed.h>
#include <ikarashiCAN_mk2.h>
#include "pinconfig.h"

#define NUM_OF_ROBOMAS_M3508 1 // m3508モータの数
#define NUM_OF_ROBOMAS_M2006 1 // m2006モータの数
#define NUM_OF_SERVO 8         // サーボモータの数
#define NUM_OF_SERVO_BOARD 1   // canサーボ基板の数（1個につきサーボ8個動かせる）
#define NUM_OF_DC_MOTOR 4      // DCモータの数  // takano使います
#define CAN_FREQUENCY 1e6      // canの周波数。ロボますモータは1 [MHz]だけど、これまで部内のモジュールは50 [kHz]だから気をつけて

#define STEER_MAX_VELOCITY 3.0 // [m/s]
#define STEER_MAX_RAD M_PI_4 // [rad]

// canの定義
ikarashiCAN_mk2 can1(pins.CAN1_RD, pins.CAN1_TD, 0, CAN_FREQUENCY);
ikarashiCAN_mk2 can2(pins.CAN2_RD, pins.CAN2_TD, 0, CAN_FREQUENCY);

DigitalOut ES(pins.EMC_STOP); // 非常停止

DigitalOut led[] = {
    DigitalOut(pins.LED1),
    DigitalOut(pins.LED2),
    DigitalOut(pins.LED3),
    DigitalOut(pins.LED4),
    DigitalOut(pins.LED5),
    DigitalOut(pins.LED6),
};

// canサーボのid
#define CAN_SERVO_ID 1
#define SERVO_MAX_DEG 270

// 高野のdefine
#define MDC_ID 2     // mdcのid
#define CAN_MDC can2 // mdcが使うcan

#define ROBOMAS_CAN can1 // ロボマスモータの制御に使うcan

#endif