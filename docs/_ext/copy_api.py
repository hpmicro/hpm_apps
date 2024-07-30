import sys, getopt
import filecmp
import os
from pathlib import Path
import re
import shutil
import tempfile
from typing import Dict, Any, List, Optional

from sphinx.application import Sphinx
from sphinx.environment import BuildEnvironment
from sphinx.util import logging

def copy_folder(source_dir, target_dir):
    # 创建目标文件夹（如果不存在）
    if not os.path.exists(target_dir):
        os.makedirs(target_dir)
    
    for root, dirs, files in os.walk(source_dir):
        # 获取当前路径相对于源文件夹的子路径
        relative_root = os.path.relpath(root, source_dir)
        
        # 构造目标路径
        target_subdir = os.path.join(target_dir, relative_root)
        
        # 创建目标子文件夹（如果不存在）
        if not os.path.exists(target_subdir):
            os.makedirs(target_subdir)
            
        # 复制文件到目标位置
        for file in files:
            source_file = os.path.join(root, file)
            target_file = os.path.join(target_subdir, file)
            shutil.copy2(source_file, target_file)

def traverse_apps_all_directory(src_path, dst_path):

    dirs = os.path.split(dst_path)
    name = os.path.dirname(dst_path)
    
    # 判断目录列表长度大于1时才有上一级目录
    if len(dirs) > 1:
        # 获取上一级目录名称
        parent_dir = dirs[-2]
    else:
        parent_dir = None
        
    # print("上一级目录名称为：", parent_dir)
    last_folder = os.path.split(str(dst_path).rstrip("/"))[-1]
    # print("last_folder:", last_folder)

    items = os.listdir(src_path)
    for item in items:
        file_path = os.path.join(src_path, item)
        if os.path.isdir(file_path):
            # print("目录名称：", item)
            # print("file_path:", src_path)
            # print("dst_path:", dst_path)
            src_result_path = os.path.join(file_path, "doc")
            
            dst_dir=  "output/"+ last_folder + "/html/apps/" + item+"/doc"
            dst_result_path = os.path.join(parent_dir, dst_dir)
            print("src_path:", src_result_path)
            print("dst_path", dst_result_path)

            copy_folder(src_result_path , dst_result_path)

            src_result_path = os.path.join(file_path, "hardware")
            
            dst_dir=  "output/"+ last_folder + "/html/apps/" + item+"/hardware"
            dst_result_path = os.path.join(parent_dir, dst_dir)
            print("src_path:", src_result_path)
            print("dst_path", dst_result_path)
            
            copy_folder(src_result_path , dst_result_path)

            src_result_path = os.path.join(file_path, "tool")
            
            dst_dir=  "output/"+ last_folder + "/html/apps/" + item+"/tool"
            dst_result_path = os.path.join(parent_dir, dst_dir)
            print("src_path:", src_result_path)
            print("dst_path", dst_result_path)
            
            copy_folder(src_result_path , dst_result_path)
            
def traverse_middleware_single_directory(src_path, dst_path):

    dirs = os.path.split(dst_path)
    name = os.path.dirname(dst_path)
    
    # 判断目录列表长度大于1时才有上一级目录
    if len(dirs) > 1:
        # 获取上一级目录名称
        parent_dir = dirs[-2]
    else:
        parent_dir = None
        
    # print("上一级目录名称为：", parent_dir)
    last_folder = os.path.split(str(dst_path).rstrip("/"))[-1]
    # print("last_folder:", last_folder)

    file_path = src_path
    last_dir_name = os.path.basename(file_path)
    if os.path.isdir(file_path):
        # print("src_path_file_path:", src_path)
        # print("dst_path:", dst_path)
        src_result_path = os.path.join(file_path, "doc")
        
        dst_dir=  "output/"+ last_folder + "/html/middleware/" + last_dir_name +"/doc"
        dst_result_path = os.path.join(parent_dir, dst_dir)
        print("src_path:", src_result_path)
        print("dst_path", dst_result_path)

        copy_folder(src_result_path , dst_result_path)

        src_result_path = os.path.join(file_path, "hardware")
        
        dst_dir=  "output/"+ last_folder + "/html/middleware/" + last_dir_name +"/hardware"
        dst_result_path = os.path.join(parent_dir, dst_dir)
        print("src_path:", src_result_path)
        print("dst_path", dst_result_path)
        
        copy_folder(src_result_path , dst_result_path)

        src_result_path = os.path.join(file_path, "tool")
        
        dst_dir=  "output/"+ last_folder + "/html/middleware/" + last_dir_name +"/tool"
        dst_result_path = os.path.join(parent_dir, dst_dir)
        print("src_path:", src_result_path)
        print("dst_path", dst_result_path)
        
        copy_folder(src_result_path , dst_result_path)
     



__version__ = "0.1.0"


DEFAULT_DIRECTIVES = ("figure", "image", "include", "literalinclude")
"""Default directives for included content."""


def adjust_includes(
    fname: Path,
    basepath: Path,
    directives: List[str],
    encoding: str,
    dstpath: Optional[Path] = None,
) -> None:
    """Adjust included content paths.

    Args:
        fname: File to be processed.
        basepath: Base path to be used to resolve content location.
        directives: Directives to be parsed and adjusted.
        encoding: Sources encoding.
        dstpath: Destination path for fname if its path is not the actual destination.
    """

    if fname.suffix != ".rst":
        return

    dstpath = dstpath or fname.parent

    def _adjust(m):
        directive, fpath = m.groups()

        # ignore absolute paths
        if fpath.startswith("/"):
            fpath_adj = fpath
        else:
            fpath_adj = Path(os.path.relpath(basepath / fpath, dstpath)).as_posix()

        return f".. {directive}:: {fpath_adj}"

    with open(fname, "r+", encoding=encoding) as f:
        content = f.read()
        content_adj, modified = re.subn(
            r"\.\. (" + "|".join(directives) + r")::\s*([^`\n]+)", _adjust, content
        )
        if modified:
            f.seek(0)
            f.write(content_adj)
            f.truncate()


def copy_contents(app: Sphinx) -> None:
    """Copy api contents.

    Args:
        app: Sphinx application instance.
    """

    print("\033[92m:{}\033[0m".format("Build: copy doxygenapi start...."))

    srcdir = Path(app.srcdir).resolve()
    # print("srcdir:", app.srcdir, "test:", srcdir)
    # print("apps:", app.config.hpm_app_dir)
    traverse_apps_all_directory(app.config.hpm_app_dir, srcdir)

    # print("-----app.config.hpm_app_assign_app:", app.config.hpm_app_assign_app)
    for content in app.config.hpm_app_assign_app:
        prefix_src, glob = content
        for src in prefix_src.glob(glob):
            if src.is_dir():
                traverse_middleware_single_directory(src, srcdir)
            else:
                print("\033[1;31mError:{} \033[0m".format("hpm_app hpm_app_assign_app error!"))
                sys.exit(1)
    
    print("\033[92m:{}\033[0m".format("Build: copy doxygenapi over!"))


def setup(app: Sphinx) -> Dict[str, Any]:
    if 'hpm_app_assign_app' not in app.config:
        app.add_config_value("hpm_app_assign_app", [], "env")

    if 'hpm_app_dir' not in app.config:
        app.add_config_value("hpm_app_dir", None, "env")

    app.connect("builder-inited", copy_contents)

    return {
        "version": __version__,
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
