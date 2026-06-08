#include  "motor_params_detection.h"


void detection_init(params_detection_t *detection)
{
    detection->result.ld = 0;
    detection->result.lq = 0;
    detection->result.rs = 0;
    detection->tick_count = 0;
}

param_detection_status  Rs_detection_pro(params_detection_t *detection)
{
    float is;
    detection->common_use.uq_ref = 0;
    detection->common_use.ud_ref = Ud_REF;
     if (detection->tick_count == 0) {
        detection->ls.is_last = detection->common_use.i_alpha;
    }
    detection->tick_count++;
    detection->cfg.inductor_detection_times = 10;
    if (detection->tick_count > detection->cfg.inductor_detection_times ) 
    {
        detection->tick_count = 0;
        detection->ls.is = detection->common_use.i_alpha;
        if(fabs(detection->ls.is - detection->ls.is_last) < 0.000001f) {
            return fail;
        }
        detection->result.rs = detection->common_use.ud_ref * 2 / 3 / detection->ls.is; 
        return success;
    }
    return running;
}

param_detection_status  Ld_detection_pro(params_detection_t *detection)
{
    detection->common_use.uq_ref = 0;
    detection->common_use.ud_ref = Ud_REF;
    if (detection->tick_count == 0) {
        detection->ls.is_last = detection->common_use.i_alpha;
    }
    detection->tick_count++;
    detection->cfg.inductor_detection_times = 4;
    if (detection->tick_count > detection->cfg.inductor_detection_times ) 
    {
        detection->tick_count = 0;
        detection->ls.is = detection->common_use.i_alpha;
        if(fabs(detection->ls.is- detection->ls.is_last) < 0.000001f) {
            return fail;
        }
        detection->result.ld = detection->common_use.ud_ref / ((detection->ls.is - detection->ls.is_last) /
                        ((detection->cfg.inductor_detection_times) * detection->cfg.detection_loop_ts));
        return success;
    }

    return running;
}

param_detection_status Lq_detection_pro(params_detection_t *detection)
{
    detection->common_use.ud_ref= 0;
    detection->common_use.uq_ref = Uq_REF;
    if (detection->tick_count == 0) {
        detection->ls.is_last = detection->common_use.i_beta;
    }
    detection->tick_count++;
    detection->cfg.inductor_detection_times = 4;
    if (detection->tick_count > detection->cfg.inductor_detection_times) {
        detection->tick_count = 0;
        detection->ls.is = detection->common_use.i_beta;
        detection->result.lq = detection->common_use.uq_ref / ((detection->ls.is - detection->ls.is_last) /
                        ((detection->cfg.inductor_detection_times ) * detection->cfg.detection_loop_ts));
        return success;
    }
    return running;
}




