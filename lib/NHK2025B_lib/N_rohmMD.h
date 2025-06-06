#ifndef NHK2025B_N_ROHM_MD_H
#define NHK2025B_N_ROHM_MD_H

#include <mbed.h>
#include "definitions.h"
#include "ikarashiCAN_mk2.h"

struct RohmMdParameter {
    int id = 1;
    ikarashiCAN_mk2* ican = &can1;
    // 他に必要なパラメータがあれば追加していって
};

class NHK2025B_RohmMD{
public:
    NHK2025B_RohmMD(){}
    void writeCan1(int board_id);
    void writeCan2(int board_id);
private:
};

#endif // NHK2025B_N_ROHM_MD_H