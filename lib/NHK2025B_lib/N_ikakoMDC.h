#ifndef NHK2025B_N_IKAKO_MDC_H
#define NHK2025B_N_IKAKO_MDC_H

#include <mbed.h>

class NHK2025B_IkakoMDC{
public:
    NHK2025B_IkakoMDC(){}
    void writeCan1(int board_id);
    void writeCan2(int board_id);
private:
};

#endif // NHK2025B_N_ROHM_MD_H