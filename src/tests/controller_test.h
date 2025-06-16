#include <mbed.h>
#include "N_puropo.h"
#include "N_controller.h"

Ticker ticker;
NHK2025B_Controller controller;
NHK2025B_Puropo puropo;

int cnt_1ms = 0;
void update_1ms()
{
    cnt_1ms++;
}

void print_debug()
{
    controller.print_debug();
    puropo.print_debug();
}

int main()
{
    puropo.setup();
    controller.setup();
    ticker.attach(&update_1ms,1ms);
    while(true){
        if(cnt_1ms > 100){
            cnt_1ms = 0;
            print_debug();
            printf("\n");
        }

        controller.setSteerDirection(puropo.getLeftX(0));
        controller.setSteerVelocity(puropo.getLeftY(0));
        controller.setSteerTurn(puropo.getRightX(0));

        controller.update();
        puropo.update();
        puts("return loop");
    }
}