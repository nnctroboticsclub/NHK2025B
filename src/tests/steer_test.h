#include "mbed.h"
#include "N_servo.h"
#include "N_steer.h"
#include "definitions.h"

std::array<ServoParameter, NUM_OF_SERVO> servo_param{{
  []{ServoParameter p;
  p.id = 0;
  return p;}(),
  []{ServoParameter p;
  p.id = 1;
  return p;}(),
}};

NHK2025B_Servo servo(servo_param);
NHK2025B_Steer steer;
Thread thread;

void send_thread()
{
  while (true)
  {
    servo.write();
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
    can1.print_debug();
    puts("");
    servo.setAngle(0, steer.getFrontAngle());
    servo.setAngle(1, steer.getBackAngle());
    servo.update();
    ThisThread::sleep_for(2s);
  }
  return 0;
}