[中文](README_zh.md)

# HPM APPS introduction
HPM APPS is an upper layer application software development kit based on HPM SDK. It provides a variety of typical general-purpose application solutions, including middleware, components, services, etc., for users to use and evaluate.

## HPM APPS directory structure

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

Note: Each CASE of the HPM APP relies on the corresponding HPM SDK version. To ensure that the project can be built and compiled correctly, please refer to the SDK version dependency instructions in the README document located in the corresponding CASE directory.

### Build

Recommend using the sdk_env environment on Windows to build the SES compilation and debugging project. In the SDK ENV tool, check **Enable User Application Path** and select the **hpm_apps/apps** directory, or choose the demo directory you need. It is recommended to use the former, as it can scan all demo files.

![sdk_env](docs/assets/hpm_app_sdkenv.png)

If you want to add some middleware from hpm_apps/middleware, please add code in demo CMakeLists.txt, suggest you add code before `generate_ses_project`, for example:

```
add_subdirectory(../../.. hpm_apps/build_tmp)
generate_ses_project()
```

## HPM_APPS documentation

- Local document:
  The HPM_APPS document can be compiled locally, and after successful compilation, the local document can be accessed through the following portals:
  - <HPM_APP_BASE>>/docs/index.html
  - <HPM_APP_BASE>>/docs/index_zh.html

- Online documentation:
  - https://hpm-apps.readthedocs.io/en/latest/
  - https://hpm-apps.readthedocs.io/zh-cn/latest/


## Code repository
- hpm_apps github: https://github.com/hpmicro/hpm_apps
- hpm_sdk github: https://github.com/hpmicro/hpm_sdk
- sdk_env github: https://github.com/hpmicro/sdk_env
- sdk extra demo: https://github.com/hpmicro/hpm_sdk_extra
- openocd (hpmicro patched): https://github.com/hpmicro/riscv-openocd
- gnu gcc toolchain: https://github.com/hpmicro/riscv-gnu-toolchain
