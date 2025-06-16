/**
 * @file K_ikakoMDC.h
 * @author 高野絆(takanokiduna@gmail.com)
 * @brief NHK2025BのikakoMDCクラス
 * @version 0.1
 * @date 2025-06-16
 * 
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_N_IKAKO_MDC_H
#define NHK2025B_N_IKAKO_MDC_H

#include <mbed.h>
#include "definitions.h"
#include "ikarashiCAN_mk2.h"

class IkakoMdcParameter {
    int id = 0;
    int board_id = 0;
    ikarashiCAN_mk2* ican = &can1;
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_IkakoMDC{
public:
    NHK2025B_IkakoMDC(){}
private:
};

#endif // NHK2025B_N_ROHM_MD_H