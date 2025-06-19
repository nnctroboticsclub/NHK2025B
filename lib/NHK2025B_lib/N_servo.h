#ifndef NHK2025B_SERVO_H
#define NHK2025B_SERVO_H

#include <mbed.h>
#include "definitions.h"
#include "ikarashiCAN_mk2.h"
#include "can_servo.h"
#include "pinconfig.h"

class ServoParameter{
public:
    int id = 0;
    int board_id = 1;
    ikarashiCAN_mk2* ican = &can1;
    float max_angle = M_PI + M_2_PI; // [rad]
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_Servo{
public:
    /**
     * @brief コンストラクタ
     */
    NHK2025B_Servo(std::array<ServoParameter, NUM_OF_SERVO> param = {ServoParameter()}){
        ServoParameter buffer[NUM_OF_SERVO];
        for(int i=0;i<NUM_OF_SERVO;i++){
            servo_data[i].parameter = param[i];
            buffer[i] = servo_data[i].parameter;
            servo_data[i].state.is_used = false;
        }
        for(int i=0;i<NUM_OF_SERVO;i++){
            for(int j=i+1;j<NUM_OF_SERVO;j++){
                if((buffer[i].ican == buffer[j].ican) && (buffer[i].board_id == buffer[j].board_id)){
                    buffer[j].board_id = -1;
                }
            }
        }
        int num_of_board = 0;
        for(int i=0;i<NUM_OF_SERVO;i++){
            if(num_of_board < NUM_OF_SERVO_BOARD){
                if(buffer[i].board_id != -1){
                    servo[num_of_board] = can_servo(servo_data[i].parameter.ican,servo_data[i].parameter.board_id);
                    // 仲間を見つける
                    for(int j=0;j<NUM_OF_SERVO;j++){
                        if((servo_data[j].parameter.board_id == servo_data[i].parameter.board_id) &&
                        (servo_data[j].parameter.ican == servo_data[i].parameter.ican)){
                            servo_data[j].state.servo = &servo[num_of_board];
                            servo_data[j].state.is_used = true;
                        }
                    }
                    num_of_board++;
                }
            }else{
                ServoParameter s;
                memset(&s,0,sizeof(ServoParameter));
                servo_data[i].parameter = s;
            }
        }

        auto print_servo_parameter = [](const ServoParameter& s) {
            const char* can_name = (s.ican == &can1) ? "can1" :(s.ican == &can2)? "can2":"NULL";
            printf("ServoParameter {\n");
            printf("  id        : %d\n", s.id);
            printf("  board_id  : %d\n", s.board_id);
            printf("  ican      : %s\n", can_name);
            printf("  max_angle : %.3f [rad]\n", s.max_angle);
            printf("}\n");
        };
        for(int i=0;i<NUM_OF_SERVO;i++){
            print_servo_parameter(servo_data[i].parameter);
        }

        puts("");
        print_debug();
        puts("");
    }

    /**
     * @brief セットアップ関数
     */
    void setup(){
        cnt_write = 0;
    }

    /**
     * @brief send_threadの中に入れる
     */
    void write()
    {
        if(servo_data[cnt_write%NUM_OF_SERVO].state.is_used == true){
            servo_data[cnt_write % NUM_OF_SERVO].state.servo->send();
        }
        cnt_write++;
    }

    /**
     * @brief 角度をセットする
     * 
     * @param angle 角度[rad]
     */
    void setAngle(int num, float angle)
    {
        servo_data[num].cmd.angle = angle;
    }

    /**
     * @brief アップデート関数
     */
    void update(){
        for(int i=0;i<NUM_OF_SERVO;i++){
            if(servo_data[i].state.is_used == true){
                servo_data[i].state.servo->set(servo_data[i].parameter.id,(uint8_t)(servo_data[i].cmd.angle / servo_data[i].parameter.max_angle * 255));
            }
        }
    }

    /**
     * @brief デバッグ関数
     */
    void print_debug()
    {
        for (int i = 0; i < NUM_OF_SERVO; i++) {
            printf("servo_data{");
    
            printf("cmd{");
            printf("angle: %0.2f", servo_data[i].cmd.angle);
            printf("}");
    
            printf("state{");
            printf("is_used: %d", servo_data[i].state.is_used);
            printf("|");
            printf("servo: %s", (servo_data[i].state.servo != nullptr) ? "set" : "null");
            printf("}");
    
            printf("}|");  // servo_data閉じ、区切り
        }
    }
private:
    std::array<can_servo,NUM_OF_SERVO_BOARD> servo;
    struct{
        struct{
            float angle;
        }cmd;
        struct{
            bool is_used;
            can_servo *servo;
        }state;
        ServoParameter parameter;
    }servo_data[NUM_OF_SERVO];
    int cnt_write;
};

#endif // NHK2025B_SERVO_H