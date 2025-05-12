/**
 * @file pinconfig.h
 * @author 五十嵐　幸多 (kotakota925ika@gmail.com)
 * @brief ボードのピンを定義するヘッダーファイル
 * @version 0.1
 * @date 2024-05-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef NHK2024A_PINCONFIG_H
#define NHK2024A_PINCONFIG_H

#include <mbed.h>

/**
 * @brief ボードのピンの構造体
 * 
 */
struct Pins
{
    static PinName const CAN1_TD = PB_9;
    static PinName const CAN1_RD = PB_8;
    static PinName const CAN2_TD = PB_6;
    static PinName const CAN2_RD = PB_5;

    static PinName const MOSI = PB_15;
    static PinName const MISO = PB_14;
    static PinName const SCK = PB_13;
    static PinName const SD_CS = PB_12;

    static PinName const ATOM_TX1 = PC_10;
    static PinName const ATOM_RX1 = PC_11;
    static PinName const ATOM_TX2 = PC_12;
    static PinName const ATOM_RX2 = PD_2;
    static PinName const ATOM_SCL = PB_10;
    static PinName const ATOM_SDA = PB_3;

    static PinName const LED1 = PC_0;
    static PinName const LED2 = PC_1;
    static PinName const LED3 = PC_2;
    static PinName const LED4 = PC_3;
    static PinName const LED5 = PC_4;
    static PinName const LED6 = PC_5;

    static PinName const SW1 = PB_0;
    static PinName const SW2 = PB_1;
    static PinName const SW3 = PB_2;

    static PinName const EMC_STOP = PA_7;

    static PinName const BNO_SCL = PA_8;
    static PinName const BNO_SDA = PB_4;

    static PinName const USB_TX = PA_9;
    static PinName const USB_RX = PA_10;    

    static PinName const PUROPO_TX = PA_0;
    static PinName const PUROPO_RX = PA_1;

    static PinName const FEP_TX = PC_6;
    static PinName const FEP_RX = PC_7;
} pins;

#endif