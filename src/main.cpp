// メイン
// #include "main.h"

// ハードウェア系
// #include "tests/servo_test.h" // 未実装
// #include "tests/robomas_test.h" // 未実装
// #include "tests/puropo_test.h" // 未試験
// #include "tests/rohmMD_test.h"
// #include "tests/ikakoMDC_test.h"

// ソフトウェア系
// #include "tests/pid_test.h" // 未実装
// #include "tests/steer_test.h" // 未実装
// #include "tests/controller_test.h" // 未試験
// #include "tests/steer_test.h"
// #include "tests/arm_test.h"
// #include "tests/elevator_test.h"

#include "tests/steer_and_controller.h"

// #include <mbed.h>

// UnbufferedSerial serial(PA_0,PA_1,115200);

// int main(){
//     uint8_t buf[256];
//     serial.set_blocking(true);
//     // serial.format(8,SerialBase::None,2);
//     while(1){
//         puts("hello");
//         printf("%d",serial.read(buf,1));
//         for(int i=0;i<20;i++){
//             printf("%d,",buf[i]);
//         }
//         puts("");
//     }
// }