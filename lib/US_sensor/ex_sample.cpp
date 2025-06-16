#include <mbed.h>
#include "US_Sensor.h"
US_Sensor us_sensor(D7, D8);

int main()
{
    int error_cnt = 0; 
    us_sensor.setup();
    while (1)
    {
        us_sensor.shot();
        while (us_sensor.falling_frag)
        {
            ThisThread::sleep_for(5ms);
            error_cnt++;
            if (error_cnt >= 10)
            {
                printf("Not returned!!\n");
                break;
            }
        }
        if (error_cnt < 10)
        {

            printf("%5d %f\n", us_sensor.getDistance());
        }
        error_cnt = 0;
    }

    return 0;
}
