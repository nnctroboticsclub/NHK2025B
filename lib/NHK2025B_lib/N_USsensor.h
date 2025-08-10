/**
 * @file N_template.h
 * @author 高野 絆(takanokiduna@gmail.com)
 * @brief NHK2025Bの超音波センサのクラス
 * @version 0.1
 * @date 2025-07-09
 *
 * @copyright Copyright (c) 2025
 */
#ifndef NHK2025B_USsensor_H
#define NHK2025B_USsensor_H

#include <mbed.h>
#include "definitions.h"
#include "US_Sensor.h"

class USsensorParameter{
public:
    float tempreature; // [℃]
    int time_out; // [ms]
};

class NHK2025B_USsensor{
    NHK2025B_USsensor(std::array<USsensorParameter, NUM_OF_US_SENSOR> param ={{USsensorParameter()}}){
        for(int i=0;i<NUM_OF_US_SENSOR;i++){
            ;
        }
    }
};

#endif // NHK2025B_USsensor_H