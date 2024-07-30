[English](README.md)

# HPM_APPS 概述
HPM_APPS是基于HPM SDK开发的上层应用软件开发套件。支持各类系统级闭环方案，包含了中间件、组件、服务等，供用户使用评估。

## HPM_APPS 目录结构

| 目录名称 | 描述 |
|--------|--------|
| <HPM_APP_BASE>/boards | 各类方案板级文件 |
| <HPM_APP_BASE>/components | 软件组件 |
| <HPM_APP_BASE>/docs | 文档 |
| <HPM_APP_BASE>/middleware | 中间件 |
| <HPM_APP_BASE>/apps | 各类方案实例代码 |
| <HPM_APP_BASE>/services | 软件服务 |
| <HPM_APP_BASE>/tools | 工具类文件 |

## HPM_APPS使用说明

### 依赖HPM SDK版本要求

HPM APP版本和HPM SDK版本一一对应，为了保持版本兼容，版本号必须相同。
例如：
| 软件名称 | 版本号 |
|--- | --- |
| hpm_sdk | 1.4.0 |
| hpm_apps | 1.4.0 |


### 安装依赖
- Note1:将hpm_apps和hpm_sdk放到同一级目录下:
![hpm_app_dir](docs/assets/hpm_app_dir.png)

- Note2:需要修改hpm_sdk目录下CMakeLists.txt文件，将hpm_apps添加为工程子目录:
![hpm_app_setup](docs/assets/hpm_app_setup.png)
路径：hpm_sdk/CMakeLists.txt
    ```shell
    add_subdirectory(../hpm_apps ../hpm_apps/build_tmp)
    ```

### 构建编译
- Windows:
    Windows下使用env环境，只支持命令行构建工程，使用SES编译调试工程。
    - Windows命令行:
    首先双击运行env环境：start_cmd.cmd命令窗口。
    cd到需要构建的apps目录下，使用generate_project命令构建工程。
    如下：
    ```shell
    cd hpm_apps\apps\mp_adapte\power_core2
    generate_project -b hpm6200power -x D:\workspace\hpm_apps\boards -f -t flash_xip
    ```
    其中：
    | generate_project 选项 | 说明 |
    |--- | --- |
    | -b | 构建工程所使用的board |
    | -x | 使用board的绝对路径 |
    | -f | 删除历史构建工程 |
    | -t | 工程构建类型 debug/release/flash_xip/flash_sdram_xip 等 |

- Linux:
    Linux下需添加HPM_SDK_BASE路径到环境变量，手动添加或通过运行命令添加。
    
    1.通过运行命令添加HPM_SDK_BASE路径到环境变量:
    ```shell
      cd hpm_sdk/
      source env.sh
    ```
    2.cd到需要构建编译的apps目录下，使用命令构建编译成功。
    如下：
    ```c
    //切换到示例应用目录
    cd hpm_apps\apps\mp_adapte\power_core2
    //创建build目录
    mkdir build
    //切换目录到"build"
    cd build
    //Ninja-build产生构建文件
    cmake -GNinja -DBOARD_SEARCH_PATH=/home/work/workspace/hpm_apps/boards -DBOARD=hpm6200power -DCMAKE_BUILD_TYPE=flash_xip ..
    //编译
    ninja
    ```
    当编译完成后,生成的elf以及对应的其他文件可以在output目录中找到。

## HPM_APPS文档
- 本地文档:
  HPM_APPS文档可以进行本地编译，成功编译之后可以通过以下入口访问本地文档:
  - <HPM_APP_BASE>>/docs/index.html
  - <HPM_APP_BASE>>/docs/index_zh.html

- 在线文档:
  

## 代码仓库
- hpm_apps github: https://github.com/hpmicro/hpm_apps
- hpm_sdk github: https://github.com/hpmicro/hpm_sdk
- sdk_env github: https://github.com/hpmicro/sdk_env
- sdk extra demo: https://github.com/hpmicro/hpm_sdk_extra
- openocd (hpmicro patched): https://github.com/hpmicro/riscv-openocd
- gnu gcc toolchain: https://github.com/hpmicro/riscv-gnu-toolchain

> 仓库在gitee上有对应的镜像。
