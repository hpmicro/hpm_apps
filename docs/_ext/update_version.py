import sys, getopt
import os
import filecmp
import hashlib
import glob
import chardet

from pathlib import Path
import re
import shlex
import shutil
from subprocess import Popen, PIPE, STDOUT
import tempfile
from typing import List, Dict, Optional, Any


from sphinx.application import Sphinx
from sphinx.environment import BuildEnvironment
from sphinx.util import logging


__version__ = "0.1.0"

# logger = logging.getLogger(__name__)

def update_version_in_file(filename, new_version):
    try:
         # 检测输入文件的编码
        with open(filename, 'rb') as f:
            result = chardet.detect(f.read())
            input_encoding = result['encoding']
        
        # 使用检测到的编码读取输入文件内容
        with open(filename, 'r', encoding=input_encoding) as file:
            content = file.read()

        # Use a regular expression to find and replace the version string
        pattern = r'version:\d+\.\d+\.\d+'
        new_version_string = f'version:{new_version}'
        new_content = re.sub(pattern, new_version_string, content)

        with open(filename, 'w', encoding=input_encoding) as file:
            file.write(new_content)

        print(f"Version updated to '{new_version}' in {filename}")
    except FileNotFoundError:
        print(f"Error: {filename} not found.")
    except Exception as e:
        print(f"Error updating version: {e}")

def read_version_info(filename):
    version_info = {}
    with open(filename, 'r') as file:
        for line in file:
            if '=' in line:
                key, value = line.strip().split('=')
                version_info[key.strip()] = int(value.strip())
    return version_info

def update_version_auto(root_path):
    version_file = os.path.join(root_path, "VERSION")
    version_info = read_version_info(version_file)

    print(f"Version: {version_info['VERSION_MAJOR']}.{version_info['VERSION_MINOR']}.{version_info['PATCHLEVEL']}")
    print(f"Version Tweak: {version_info['VERSION_TWEAK']}")
    print(f"Extra Version: {version_info['EXTRAVERSION']}")

    new_version_file_zh = os.path.join(root_path, "docs/zh/index.rst")
    new_version_file_en = os.path.join(root_path, "docs/en/index.rst")

    version_string = f"{version_info['VERSION_MAJOR']}.{version_info['VERSION_MINOR']}.{version_info['PATCHLEVEL']}"

    update_version_in_file(new_version_file_zh, version_string)
    update_version_in_file(new_version_file_en, version_string)

def updateversion_sphinx_run(app: Sphinx) -> None:

    print("\033[92m:{}\033[0m".format("Build: update version start...."))

    if app.config.hpm_app_base:
        outdir = Path(app.config.hpm_app_base)
        update_version_auto(outdir)
        print("\033[92m:{}\033[0m".format("Done: udpate version over!"))
    else:
        print("\033[1;31mError:{} \033[0m".format("hpm_app dir error!"))
        sys.exit(1)


def setup(app: Sphinx) -> Dict[str, Any]:
    # app.add_config_value("doxyrunner_doxygen", None, "env")
    # app.add_config_value("doxyrunner_doxyfile", None, "env")
    # app.add_config_value("doxyrunner_outdir", None, "env")
    # app.add_config_value("doxyrunner_outsubdir", None, "env")
    # app.add_config_value("doxyrunner_outdir_var", None, "env")
    # app.add_config_value("doxyrunner_fmt", False, "env")
    # app.add_config_value("doxyrunner_fmt_vars", {}, "env")
    # app.add_config_value("doxyrunner_fmt_pattern", "@{}@", "env")
    # app.add_config_value("doxyrunner_silent", True, "")
    # app.add_config_value("hpm_app_dir", None, "env")
    app.add_config_value("hpm_app_base", None, "env")

    app.connect("builder-inited", updateversion_sphinx_run)

    return {
        "version": __version__,
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }


def main(argv):
    print("updateversion run...")

    bsolute_path1 = os.path.abspath(argv[1])

    update_version_auto(bsolute_path1)




if __name__ == '__main__':
    main(sys.argv)
    

    
