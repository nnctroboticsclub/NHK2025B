# サンプルプログラム
```C
#include <mbed.h>
#include <can_servo.h>
#include <button_state.h>
#define DEG_MAX 180

ikarashiCAN_mk2 ican(D15, D14, 0, 500000);
can_servo servo(&ican, 2);
DigitalOut led(LED2);
DigitalIn button(USER_BUTTON);
Button_state b_state;

int main()
{
    uint8_t btn_buff = 0;
    int state = 0;
    float deg = 90;
    float servo_out = 0;
    while (1)
    {
        btn_buff = button;
        state = b_state.state(btn_buff);
        if (state == -1)
        {
            deg = 90;
            //servo_out = 255;
        }
        else if (state == 1)
        {
            deg = 0;
            //servo_out = 0;
        }
        servo_out = deg / DEG_MAX * 255;
        for (int i = 0; i < 8; i++)
        {
            servo.set(i, servo_out);
        }
        servo.set(5, deg / 270 * 255);
        led = servo.send();
        printf("\nsend:%d|", ican.get_send_flag());
        printf(" out:%d|", (int)servo_out);
        printf(" btn:%d|", state);
        ThisThread::sleep_for(1ms);
    }
}
```