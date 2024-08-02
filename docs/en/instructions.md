
# User guide

## HPM SDK version requirements

The versions of HPM APP and HPM SDK correspond one-to-one. To maintain version compatibility, the version number should be the same.
For example:
| Software Name | Version |
|--- | --- |
| hpm_sdk | 1.x.y |
| hpm_apps | 1.x.y |

## Build

Recommend using the sdk_env environment on Windows to build the SES compilation and debugging project. In the SDK ENV tool, check **Enable User Application Path** and select the **hpm_apps/apps** directory, or choose the demo directory you need. It is recommended to use the former, as it can scan all demo files.

![sdk_env](docs/assets/hpm_app_sdkenv.png)

If you want to add some middleware from hpm_apps/middleware, please add code in demo CMakeLists.txt, suggest you add code before `generate_ses_project`, for example:

```
add_subdirectory(../../.. hpm_apps/build_tmp)
generate_ses_project()
```
