#include <mbed.h>
#include "N_puropo.h"

Ticker ticker;
NHK2025B_Puropo puropo;

int cnt_1ms = 0;
void update_1ms()
{
    puropo.update_ts();
    cnt_1ms++;
}

void print_debug()
{
    puropo.print_debug();
}

int main()
{
    puts("setup start");
    puropo.setup();
    puts("setup ok");
    ES = 1;
    ticker.attach(&update_1ms,1ms);
    puts("ticker ok");
    while(true){
        if(cnt_1ms > 100){
            cnt_1ms = 0;
            print_debug();
            printf("\n");
        }
        // puts("print ok");
        puropo.update();
        // puts("all ok");
    }
}
