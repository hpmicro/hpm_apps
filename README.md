[中文](README_zh.md)

# Introduction

## HPM APPS introduction
HPM APPS is an upper layer application software development kit based on HPM SDK. It provides a variety of typical general-purpose application solutions, including middleware, components, services, etc., for users to use and evaluate.

## HPM APPS directory structure
![hpm_apps](docs/assets/hpm_app.png)

| Directory name | Description |
|--------|--------|
| <HPM_APP_BASE>/boards | Files for various development boards |
| <HPM_APP_BASE>/components | Software components |
| <HPM_APP_BASE>/docs | Document |
| <HPM_APP_BASE>/middleware | Middleware |
| <HPM_APP_BASE>/apps | Samples of various solutions |
| <HPM_APP_BASE>/services | Software Services |
| <HPM_APP_BASE>/tools | Tool files |

## Instructions for using the HPM APP

### HPM SDK version requirements

The versions of HPM APP and HPM SDK correspond one-to-one. To maintain version compatibility, the version number should be the same.
For example:
| Software Name | Version |
|--- | --- |
| hpm_sdk | 1.4.0 |
| hpm_apps | 1.4.0 |

### Installation
- Note1:Put the hpm_apps and hpm_sdk in the same directory:
![hpm_app_dir](docs/assets/hpm_app_dir.png)

- Note2:To modify the CMakeLists.txt file in the hpm_sdk directory, add hpm_apps to the project subdirectory (if hpm_sdk has been added, ignore it):
![hpm_app_setup](docs/assets/hpm_app_setup.png)
Path：hpm_sdk/CMakeLists.txt
    ```shell
    add_subdirectory(../hpm_apps ../hpm_apps/build_tmp)
    ```

### Build
- Windows:
    Using the env environment in Windows, you can use the GUI tool to build the project and the IDE SEGGER Embedded Studio for RISV-V to build and debug the project.
    - Generate projects using Windows GUI tools:
        
        Double-click to run the GUI tool:

        ![start_gui](docs/assets/start_gui.png)

        User Application Path: Select the project path to build.
        (If you want to use your own board，enable User Board Path)

        ![gui_help_start](docs/assets/gui_help_start.png)

       Build and debug with SES
        ![ses_debug](docs/assets/segger_build.png)


- Linux:
    In Linux, add the HPM_SDK_BASE path to the environment variable. Add it manually or by running a command.
    
    1.Add the HPM_SDK_BASE path to the environment variable by running the command:
    ```shell
      cd hpm_sdk/
      source env.sh
    ```
    2.Go to the apps directory that you want to build and compile, and use the build and compile command.
    for example：
    ```shell
    //Switch to the sample application directory
    cd hpm_apps\apps\mp_adapte\power_core2
    //Create build directory
    mkdir build
    //Switch directory to "build"
    cd build
    //Ninja-build:Generate build file
    cmake -GNinja -DBOARD_SEARCH_PATH=/home/work/workspace/hpm_apps/boards -DBOARD=hpm6200power -DCMAKE_BUILD_TYPE=flash_xip ..
    //build
    ninja
    ```
    After compilation is completed, the generated ELF and corresponding files can be found in the output directory.


## HPM_APPS ocumentation

- Local document:
  The HPM_APPS document can be compiled locally, and after successful compilation, the local document can be accessed through the following portals:
  - <HPM_APP_BASE>>/docs/index.html
  - <HPM_APP_BASE>>/docs/index_zh.html

- Online documentation:
  

## Code repository
- hpm_apps github: https://github.com/hpmicro/hpm_apps
- hpm_sdk github: https://github.com/hpmicro/hpm_sdk
- sdk_env github: https://github.com/hpmicro/sdk_env
- sdk extra demo: https://github.com/hpmicro/hpm_sdk_extra
- openocd (hpmicro patched): https://github.com/hpmicro/riscv-openocd
- gnu gcc toolchain: https://github.com/hpmicro/riscv-gnu-toolchain
