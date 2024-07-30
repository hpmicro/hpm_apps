/*
 * Copyright (c) 2023 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */
#include "wr_eeprom.h"


#define BOARD_APP_I2C2                HPM_I2C2_BASE
#define BOARD_I2C2_CLOCK_NAME         clock_i2c2

#define AT24C16_WRITE_ADDR            0xA0
#define AT24C16_READ_ADDR             0xA1

#define SCL(a)                        gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOB, 8, a)
#define SDA_OUT(b)                    gpio_write_pin(HPM_GPIO0, GPIO_DO_GPIOB, 9, b)
#define SDA_IN                        gpio_read_pin(HPM_GPIO0, GPIO_DI_GPIOB, 9)


void iic_SDA_OutMode(void)
{
    gpio_set_pin_output(HPM_GPIO0, GPIO_DO_GPIOB,9);
}

void iic_SDA_InputMode(void)
{
    gpio_set_pin_input(HPM_GPIO0, GPIO_DI_GPIOB,9);
}
 void iic_start(void)
{
    iic_SDA_OutMode();
    SDA_OUT(1);     
    SCL(1);       
    board_delay_us(4);        
    SDA_OUT(0);     
    board_delay_us(4);         
    SCL(0);        
}

void iic_stop(void)
{
    iic_SDA_OutMode();
    SDA_OUT(0);     
    SCL(0);         
    board_delay_us(4);            
    SCL(1);       
    board_delay_us(4);            
    SDA_OUT(1);    
}

uint8_t iic_GetAck(void)
{
    uint8_t cnt=0;
    iic_SDA_InputMode();
    SDA_OUT(1);     
    board_delay_us(2);         
    SCL(0);          
    board_delay_us(2);         
    SCL(1);          
    while(SDA_IN)   
    {
      cnt++;
      if(cnt>250)return 1;
    }
    SCL(0);         
    return 0;
}

void iic_SendAck(uint8_t stat)
{
    iic_SDA_OutMode(); 
    SCL(0);         
    if(stat)SDA_OUT(1);  
    else SDA_OUT(0);     
    board_delay_us(2);            
    SCL(1);             
    board_delay_us(2);            
    SCL(0);            
}


void iic_WriteOneByteData(uint8_t data)
{
    uint8_t i;
    iic_SDA_OutMode(); 
    SCL(0);         
    for(i=0;i<8;i++)
    {
      if(data&0x80)SDA_OUT(1); 
      else SDA_OUT(0);    
      SCL(1);             
      board_delay_us(2);            
      SCL(0);             
      board_delay_us(2);           
      data<<=1;              
    }
}

uint8_t iic_ReadOneByteData(void)
{
    uint8_t i,data;
    iic_SDA_InputMode();
    for(i=0;i<8;i++)
    {
      SCL(0);          
      board_delay_us(2);         
      SCL(1);          
      data<<=1;           
      if(SDA_IN)data|=0x01;
      board_delay_us(2);         
    }
      SCL(0);             
      return data;
}


uint8_t At24c16ReadOneByteData(uint32_t addr)
{
    uint8_t data;
    iic_start();  
    iic_WriteOneByteData(AT24C16_WRITE_ADDR);
    iic_GetAck();
    iic_WriteOneByteData(addr); 
    iic_GetAck();
 
    iic_start();  
    iic_WriteOneByteData(AT24C16_READ_ADDR); 
    iic_GetAck();
    data=iic_ReadOneByteData(); 
    iic_SendAck(1); 
    iic_stop(); 
    return data;
}
 
 

void At24c16WriteOneByteData(uint32_t addr,uint8_t data)
{
    iic_start(); 
    iic_WriteOneByteData(AT24C16_WRITE_ADDR); 
    iic_GetAck();
    iic_WriteOneByteData(addr); 
    iic_GetAck();
    iic_WriteOneByteData(data); 
    iic_GetAck();
    iic_stop();  
    board_delay_ms(10); 
}

void AT24C16_ReadData(uint16_t addr,uint16_t len,uint8_t *p)
{
    uint16_t i;
    uint8_t page_addr,page_addr_H,Devicce_Write_Addr,Devicce_Read_Addr;
    page_addr=addr>>4;
    page_addr_H=page_addr>>5;
    Devicce_Write_Addr=AT24C16_WRITE_ADDR + (page_addr_H<<1);
    Devicce_Read_Addr=AT24C16_READ_ADDR+(page_addr_H<<1);
    iic_start();
    iic_WriteOneByteData(Devicce_Write_Addr); 
    iic_GetAck();
    iic_WriteOneByteData(addr&0xFF);
    iic_GetAck(); 
    iic_start();
    iic_WriteOneByteData(Devicce_Read_Addr); 
    iic_GetAck(); 
 
    for(i=0;i<len;i++)
    {
        p[i]=iic_ReadOneByteData(); 
        iic_SendAck(0);  
    }
    iic_SendAck(1);  
    iic_stop();      
} 
 

 void AT24C16_PageWrite(uint16_t addr,uint16_t len,uint8_t *p)
{
      uint8_t i;
      uint8_t page_addr,page_addr_H,Devicce_Write_Addr,Devicce_Read_Addr;
      page_addr=addr>>4;
      page_addr_H=page_addr>>5;
      Devicce_Write_Addr=AT24C16_WRITE_ADDR + (page_addr_H<<1);
      Devicce_Read_Addr=AT24C16_READ_ADDR+(page_addr_H<<1);
      iic_start();
      iic_WriteOneByteData(Devicce_Write_Addr); 
      iic_GetAck(); 
      iic_WriteOneByteData(addr&0xFF);  
      iic_GetAck(); 
      for(i=0;i<len;i++)
      {
          iic_WriteOneByteData(p[i]);  
          iic_GetAck(); 
      }
      iic_stop();   
      board_delay_ms(10);  
}

void AT24C16_WriteData(uint16_t addr,uint16_t len,uint8_t *p)
{
      uint8_t page_remain=16-addr%16; 
      if(page_remain>=len)
      {
          page_remain=len;
      }
      while(1)
      {
          AT24C16_PageWrite(addr,page_remain,p);
          if(page_remain==len)break;
          addr+=page_remain;
          p+=page_remain;
          len-=page_remain;
          if(len>=16)page_remain=16;
          else page_remain=len;
      }
}
 


void iic_pin_cfg(void)
{
      HPM_PIOC->PAD[IOC_PAD_PB08].FUNC_CTL = IOC_PB08_FUNC_CTL_GPIO_B_08;
      HPM_PIOC->PAD[IOC_PAD_PB09].FUNC_CTL = IOC_PB09_FUNC_CTL_GPIO_B_09;
      gpio_set_pin_output(HPM_GPIO0, GPIO_DO_GPIOB,8);
}

void parmStoreAll(void)
{  
     int32_t len = sizeof(globalAxisParm[0].parmMotor)+ sizeof(globalAxisParm[0].parmAssist)+ sizeof(globalAxisParm[0].parmCurrent)+ sizeof(globalAxisParm[0].parmPos)+ sizeof(globalAxisParm[0].parmVel);//sizeof(globalAxisParm[0]);
     AT24C16_WriteData(0,len,&globalAxisParm[0]);
}
void parmReadAll(uint8_t* address)
{
    int32_t len  = sizeof(globalAxisParm[0].parmMotor)+ sizeof(globalAxisParm[0].parmAssist)+ sizeof(globalAxisParm[0].parmCurrent)+ sizeof(globalAxisParm[0].parmPos)+ sizeof(globalAxisParm[0].parmVel);//sizeof(globalAxisParm[0]);
    AT24C16_ReadData(0,len,address);
}

