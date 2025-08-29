#ifndef _QEI_INIT_H
#define _QEI_INIT_H

#include "board.h"

/**
 * @brief QEI API ;QEI接口函数
 * @addtogroup QEI_API
 * @{
 *
 */

 /**
 * @brief     qei parameter 
 *            ;qei模块计算角度/速度结构体
 */
typedef struct _qeiv2_Obj 
{
   float    ele_angle;
   int32_t  z;
   int32_t  ph;
   uint32_t  get_pos;
   float   speed;
   uint32_t angle;
}qeiv2_Obj;

/**
 * @brief     qei params init
 *            ;qei参数初始化
 */
void qei_init(void);

/**
 * @brief     qei calculate theta/speed
 *            ;qei模块计算速度/角度
 */
void qeiv2_theta_cal(qeiv2_Obj* qeiv2Hdl);

/** @} */

#endif //_QEI_INIT_H