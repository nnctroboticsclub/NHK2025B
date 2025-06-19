#include "N_servo.h"
#include "N_steer.h"

NHK_servo servo(&can1, 1);
NHK2025B_Steer steer;
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
  steer.setup();
  thread.start(&send_thread);
  ES = 1;
  int i = 0;
  while (1)
  {
    switch ((i++) % 4)
    {
    case 0:
      steer.setTurn(0);
      printf("setTurn(0):  ");
      break;
    case 1:
      steer.setTurn(-1);
      printf("setTurn(-1): ");
      break;
    case 2:
      steer.setTurn(0);
      printf("setTurn(0):  ");
      break;
    case 3:
      steer.setTurn(1);
      printf("setTurn(1):  ");
      break;
    }
    printf("front: %f(%d), back: %f(%d), velocity: %f\n", steer.getFrontAngle(), rad2deg(steer.getFrontAngle()), steer.getBackAngle(), rad2deg(steer.getBackAngle()), steer.getVelocity());
    servo.set_deg(0, rad2deg(steer.getFrontAngle()), steer.getMaxAngle());
    servo.set_deg(1, rad2deg(steer.getBackAngle()), steer.getMaxAngle());
    ThisThread::sleep_for(2s);
  }
  return 0;
}