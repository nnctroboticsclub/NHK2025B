#include "mbed.h"
#include "lpf.h"

lpf::lpf(float system_cycle_second_,float arrive_time_){

    /*
        引数1 制御周期(秒)
        引数2 何秒で目標値へ到達させたいか(s)時定数ではない
    */
    
    system_cycle_second = system_cycle_second_;
    f_t_const = arrive_time_;
    one_before_output = 0.0;
    output=0.0;
}

float lpf::path_value(float target_value){
     
    output = ((system_cycle_second * target_value) + (f_t_const * one_before_output))/(system_cycle_second + f_t_const);
    one_before_output = output;
    
    return output;
}


void lpf::change_time_constant(float change_time_constant_){
    
    f_t_const = change_time_constant_;
}


void lpf::reset(void){
    
    output = 0.0;
    one_before_output = 0.0;
    
}