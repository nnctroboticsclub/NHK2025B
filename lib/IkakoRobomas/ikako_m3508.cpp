#include "ikako_m3508.h"

IkakoM3508::IkakoM3508(int _id)
{
    config.read_flag = 0;
    this->set_params(_id);
}

IkakoM3508::IkakoM3508(){}

void IkakoM3508::set_params(int _id)
{
    config.id = _id;
    config.read_flag = 0;
    params.R = this->R;
    params.L = this->L;
    params.D = 0.0;
    params.J = 0.04;
    params.Kt = this->kt;
    params.p_pairs = this->p_pairs;
    params.gear_ratio = this->gear_ratio;
    config.current_limit = this->CURRENT_LIMIT;
    config.convert_current_ref_ratio = this->CONVERT_CURRENT_REF_RATIO;
    config.convert_current_to_float = this->CONVERT_CURRENT_TO_FLOAT;
    config.convert_position_to_float = this->CONVERT_POSITION_TO_FLOAT;
    config.convert_velocity_to_float = this->CONVERT_VELOCITY_TO_FLOAT;
}
