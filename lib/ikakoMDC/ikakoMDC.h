#include "mbed.h"
#include "ikarashiCAN_mk2.h"
#include "PID.h"
#include "lpf.h"

#ifndef IKAKO_MDC
#define IKAKO_MDC

class ikakoMDC : public lpf, public PID
{
public:

    ikakoMDC(int _motor_num, int _min_speed, int _max_speed, float cycle_s, float time_s, float Kc, float tauI, float tauD, float interval);
    ikakoMDC(){}
    void set_speed(int8_t _speed, bool mode = true);
    int get_enc();
    int get_speed();
    int get_motor_num();

    void pid_update(bool mode = true);
    void set_limits(float inMin, float inMax, float outMin, float outMax);
    void set_tunings(float Kc, float tauI, float tauD);
    void set_set_point(float sp);
    void set_process_value(float pv);

    void stop();
    void start();

    int motor_num;
    int pid_speed;
    int speed;
    int result_speed;
    int enc;
    int max_speed;
    int min_speed;
    int max_out_speed;
    int min_out_speed;
    int pid_max_speed;
    int pid_min_speed;
    bool stop_flag;

private:
};

class ikakoMDC_sender
{
public:

    ikakoMDC_sender(ikakoMDC *_imdc, int _imdc_size, ikarashiCAN_mk2 *_can, int _mdc_id);
    ikakoMDC_sender(){}
    int send();
    bool read_enc();
    int get_mdc_id();
    void send_start();

private:

    void thread_task();

    int mdc_id;
    int mdc_size;
    ikakoMDC *imdc;
    ikarashiCAN_mk2 *can;
    Thread *mdc_thread;

    union can_motor_speed_data_format
    {
        struct
        {
            int8_t motor[4];
        } __attribute__((packed));
        uint8_t data_array[4];
        uint64_t data;
    } __attribute__((packed));

    union can_encoder_data_format
    {
        struct
        {
            int16_t encoder[4];
        } __attribute__((packed));
        uint8_t data_array[8];
        uint64_t data;
    } __attribute__((packed));

    can_motor_speed_data_format cmsDF;
    can_encoder_data_format ceDF;

};

#endif