# Doxygen 使用说明

当前版本 doxygen 生成流程如下：

- 查找 apps 和 middleware 目录下的 doxygen file（en 和 zh）
- 替换相关字符并生成 mainpage_en.md 和 mainpage_zh.md
- 自动查找 readme 中所需要的文件（包含文件和图片），并自动填充到 doxygen file 中
- 进入 docs 目录，执行 `make html` （linux）， 或者 执行 `.\make.bat html` (windows)

## 新增和修改

- 拷贝现有工程，比如从 adc 下拷贝 doc 目录到新目录
- 修改 doxygen 文件中 **INPUT** 参数,其中，mainpage_xx.md 位置必须下面的代码位置，`$(HPM_APP_BASE)/docs/en/` 或者 `$(HPM_APP_BASE)/docs/zh/`

```
INPUT                  = $(HPM_APP_BASE)/docs/en/apps/adc/doc/doxygen/mainpage_en.md \
                         $(HPM_APP_BASE)/apps/adc/software/adc16_inl_dnl \
			 $(HPM_APP_BASE)/apps/adc/software/adc16_sinad

```

- readme_xx.md 文件中 png 文件路径必须存放在 doc/assets/ 下， pdf 文件必须位于 doc 目录下
- 最后的 api 路径按下如下修改,以 `../../_static/`  为前缀，`html/index.html` 为后缀，可以对照输出的 html 目录查看是否正确，
输出 API 的目录为 `docs/output/en(or zh)/html/_static/` 下

```
## API

:::{eval-rst}

About software API: `API doc <../../_static/apps/adc/html/index.html>`_ 。
:::
```

## 编译

- 安装 doxygen 工具（如果是windows，可以直接将 docs/doxygen/doxygen_windows 放到系统 path）
- 编译：docs 目录下执行 `make html` （linux）， 或者 执行 `.\make.bat html` (windows)
- 清空缓存： docs 目录下执行 `make clean` （linux）， 或者 执行 `.\make.bat clean` (windows)



## 注意事项

pdf，png 等文件禁止包含空格