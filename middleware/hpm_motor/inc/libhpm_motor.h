/*
 * Copyright (c) 2023 hpmicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#ifndef HPM_MOTOR_H_
#define HPM_MOTOR_H_
#include <stdio.h>
#include"stdint.h"
#include <math.h>
#include <string.h>

/**
 * @brief CMD GENE API ;轨迹规划生成接口函数
 * @addtogroup CMD_GENE_API
 * @{
 *
 */

/***********************************************************************************************************************
 *
 * Definitions
 *
 **********************************************************************************************************************/

/**
 * @brief     trajectory planning output params
 *            ;轨迹规划输出参数
 */
typedef struct cmdgene_output_para
{
        /** @brief position command     
        *         ;位移指令 */
	double poscmd;
        /** @brief velocity command    
        *         ;速度指令 */
	double velcmd;
        /** @brief acceleration command  
        *         ;加速度指令 */
	double acccmd;
        /** @brief jerk command 
        *         ;加加速度指令 */
        double jerkcmd;
}CMDGENE_OUTPUT_PARA;


/**
 * @brief     position params for trajectory planning
 *            ;轨迹规划生成位置指令所需输入参数
 */
typedef struct cmdgene_poscfg_para{
        /** @brief start position    
        *         ;起始位置 */
        double  q0;
        /** @brief absolute move position    
        *         ;绝对运动位置 */
        double  q1;
        /** @brief start velocity    
        *         ;起始速度 */
        double  v0;
        /** @brief end velocity    
        *         ;结束速度 */
        double  v1;
        /** @brief max velocity    
        *         ;最大速度 */
        double  vmax;
        /** @brief max acceleration    
        *         ;最大加速度 */
        double  amax;
        /** @brief max jerk    
        *         ;最大加加速度 */
        double  jmax;
        /** @brief motion type    
        *         ;运动类型 */
        unsigned long cycletype;
        /** @brief cycle count    
        *         ;往返次数 */
	unsigned long cyclecnt;
        /** @brief dwell time ms    
        *         ;等待时间 unit ms */
	double dwelltime;
        /** @brief isr time s    
        *         ;中断时间 unit s */
        double isr_time_s;
}CMDGENE_POSCFG_PARA;

/**
 * @brief     velocity params for trajectory planning
 *            ;轨迹规划生成速度指令所需参数
 */
typedef struct cmdgene_velcfg_para{
        /** @brief start position    
        *         ;起始位置 */
        double  q0;
        /** @brief time spent at a constant speed    
        *         ;匀速时间 */
        double  Tv;
        /** @brief start velocity    
        *         ;起始速度 */
        double  v0;
        /** @brief end velocity    
        *         ;结束速度 */
        double  v1;
        /** @brief max velocity    
        *         ;最大速度 */
        double  vmax;
        /** @brief max acceleration    
        *         ;最大加速度 */
        double  amax;
        /** @brief max jerk    
        *         ;最大加加速度 */
        double  jmax;
                /** @brief isr time s    
        *         ;中断时间 unit s */
        double isr_time_s;
}CMDGENE_VELCFG_PARA;

/**
 * @brief     input params for trajectory planning
 *            ;轨迹规划所需输入参数
 */
typedef struct cmdgene_input_para
{
        /** @brief position cmd generation config    
        *         ;指令生成位置配置 */
	CMDGENE_POSCFG_PARA poscfgpar;
        /** @brief velocity cmd generation config    
        *         ;指令生成速度配置 */
        CMDGENE_VELCFG_PARA velcfgpar;

}CMDGENE_INPUT_PARA;

/**
 * @brief     user params for trajectory planning
 *            Note: temp variables during trajectory planning, user does not need to care
 *            ;轨迹规划中间变量
 *            注意：该结构体是轨迹生成中间变量，用户无需操作。
 */

typedef struct cmdgene_user_para
{
        double Ta;
        double Tv;
        double Td;
        double Tj1;
        double Tj2;
        double q_0;
        double q_1;
        double v_0;
        double v_1;
        double vlim;
        double a_max;
        double a_min;
        double a_lima;
        double a_limd;
        double j_max;
        double j_min;
        double t;
        double t_old;
        double t_total;
        double t_timer;
        double sigma;
        double sigma1;
        unsigned long halfcycleCnt;
        unsigned long cycletype;
        unsigned long cyclecnt;
        unsigned long dwelltime;
}CMDGENE_USER_PARA;

/**
 * @brief     struct for trajectory planning
 *            ;轨迹规划结构体
 */
typedef struct cmdgene_para{
        /** @brief input params    
        *         ;轨迹规划输入参数 */
       CMDGENE_INPUT_PARA  cmdgene_in_par;
       /** @brief user params    
        *         ;轨迹规划中间变量 */
       CMDGENE_USER_PARA   cmdgene_user_par;
       /** @brief output params    
        *         ;轨迹规划输出参数 */
       CMDGENE_OUTPUT_PARA cmdgene_out_par;
}CMDGENE_PARA;

/**
 * @brief     motion type enumeration
 *            ;运动类型枚举
 */
typedef  enum
{
        /** @brief infinite move    
        *         ;连续运动 */
        infinitemove,
        /** @brief single move    
        *         ;单次运动 */
	singlemove,
        /** @brief finite move    
        *         ;多次运动 */
	finitemove,
	
}motiontype_t;

/**
 * @brief           reset temp variable 
 *                  ;重置轨迹规划中用到的中间变量
 * @param[in]   userpar   user struct to operate on;中间变量
 */
void cmd_gene_disable( CMDGENE_USER_PARA* userpar);

/**
 * @brief           Generate Cmd Output with specific end position
 *                  ;位置指令规划
 * @param[in]   par   struct to operate on;轨迹规划结构体
 */
void pos_cmd_gene(CMDGENE_PARA* par);

/**
 * @brief           Generate Cmd Output without specific end position
 *                  ;速度指令规划函数
 * @param[in]   par   struct to operate on;轨迹规划结构体
 */
void vel_cmd_gene(CMDGENE_PARA* par);


/**
 * @brief           get lib version
 *                  ;获取lib库版本
 * @return       lib version;版本号
 */
uint8_t lib_cmd_version_get(void);


/** @} */

#endif //__HPM_MOTOR_H




