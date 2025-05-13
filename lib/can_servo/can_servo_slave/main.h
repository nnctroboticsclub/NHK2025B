#include "can_servo_slave.h"

can_servo_slave css;

int main()
{
    css.setup();
    css.start();
    while(1)
    {
        css.update();
        printf("\nservo:%d %d %d %d|", (int)(servo0 * 255), (int)(servo1 * 255), (int)(servo2 * 255), (int)(servo3 * 255));
        printf(" read:%d|", ican.get_read_flag());
        printf(" this:%d|", ican.get_this_id());
        printf(" target:%d|", ican.get_target_id());
    }
}