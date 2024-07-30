
# 使用说明

## 依赖HPM SDK版本要求
(hpm_app_instruction_zh)=

HPM APP版本和HPM SDK版本一一对应，为了保持版本兼容，版本号必须相同。
例如：
| 软件名称 | 版本号 |
|--- | --- |
| hpm_sdk | 1.4.0 |
| hpm_apps | 1.4.0 |

## 安装依赖
- Note1:将hpm_apps和hpm_sdk放到同一级目录下:
![hpm_app_dir](../assets/hpm_app_dir.png)

- Note2:需要修改hpm_sdk目录下CMakeLists.txt文件，将hpm_apps添加为工程子目录(如hpm_sdk已添加，则忽略):
![hpm_app_setup](../assets/hpm_app_setup.png)
路径：hpm_sdk/CMakeLists.txt
    ```shell
    add_subdirectory(../hpm_apps ../hpm_apps/build_tmp)
    ```

## 构建编译
- Windows:
    Windows下使用env环境，可使用GUI工具构建工程，使用IDE SEGGER Embedded Studio for RISV-v编译调试工程。
    - Windows GUI工具构建工程:
        
        双击运行GUI工具

        ![start_gui](../assets/start_gui.png)

        使能 User Application Path:选择要构建的工程路径构建即可。
        (如果使用私有的board，使能 User Board Path)

        ![gui_help_start](../assets/gui_help_start.png)

        使用SES编译调试
        ![ses_debug](../assets/segger_build.png)


- Linux:
    Linux下需添加HPM_SDK_BASE路径到环境变量，手动添加或通过运行命令添加。
    
    1.通过运行命令添加HPM_SDK_BASE路径到环境变量:
    ```shell
      cd hpm_sdk/
      source env.sh
    ```
    2.cd到需要构建编译的apps目录下，使用命令构建编译。
    如下：
    ```shell
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
