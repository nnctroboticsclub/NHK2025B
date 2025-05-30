#include <mbed.h>
#include "N_puropo.h"

Ticker ticker;
NHK2025B_Puropo puropo;

int cnt_1ms = 0;
void update_1ms()
{
    cnt_1ms++;
}

void print_debug()
{
    puropo.print_debug();
}

int main()
{
    puropo.setup();
    while(true){
        if(cnt_1ms > 100){
            cnt_1ms = 0;
            print_debug();
            printf("\n");
        }
        puropo.update();
    }
}