#include <mbed.h>
#include <ikarashiCAN_mk2.h>
#include <Servo.h>
#include "pinconfig.h"

ikarashiCAN_mk2 ican(CAN1_RD, CAN1_TD, 2, 1e6);
DigitalIn dip[]{
    DigitalIn(ADDR1),
    DigitalIn(ADDR2),
    DigitalIn(ADDR3),
    DigitalIn(ADDR4),
};
DigitalOut led[]{
    DigitalOut(LED_0),
    DigitalOut(LED_1),
    DigitalOut(LED_2),
    DigitalOut(LED_3),
};
DigitalOut rled(nanndakke_LED);
DigitalOut can_led(CAN_LED);
Servo servo0(SERVO_0);
Servo servo1(SERVO_1);
Servo servo2(SERVO_2);
Servo servo3(SERVO_3);
Servo servo4(SERVO_4);
Servo servo5(SERVO_5);
Servo servo6(SERVO_6);
Servo servo7(SERVO_7);
Servo servo8(SERVO_8);
Servo servo9(SERVO_9);
Servo servo10(SERVO_10);
Servo servo11(SERVO_11);

class can_servo_slave
{
public:
    can_servo_slave()
    {
        for (int i = 0; i < 8; i++)
        {
            csdf.data_array[i] = 0;
        }
    }

    void start()
    {
        ThisThread::sleep_for(100ms);
        for (int i = 0; i < 4; i++)
            led[i] = 0;
        for (int i = 0; i < 10; i++)
        {
            for (int j = 0; j < 4; j++)
                led[j] = !led[j];
            ThisThread::sleep_for(100ms);
        }
        for (int i = 0; i < 4; i++)
            led[i] = 1;
        ThisThread::sleep_for(500ms);
        for (int i = 0; i < 4; i++)
            led[i] = 0;
    }

    int dip_to_id()
    {
        int dip_value = 0;
        for (int i = 0; i < 4; i++)
            led[i] = !dip[i];
        for (int i = 0; i < 4; i++)
            dip_value |= ((int)!dip[i] << i);
        return dip_value;
    }

    void setup()
    {
        ican.set_this_id(this->dip_to_id());
        ican.read_start();
        rled = 1;
        servo0.calibrate(0.00095, 90);
        servo1.calibrate(0.00095, 90);
        servo2.calibrate(0.00095, 90);
        servo3.calibrate(0.00095, 90);
        servo4.calibrate(0.00095, 90);
        servo5.calibrate(0.00095, 90);
        servo6.calibrate(0.00095, 90);
        servo7.calibrate(0.00095, 90);
    }

    void read()
    {
        if (ican.get_read_flag() && (ican.get_target_id() == ican.get_this_id()))
        {
            can_led = 1;
            ican.get(csdf.data_array, sizeof(csdf.data_array));
        }
        else
        {
            can_led = 0;
            servo0 = 0;
            servo1 = 0;
            servo2 = 0;
            servo3 = 0;
            servo4 = 0;
            servo5 = 0;
            servo6 = 0;
            servo7 = 0;
        }
    }

    void update()
    {
        this->read();
        this->dip_to_id();

        servo0 = (float)csdf.servo[0] / 255;
        servo1 = (float)csdf.servo[1] / 255;
        servo2 = (float)csdf.servo[2] / 255;
        servo3 = (float)csdf.servo[3] / 255;
        servo4 = (float)csdf.servo[4] / 255;
        servo5 = (float)csdf.servo[5] / 255;
        servo6 = (float)csdf.servo[6] / 255;
        servo7 = (float)csdf.servo[7] / 255;
    }

private:
    union can_servo_data_format_1
    {
        struct
        {
            uint8_t servo[8];
        } __attribute__((packed));
        uint8_t data_array[8];
    } __attribute__((packed));

    can_servo_data_format_1 csdf;
}