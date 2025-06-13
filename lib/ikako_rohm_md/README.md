# 概要
毎年ROHMが提供しているモタドラをcanで制御できるようにするボードのライブラリ

# サンプルコード
```c++
#include <mbed.h>
#include <ikarashiCAN_mk2.h>
#include <rohm_md.h>

ikarashiCAN_mk2 can(PB_8, PB_9, 0, 1e6);
RohmMD md(&can, 1);
DigitalOut es(PA_7);
Thread thread;

int cnt = 0;

void send_thread()
{
    while (1)
    {
        md.send();
        cnt++;
        ThisThread::sleep_for(1ms);
    }
}

int main()
{
    float velocity = 0;
    es = 1;
    thread.start(&send_thread);
    can.read_start();
    while(1){
        velocity = 0.5f * sin(2 * M_PI * cnt / 1000);
        md.set(0, velocity);
        if(md.read()){
            printf("vel:%0.2f, cur:%0.2f, rad:%0.2f, ", md.get_vel(), md.get_cur(), md.get_rad());
        }
        printf(", velocity:%0.2f", velocity);
        // md.print_debug();
        printf("\n");
    }
}
```