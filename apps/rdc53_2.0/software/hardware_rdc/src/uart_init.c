/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */


#include "hpm_uart_drv.h"
#include "uart_init.h"
#include "rdc.h"

uint8_t rx_buffer[1];
int8_t pt_send_uart[5];
/* uart 4M*/
void uart_pins_init(void)
{
    HPM_IOC->PAD[IOC_PAD_PA23].FUNC_CTL = IOC_PA23_FUNC_CTL_UART5_TXD;
    HPM_IOC->PAD[IOC_PAD_PA21].FUNC_CTL = IOC_PA21_FUNC_CTL_UART5_DE;
    HPM_IOC->PAD[IOC_PAD_PA22].FUNC_CTL = IOC_PA22_FUNC_CTL_UART5_RXD;
    uint32_t freq = 0U;
    clock_set_source_divider(clock_uart5, clk_src_pll1_clk0, 2);
    clock_add_to_group(clock_uart5, 0);
    freq = clock_get_frequency(clock_uart5); 
}
void uart_cfg(void)
{
    uart_pins_init();
    hpm_stat_t stat;
    uart_config_t config = {0};   
    uart_default_config(BOARD_UART, &config);
    
    config.fifo_enable = true;
    config.dma_enable = true;
    config.src_freq_in_hz = clock_get_frequency(BOARD_UART_CLOCK);
    
    config.tx_fifo_level = uart_tx_fifo_trg_not_full;
    config.rx_fifo_level = uart_rx_fifo_trg_not_empty;
    
    stat = uart_init(BOARD_UART, &config);
    uart_set_baudrate(BOARD_UART,4000000,config.src_freq_in_hz);
}
void  uart_data_package(void)
{
     memset(&pt_send_uart,0,sizeof(pt_send_uart)) ;
    uint16_t len =  0;
    uint16_t theta_rdc = rdcObj.rdc_theta*100;
    int16_t pll_vel = rdcObj.pll_vel*100;
    
    pt_send_uart[0]= (int8_t)(pll_vel & 0x00ff);
    pt_send_uart[1]= (int8_t)((pll_vel & 0xff00) >> 8);
    pt_send_uart[2]= (uint8_t)(theta_rdc & 0x00ff);
    pt_send_uart[3]= (uint8_t)((theta_rdc & 0xff00) >> 8);
    pt_send_uart[4]= pt_send_uart[0]^pt_send_uart[1]^pt_send_uart[2]^pt_send_uart[3];
}
/*package data */
void sendDataToPC(void) 
{
    uart_data_package();       
    uart_send_data(HPM_UART5, (uint8_t*)pt_send_uart,5);         
}

/*uart write/read*/
void uart_trans_pro (void)
{
    static DataTrans DataTransStatus = UART_READ;
        
   switch(DataTransStatus)
  {
	case UART_WRITE:                        
              sendDataToPC();
              memset(&rx_buffer,0,sizeof(rx_buffer)) ;
              DataTransStatus = UART_READ ;
              break ;

        case UART_READ:
             hpm_stat_t status = uart_receive_data(BOARD_UART,&rx_buffer , 1);
             if(status == status_success)
             {
                  if(rx_buffer[0] == 0x8A)                                 
                  {
                        DataTransStatus = UART_WRITE ;
                  }
            }
            break ;
	}
}

