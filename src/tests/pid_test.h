#include "N_PID.h"
#include <mbed.h>
#include "definitions.h"
#include "N_robomas.h"

std::array<RobomasParameter, NUM_OF_ROBOMAS> params{{
    []
    { RobomasParameter p; p.robomas_id = 2; p.type = RobomasParameter::TYPE_OF_M2006; p.ican_ptr = &can1; return p; }(),
    // []
    // { RobomasParameter p; p.robomas_id = 2; return p; }(),
    // []
    // { RobomasParameter p; p.robomas_id = 3; return p; }(),
    // []
    // { RobomasParameter p; p.robomas_id = 4; return p; }(),
}};

NHK2025B_Robomas robomas(params);
Thread thread;
Ticker ticker;

PidParameter pid_param;
NHK2025B_PID pid;
float target = 0.F;

void send_thread()
{
  while (true)
  {
    robomas.write();
    ThisThread::sleep_for(1ms);
  }
}

int cnt_1ms = 0;
void update_ts()
{
  robomas.update_ts();
  for (int i = 0; i < NUM_OF_ROBOMAS; ++i)
    // robomas.setCurrent(i, pid.calc(target - robomas.getVeclocity(i), i)); // 位置制御
    robomas.setCurrent(i, pid.calc(target - robomas.getVeclocity(i), i)); // 速度制御
  cnt_1ms++;
}

void print_debug()
{
  can1.print_debug();
  // can2.print_debug();
}

DigitalIn button(pins.SW1);
int main()
{
  robomas.setup();
  thread.start(&send_thread);
  ticker.attach(&update_ts, 1ms);
  can1.read_start();
  // can2.read_start();
  int loop_cnt = 0;
  ES = 1;

  pid_param.kp = 1.0;
  // pid_param.kd = 0.5F;
  // pid_param.ki = 0.01F;
  pid = NHK2025B_PID(pid_param);

  float speed = M_PI_2 * 4;
  int curr = 0;
  while (true)
  {
    if (curr != button)
      pid.reset();
    curr = button;
    target = button * speed;

    if (!(loop_cnt++ % 100))
    {
      // robomas.print_debug();
      // can1.print_debug();
      printf("robomas angle: %0.3f, target: %0.3f\n", robomas.getAbsAngle(0), target);
      pid.print_debug();
      puts("");
    }
    robomas.update();
    can1.update();
    // can2.update();
    ThisThread::sleep_for(1ms);
  }

  return 0;
}