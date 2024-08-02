
# 使用说明

## 依赖HPM SDK版本要求

HPM APP版本和HPM SDK版本一一对应，为了保持版本兼容，版本号必须相同。
例如：
| 软件名称 | 版本号 |
|--- | --- |
| hpm_sdk | 1.x.y |
| hpm_apps | 1.x.y |


## 构建编译

推荐在 Windows下使用 sdk_env 环境，构建 SES 编译调试工程。在 SDK ENV 工具中勾选 **Enable User Application Path** 并选择 **hpm_apps/apps** 目录即可，或者选择到自己需要用的 demo 目录，推荐使用前者，可以扫描所有 demo 文件。

![sdk_env](../assets/hpm_app_sdkenv.png)

如果需要增加 hpm_apps/middleware 中的相关中间件，请在 demo 下的 CMakeLists.txt 文件中增加以下代码, 推荐放在 `generate_ses_project` 之前,举例如下：

```
add_subdirectory(../../.. hpm_apps/build_tmp)
generate_ses_project()
```
