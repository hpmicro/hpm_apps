import os
import shutil
from sphinx.application import Sphinx
import os.path
import subprocess
import shutil
import re
import sys
from pathlib import Path
import sphinx_rtd_theme
from typing import List, Dict, Optional, Any

__version__ = "0.1.0"

HPM_APP_BASE = Path(__file__).resolve().parents[2]
def search_doxygen_directories(directory):
    doxyfile_dirs = []
    for root, dirs, files in os.walk(directory):
        if 'Doxyfile_en' in files:
            doxyfile_dirs.append(root)

    return doxyfile_dirs

def doxygen_run(base_path, doxygen_path, app_name):
    doxygen_conf = os.path.join(doxygen_path, r'Doxyfile_zh')
    output_path = os.path.join(str(HPM_APP_BASE / "docs" / "zh" / "_static" / base_path), app_name)
    os.environ[r'DOXYGEN_OUTPUT_DIR'] = output_path
    if not os.path.exists(output_path):
        os.makedirs(output_path)

    shutil.rmtree(os.environ[r'DOXYGEN_OUTPUT_DIR'], ignore_errors=True)
    doxygen_cmd = r'doxygen ' + doxygen_conf
    return_code, out = subprocess.getstatusoutput(doxygen_cmd)

    if 0 != return_code:
        print("Doxygen failed!!!")
        print(out)

def doxygen_build(base_path):
    doxygen_path = search_doxygen_directories(str(HPM_APP_BASE / "docs" / "zh" / base_path))

    if doxygen_path:
        for dir_path in doxygen_path:
            # Get the absolute path
            abs_path = os.path.abspath(dir_path)
            # Get the parent directory's path
            third_last_dir_path = os.path.dirname(os.path.dirname(abs_path))
            dir_name = os.path.basename(os.path.normpath(third_last_dir_path))

            #print(third_last_dir_path)

            with open(third_last_dir_path + '/README_zh.md', 'r') as file:
                content = file.read()

                # 删除包含特定字符串的部分
                content = content.replace('](doc/api/assets/', '](')
                content = content.replace('](doc/', '](')
                content = content.replace('](hardware/', '](')

                # 使用正则表达式替换指定内容
                content = re.sub(r'## API.*:::{eval-rst}.*:::', '## Licensing\r\n\r\nHPM APP is permissively licensed using the BSD 3-clause license', content, flags=re.DOTALL)

            # 将处理后的内容写入新文件
            with open(third_last_dir_path + '/doc/doxygen/mainpage_zh.md', 'w') as new_file:
                new_file.write(content)

            # 读取文件内容
            with open(third_last_dir_path + '/doc/doxygen/Doxyfile_zh', 'r') as file:
                file_content = file.readlines()

            new_content = []

            # 处理文件内容
            for line in file_content:
                if line.startswith('HTML_EXTRA_FILES       ='):
                    # 在 HTML_EXTRA_FILES 后插入其他字符串
                    new_content.append(line)

                    # 遍历当前文件夹下的所有文件并打印它们的路径
                    for root, dirs, files in os.walk(third_last_dir_path + '/doc'):
                        for file in files:
                            file_path = os.path.join(root, file)
                            if file.endswith(".png"):
                                file_name = os.path.basename(file_path)
                                new_content.append('                         $(HPM_APP_BASE)/docs/zh/' + base_path + '/' + dir_name + '/doc/api/assets/' + file_name + ' \\\n')
                            elif file.endswith(".pdf"):
                                file_name = os.path.basename(file_path)
                                new_content.append('                         $(HPM_APP_BASE)/docs/zh/' + base_path + '/' + dir_name + '/doc/' + file_name + ' \\\n')

                else:
                    new_content.append(line)

            # 将处理后的内容写入文件
            with open(third_last_dir_path + '/doc/doxygen/Doxyfile_zh', 'w') as file:
                file.writelines(new_content)

            doxygen_run(base_path, dir_path, dir_name)

    else:
        print("未找到包含 Doxyfile_en 的目录.")

def doxygen_app(app: Sphinx) -> None:
    doxygen_build('apps')
    doxygen_build('middleware')

def setup(app: Sphinx) -> Dict[str, Any]:
    app.connect("builder-inited", doxygen_app)

    return {
        "version": __version__,
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
