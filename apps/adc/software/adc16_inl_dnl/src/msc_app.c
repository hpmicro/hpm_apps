/*
 * Copyright (c) 2021 HPMicro
 *
 * SPDX-License-Identifier: BSD-3-Clause
 *
 */

#include "hpm_common.h"
#include "tusb.h"
#include "diskio.h"
#include "ff.h"
#include "file_op.h"
#include "adc.h"
#include "board.h"
#include "hpm_usb_drv.h"

ATTR_PLACE_AT_NONCACHEABLE_INIT char wbuff[50] = {"USB Host MSC FatFs Demo!"};
ATTR_PLACE_AT_NONCACHEABLE_INIT char rbuff[50] = {0};

#define  ADC_DATA_SIZE          0x2000000//128MB
#define  ADC_ONCE_WRITE_SIZE    0x10000
volatile uint32_t adcbuffer[ADC_ONCE_WRITE_SIZE];
uint32_t *adc_seq_buff;

#if CFG_TUH_MSC
ATTR_PLACE_AT_NONCACHEABLE static scsi_inquiry_resp_t inquiry_resp;
ATTR_PLACE_AT_NONCACHEABLE FATFS fatfs[CFG_TUSB_HOST_DEVICE_MAX];
ATTR_PLACE_AT_NONCACHEABLE FIL file;

FRESULT set_timestamp (
    char *obj,   /* Pointer to the file name */
    int year,
    int month,
    int mday,
    int hour,
    int min,
    int sec
)
{
    FILINFO fno;

    fno.fdate = (WORD)(((year - 1980) * 512U) | month * 32U | mday);
    fno.ftime = (WORD)(hour * 2048U | min * 32U | sec / 2U);

    return f_utime(obj, &fno);
}

/*---------------------------------------------------------------------*
 * Function Prototyes
 *---------------------------------------------------------------------*/
void led_set_blinking_mounted_interval(void);
void led_set_blinking_unmounted_interval(void);

/**
 * @brief 文件系统装载;File system mount
 *
 * @param [in] dev_addr 设备地址;Device addresss
 * @retval 文件系统装载成功或失败;Result(ture or false) for the file system mount
 */
bool file_system_mount(uint8_t dev_addr)
{
    char *filename = {"USBHost.txt"};
    char logic_drv_num[10] = {0};
    uint8_t phy_disk = dev_addr - 1;
    unsigned int cnt = 0;
    uint32_t seq_rdptr;
    int i,j,u,k;
    uint32_t cycle;
    uint32_t usb_irqen;

    /* file system */
    disk_initialize(phy_disk);

    if (disk_is_ready(phy_disk)) {
        sprintf(logic_drv_num, "%d:", phy_disk);

        if (f_mount(&fatfs[phy_disk], logic_drv_num, 0) != FR_OK)
        {
            printf("FatFs mount failed!\n");
            return false;
        } else {
            printf("FatFs mount succeeded!\n");
        }

        f_chdrive(logic_drv_num); /* change to newly mounted drive */
        f_chdir("/");             /* set root directory as current directory */

        if(f_open(&file, filename, FA_OPEN_ALWAYS | FA_WRITE | FA_READ) != FR_OK) {
            printf("Can't Open the %s file!\n", filename);
        } else {
            printf("The %s is open.\n", filename);    
            
            adc_init();

            adc_seq_buff = adc_dma_buffer_get();
            
            trigger_init(BOARD_APP_ADC_TRIG_PWM,664);

            trigger_start(BOARD_APP_ADC_TRIG_PWM);
                 
            //write adc data to file
            cycle = 1;
            
            for(j = 0;j < ADC_DATA_SIZE/ADC_ONCE_WRITE_SIZE; j++)
            {      
                seq_rdptr = 0;
                while(seq_rdptr < ADC_ONCE_WRITE_SIZE)
                {
                  if(cycle) 
                  {
                      while((adc_seq_buff[BOARD_ADC_DMA_BUFFER_LEN >> 1] & 0x80000000) == 0);
                  }          
                  else 
                  {
                      while((adc_seq_buff[BOARD_ADC_DMA_BUFFER_LEN >> 1] & 0x80000000) == 0x80000000);
                  }

                  for(i = 0;i < (BOARD_ADC_DMA_BUFFER_LEN >> 1); i++)
                  {
                      adcbuffer[seq_rdptr+i] = adc_seq_buff[i]; 
                  }

                  if(cycle) 
                  {
                      while((adc_seq_buff[0] & 0x80000000) == 0x80000000);
                  }             
                  else
                  {
                      while((adc_seq_buff[0] & 0x80000000) == 0x00000000);
                  }
            
                  for(i = 0;i < (BOARD_ADC_DMA_BUFFER_LEN >> 1); i++) 
                    adcbuffer[seq_rdptr + (BOARD_ADC_DMA_BUFFER_LEN >> 1) + i] = adc_seq_buff[(BOARD_ADC_DMA_BUFFER_LEN >> 1) + i];


                  seq_rdptr += BOARD_ADC_DMA_BUFFER_LEN;
                  cycle = 1-cycle;
                }

                f_write(&file, adcbuffer, sizeof(adcbuffer), &cnt);
                printf("write udisk...\n\r");           
            }
            
            printf("Write the %s file done with %xsamples!\n", filename,seq_rdptr+j);
            f_close(&file);        
            if(f_open(&file, filename, FA_READ) == FR_OK) {
                f_read(&file, rbuff, strlen(wbuff), &cnt);
                set_timestamp(filename, 2022, 9, 20, 13, 37, 30);
                f_close(&file);
                f_scan("/");
            }
        }
    }

    return true;
}

/**
 * @brief 设备询问回调;The calllback function for the device inquiry
 *
 * @param [in] dev_addr 设备地址;Device addresss
 * @param [in] cbw 命令块;Command block wrapper
 * @param [in] csw 状态块;Command status wrapper
 * @retval 设备问询结果;The result(ture or false) for the device inquiry
 */
bool inquiry_complete_cb(uint8_t dev_addr, tuh_msc_complete_data_t const *cb_data)
{
    uint32_t block_count;
    uint32_t block_size;

    if (cb_data->csw->status != 0) {
        printf("Inquiry failed\r\n");
        return false;
    }

    /* Print out Vendor ID, Product ID and Rev */
    printf("%.8s %.16s rev %.4s\r\n", inquiry_resp.vendor_id, inquiry_resp.product_id, inquiry_resp.product_rev);

    /* Get capacity of device */
    block_count = tuh_msc_get_block_count(dev_addr, cb_data->cbw->lun);
    block_size = tuh_msc_get_block_size(dev_addr, cb_data->cbw->lun);

    printf("Disk Size: %lu MB\r\n", block_count / ((1024*1024)/block_size));
    printf("Block Count = %lu, Block Size: %lu\r\n", block_count, block_size);

    file_system_mount(dev_addr);

    return true;
}

/**
 * @brief 存储设备装载回调;The calllback function for the msc mount
 *
 * @param [in] dev_addr 设备地址;Device addresss
 */
void tuh_msc_mount_cb(uint8_t dev_addr)
{
    uint8_t const lun = 0;
    printf("\nA MassStorage device is mounted.\r\n");
    led_set_blinking_mounted_interval();
    tuh_msc_inquiry(dev_addr, lun, &inquiry_resp, inquiry_complete_cb,(uintptr_t)NULL);
}

/**
 * @brief 存储设备卸载回调;The calllback function for the msc unmount
 *
 * @param [in] dev_addr 设备地址;Device addresss
 */
void tuh_msc_unmount_cb(uint8_t dev_addr)
{
    uint8_t phy_disk = dev_addr-1;
    char logic_drv_num[10] = {0};

    printf("A MassStorage device is unmounted.\r\n");

    led_set_blinking_unmounted_interval();

    sprintf(logic_drv_num, "%d", phy_disk);
    if ( FR_OK == f_unmount(logic_drv_num)) {
        printf("FatFs unmount succeeded!\n");
        disk_deinitialize(phy_disk);
    }
}

#endif /* End of CFG_TUH_MSC */
