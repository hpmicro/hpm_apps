#ifndef __DATA_COMMON_H
#define __DATA_COMMON_H

#include <stdint.h>
#include "cia402_def.h"

#define MODE_CSP_MIN                 (0)
#define MODE_CSP_MAX                 (2000000)

#define MODE_CSV_MIN                 (0)
#define MODE_CSV_MAX                 (100)

#define MODE_POSITION_SIZE           (1000)

#define MODE_STEP_SIZE               (30)

#define MOTOR_EVENT_START            0
#define MOTOR_EVENT_STOP             1
#define MOTOR_EVENT_FORWARD          2
#define MOTOR_EVENT_BACKWARD         3
#define MOTOR_EVENT_RESET            4

#define ETHERCAT_MOTOR_CYCLE_TIME_US (1000)

enum {
    MODE_CSP = 0,
    MODE_CSV,
    MODE_CSP_CSV,
    MODE_NONE,
};

enum {
    DIR_FORWORD = 0,
    DIR_BACKWORD,
};

typedef struct {
    uint32_t dev_count;
    uint32_t cur_index;
    uint8_t mode;
    uint8_t dir;
    uint32_t position;
    uint32_t speed;
    union {
        struct {
            TCiA402PDO1A01 input;
            TCiA402PDO1601 output;

        } cia402_csp;
        struct {
            TCiA402PDO1A02 input;
            TCiA402PDO1602 output;
        } cia402_csv;

        struct {
            TCiA402PDO1A00 input;
            TCiA402PDO1600 output;
        } cia402_csp_csv;
    };
} data_pack_t;

#endif //__DATA_COMMON_H