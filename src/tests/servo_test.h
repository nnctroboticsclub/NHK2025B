#include "N_servo.h"

NHK_servo servo(&can1, 1);
Thread thread;

void send_thread()
{
  while (true)
  {
    servo.update();
    ThisThread::sleep_for(1ms);
  }
}

int main()
{
  servo.setup();
  thread.start(&send_thread);
  ES = 1;
  while (1)
  {
    servo.set_deg(0, 90, SERVO_MAX_DEG);
    ThisThread::sleep_for(2s);
    servo.set_deg(0, 180, SERVO_MAX_DEG);
    ThisThread::sleep_for(2s);
  }
  return 0;
}