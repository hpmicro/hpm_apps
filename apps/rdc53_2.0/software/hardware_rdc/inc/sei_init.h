#ifndef SEI_H
#define SEI_H

#include "board.h"
/**
 * @brief SEI API ;SEI接口函数
 * @addtogroup SEI_API
 * @{
 *
 */

/**
 * @brief    SEI structure
 *            ;SEI模块计算角度/速度结构体
 */
typedef struct _tag_pos_angle_cal_obj{
int32_t z;
int32_t ph;
int32_t maxph;
uint8_t motor_pole;
float elec_angle;
int32_t pos;
int32_t pos0;
float theta0;
uint32_t sei_abnormal_cnt;
}sei_angle_CalObj;

/**
 * @brief     sei config
 *            ;sei配置
 */
uint8_t sei_config(void);
/**
 * @brief     sei calculate angle
 *            ;sei模块获取位置计算角度
 */
void sei_angle(sei_angle_CalObj* sei_angleCalHdl);
/**
 * @brief     sei struct init
 *            ;sei结构体初始化
 */
void sei_params_init(sei_angle_CalObj* sei_angleCalHdl);

/** @} */

#endif