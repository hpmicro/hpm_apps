# hpm_monitor服务使用例程

## 依赖SDK1.10.0

## 介绍

hpm_monitor 是一个高效的、易用的、可移植性高的服务，用来实时查看和设置当前设备中的全局变量，或者用来高速(1Khz-1ms)上报全局变量；常常被用来当做监控数据示波器使用；对电机、电源等调试非常友好；

注意：hpm_monitor服务 需搭配PC上位机HPMicroMonitorStudio工具使用；

HPMicroMonitorStudio从先楫半导体官网获取下载。

为了方便用户使用、了解hpm_monitor服务，本文为hpm_monitor服务使用的demo。

本例程软件实现了4中波形：三角波、正弦波、方波、锯齿波；可通过hpm_monitor服务搭配上位机实时查看此波形，同时也可以设置下发修改波形数据。

[hpm_monitor服务说明](hpm_monitor_instruction_zh)

## hpm_monitor服务启用

- cmakelist.txt使能hpm_monitor服务
set(CONFIG_A_HPMMONITOR 1) 使能hpm_monitor服务
set(CONFIG_MONITOR_INTERFACE "uart") 使用UART通道
set(CONFIG_MONITOR_INTERFACE "usb") 使用USB通道

``` c
set(CONFIG_A_HPMMONITOR 1)
set(CONFIG_MONITOR_INTERFACE "uart")
# set(CONFIG_MONITOR_INTERFACE "usb")

if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")

elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    set(CONFIG_CHERRYUSB 1)
    set(CONFIG_USB_DEVICE 1)
    set(CONFIG_USB_DEVICE_CDC 1)
endif()

find_package(hpm-sdk REQUIRED HINTS $ENV{HPM_SDK_BASE})


if("${CONFIG_MONITOR_INTERFACE}" STREQUAL "uart")
    sdk_compile_definitions("-DCONFIG_UART_CHANNEL=1")
elseif("${CONFIG_MONITOR_INTERFACE}" STREQUAL "usb")
    sdk_compile_definitions("-DCONFIG_USB_CHANNEL=1")
endif()

```
- 调用初始化monitor_init()和轮询monitor_handle()，即可完成服务启用;
```c
int main(void)
{
    uint64_t time = 0;
    board_init();
    printf("general debug demo!\r\n");
    printf("__DATE__:%s, __TIME__:%s\r\n", __DATE__, __TIME__);

    monitor_init();

    while (1)
    {
        if(clock_get_now_tick_ms() - time >= 10)
        {
            time = clock_get_now_tick_ms();
            triangule_wave_handle();
            sine_wave_handle();
            square_ware_handle();
            sawtooth_ware_handle();
        }
        monitor_handle();
    }
    return 0;
}
```

- monitor_config.h 配置
注意：
 1.建议增大UART通信波特率(2M及以上)。低速率通信波特率在高速率上报频率下，会导致概率上报失败或设置失败；
 2.针对电机、电源高实时性要求，避免其它中断影响电机、电源电流环等运行，建议关闭monitor通信通道中断，如USB使用轮询方式(使能#define CONFIG_USB_POLLING_ENABLE)；
```c
//PID VID配置
#define MONITOR_PID                  (0xFFFF)
#define MONITOR_VID                  (0x34B7) /* HPMicro VID */

//协议最大数据包设置
#define MONITOR_PROFILE_MAXSIZE      (512)

//打印日志等级设置
#define CONFIG_MONITOR_DBG_LEVEL MONITOR_DBG_INFO

#if defined(CONFIG_UART_CHANNEL) && CONFIG_UART_CHANNEL
//UART通道相关配置
...
#endif

#if defined(CONFIG_USB_CHANNEL) && CONFIG_USB_CHANNEL
//USB通道 相关配置
//使能USB 轮询模式，关闭USB中断，避免影响主程序高实时性外设
#define CONFIG_USB_POLLING_ENABLE
...
#endif
```

## 工程路径

- 工程路径：hpm_monitor/software

## 工程配置

- 无

## 工程构建

- 当前工程支持HPM 全系列MCU及board，用户根据需求选择对应的board即可。
- 工程构建类型任意都可以。
- 如下windows构建工程。
![hpm_monitor_build1](doc/api/assets/hpm_monitor_build1.png)


## 工程运行

注:HPMicro_Monitor_Studio详细操作文档请查看HPMicro_Monitor_Studio帮助文档；本文只按照运行步骤介绍基础功能;
1. 烧录运行工程
2. 根据选择的通信通道，设备(EVK)连接电脑(UART/USB)
3. PC双击运行HPMicro_Monitor_Studio工具
4. 设备连接 (设备界面)
   ![hpm_monitor_run2](doc/api/assets/hpm_monitor_run2.png)

   ![hpm_monitor_run1](doc/api/assets/hpm_monitor_run1.png)
5. elf文件打开 (文件界面)
   ![hpm_monitor_run3](doc/api/assets/hpm_monitor_run3.png)

6. 参数文件创建 (文件界面)
   ![hpm_monitor_run4](doc/api/assets/hpm_monitor_run4.png)

   ![hpm_monitor_run5](doc/api/assets/hpm_monitor_run5.png)

7. 参数设置(参数界面)
   打开参数文件
   ![hpm_monitor_run6](doc/api/assets/hpm_monitor_run6.png)

   ![hpm_monitor_run7](doc/api/assets/hpm_monitor_run7.png)

   ![hpm_monitor_run8](doc/api/assets/hpm_monitor_run8.png)

   此时会列出当前固件中所有的全局变量；
   
8. 界面设置(参数界面)
  创建主菜单和子菜单(菜单名称可任意命名)。
  例如：多轴电机可根据需求设置。如下3D打印。
  ![hpm_monitor_run20](doc/api/assets/hpm_monitor_run20.png)

  ![hpm_monitor_run9](doc/api/assets/hpm_monitor_run9.png)

9. 参数选择添加(参数界面)
   选择需要监控或设置的全局变量，并根据需求选择合适的参数控件。
   ![hpm_monitor_run10](doc/api/assets/hpm_monitor_run10.png)

   注：如需变量观测波形，需将对应变量添加到示波器窗口。
   ![hpm_monitor_run11](doc/api/assets/hpm_monitor_run11.png)

10. 数据监控(监控界面)
   使能可用的设备连接monitor通信;
   ![hpm_monitor_run12](doc/api/assets/hpm_monitor_run12.png)

   ![hpm_monitor_run13](doc/api/assets/hpm_monitor_run13.png)

   可设置上报频率和示波器窗口自动显示;
   ![hpm_monitor_run14](doc/api/assets/hpm_monitor_run14.png)

11. 数据下发(监控界面)
   子菜单里面的参数根据需要可设置下发修改当前参数值，如下方波修改后的波形显示及修改成功弹窗提醒；
   ![hpm_monitor_run15](doc/api/assets/hpm_monitor_run15.png)

12. 波形效果(监控界面)
   使能或禁用对应的波形显示；
   正弦波:
   ![hpm_monitor_run16](doc/api/assets/hpm_monitor_run16.png)

   三角波:
   ![hpm_monitor_run17](doc/api/assets/hpm_monitor_run17.png)

   方波:
   ![hpm_monitor_run18](doc/api/assets/hpm_monitor_run18.png)

   锯齿波:
   ![hpm_monitor_run19](doc/api/assets/hpm_monitor_run19.png)


## API

:::{eval-rst}

关于软件API 请查看 `方案API 文档 <../../_static/apps/hpm_monitor/html/index.html>`_ 。
:::