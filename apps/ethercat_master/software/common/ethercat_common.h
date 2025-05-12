#ifndef ETHERCAT_COMMON_H
#define ETHERCAT_COMMON_H

#include "board.h"
#include "ethercat.h"
#include "hpm_gptmr_drv.h"

typedef void (*esc_sdo_config_t)(void);

extern volatile bool esc_enter_op;
extern volatile bool esc_pdo_error;

int ethercat_common_start(esc_sdo_config_t sdo_config);
void ethercat_common_stop(void);

#endif