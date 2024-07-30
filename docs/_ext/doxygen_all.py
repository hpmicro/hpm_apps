"""
Doxyrunner Sphinx Plugin
########################

Copyright (c) 2021 Nordic Semiconductor ASA
SPDX-License-Identifier: Apache-2.0

Introduction
============

This Sphinx plugin can be used to run Doxygen build as part of the Sphinx build
process. It is meant to be used with other plugins such as ``breathe`` in order
to improve the user experience. The principal features offered by this plugin
are:

- Doxygen build is run before Sphinx reads input files
- Doxyfile can be optionally pre-processed so that variables can be inserted
- Changes in the Doxygen input files are tracked so that Doxygen build is only
  run if necessary.
- Synchronizes Doxygen XML output so that even if Doxygen is run only changed,
  deleted or added files are modified.

References:

-  https://github.com/michaeljones/breathe/issues/420

Configuration options
=====================

- ``doxyrunner_doxygen``: Path to the Doxygen binary.
- ``doxyrunner_doxyfile``: Path to Doxyfile.
- ``doxyrunner_outdir``: Doxygen build output directory (inserted to
  ``OUTPUT_DIRECTORY``)
- ``doxyrunner_outdir_var``: Variable representing the Doxygen build output
  directory, as used by ``OUTPUT_DIRECTORY``. This can be useful if other
  Doxygen variables reference to the output directory.
- ``doxyrunner_fmt``: Flag to indicate if Doxyfile should be formatted.
- ``doxyrunner_fmt_vars``: Format variables dictionary (name: value).
- ``doxyrunner_fmt_pattern``: Format pattern.
- ``doxyrunner_silent``: If Doxygen output should be logged or not. Note that
  this option may not have any effect if ``QUIET`` is set to ``YES``.
"""
import sys, getopt
import os
import filecmp
import hashlib
import glob
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


logger = logging.getLogger(__name__)


HPM_APP_BASE = Path(__file__).resolve().parents[2]
HTML_STATIC_DIR = Path(__file__).resolve().parents[0] / "_static"
DOXY_OUT = HPM_APP_BASE / "apps"
DOXY_OUT_SUB = "doc/api/zh"

DOXY_OUT.mkdir(parents = True, exist_ok = True)



# doxyrunner_doxygen = os.environ.get("DOXYGEN_EXECUTABLE", "doxygen")
hpm_app_dir = HPM_APP_BASE / "apps"
doxyrunner_doxygen = HPM_APP_BASE / "docs" / "doxygen" / "doxygen_windows" / "doxygen"
doxyrunner_doxyfile = HPM_APP_BASE / "docs" / "doxygen" / "Doxyfile" 
doxyrunner_outdir = DOXY_OUT
doxyrunner_outsubdir = DOXY_OUT_SUB
doxyrunner_fmt = False
doxyrunner_fmt_vars = {"HPM_APP_BASE": str(HPM_APP_BASE)}
doxyrunner_outdir_var = "DOXYGEN_OUTPUT_DIR"
doxyrunner_fmt_pattern = "@{}@"
doxyrunner_silent = True


def hash_file(file: Path) -> str:
    """Compute the hash (SHA256) of a file in text mode.

    Args:
        file: File to be hashed.

    Returns:
        Hash.
    """

    with open(file, encoding="utf-8") as f:
        sha256 = hashlib.sha256(f.read().encode("utf-8"))

    return sha256.hexdigest()

def get_doxygen_option(doxyfile: str, option: str) -> List[str]:
    """Obtain the value of a Doxygen option.

    Args:
        doxyfile: Content of the Doxyfile.
        option: Option to be retrieved.

    Notes:
        Does not support appended values.

    Returns:
        Option values.
    """

    option_re = re.compile(r"^\s*([A-Z0-9_]+)\s*=\s*(.*)$")
    multiline_re = re.compile(r"^\s*(.*)$")

    values = []
    found = False
    finished = False
    for line in doxyfile.splitlines():
        if not found:
            m = option_re.match(line)
            if not m or m.group(1) != option:
                continue

            found = True
            value = m.group(2)
        else:
            m = multiline_re.match(line)
            if not m:
                raise ValueError(f"Unexpected line content: {line}")

            value = m.group(1)

        # check if it is a multiline value
        finished = not value.endswith("\\")

        # strip backslash
        if not finished:
            value = value[:-1]

        # split values
        values += shlex.split(value.replace("\\", "\\\\"))

        if finished:
            break

    return values


def process_doxyfile(
    doxyfile: str,
    outdir: Path,
    silent: bool,
    fmt: bool = False,
    fmt_pattern: Optional[str] = None,
    fmt_vars: Optional[Dict[str, str]] = None,
    outdir_var: Optional[str] = None,
) -> str:
    """Process Doxyfile.

    Notes:
        OUTPUT_DIRECTORY, WARN_FORMAT and QUIET are overridden to satisfy
        extension operation needs.

    Args:
        doxyfile: Path to the Doxyfile.
        outdir: Output directory of the Doxygen build.
        silent: If Doxygen should be run in quiet mode or not.
        fmt: If Doxyfile should be formatted.
        fmt_pattern: Format pattern.
        fmt_vars: Format variables.
        outdir_var: Variable representing output directory.

     Returns:
        Processed Doxyfile content.
    """

    with open(doxyfile) as f:
        content = f.read()

    content = re.sub(
        r"^\s*OUTPUT_DIRECTORY\s*=.*$",
        f"OUTPUT_DIRECTORY={outdir.as_posix()}",
        content,
        flags=re.MULTILINE,
    )

    content = re.sub(
        r"^\s*WARN_FORMAT\s*=.*$",
        'WARN_FORMAT="$file:$line: $text"',
        content,
        flags=re.MULTILINE,
    )

    content = re.sub(
        r"^\s*QUIET\s*=.*$",
        "QUIET=" + "YES" if silent else "NO",
        content,
        flags=re.MULTILINE,
    )

    if fmt:
        if not fmt_pattern or not fmt_vars:
            raise ValueError("Invalid formatting pattern or variables")

        if outdir_var:
            fmt_vars = fmt_vars.copy()
            fmt_vars[outdir_var] = outdir.as_posix()

        for var, value in fmt_vars.items():
            content = content.replace(fmt_pattern.format(var), value)

    return content


def process_doxygen_output(line: str, silent: bool) -> None:
    """Process a line of Doxygen program output.

    This function will map Doxygen output to the Sphinx logger output. Errors
    and warnings will be converted to Sphinx errors and warnings. Other
    messages, if not silent, will be mapped to the info logger channel.

    Args:
        line: Doxygen program line.
        silent: True if regular messages should be logged, False otherwise.
    """

    m = re.match(r"(.*):(\d+): ([a-z]+): (.*)", line)
    if m:
        type = m.group(3)
        message = f"{m.group(1)}:{m.group(2)}: {m.group(4)}"
        if type == "error":
            print("error:", message)
        elif type == "warning":
            print("warning:", message)
        else:
            print(message)
    elif not silent:
        print(line)


def run_doxygen(doxygen: str, doxyfile: str, silent: bool = False) -> None:
    """Run Doxygen build.

    Args:
        doxygen: Path to Doxygen binary.
        doxyfile: Doxyfile content.
        silent: If Doxygen output should be logged or not.
    """

    f_doxyfile = tempfile.NamedTemporaryFile("w", delete=False)
    f_doxyfile.write(doxyfile)
    f_doxyfile.close()

    p = Popen([doxygen, f_doxyfile.name], stdout=PIPE, stderr=STDOUT, encoding="utf-8")
    while True:
        line = p.stdout.readline()  # type: ignore
        if line:
            process_doxygen_output(line.rstrip(), silent)
        if p.poll() is not None:
            break

    Path(f_doxyfile.name).unlink()

    if p.returncode:
        raise IOError(f"Doxygen process returned non-zero ({p.returncode})")


def sync_doxygen(doxyfile: str, new: Path, prev: Path) -> None:
    """Synchronize Doxygen output with a previous build.

    This function makes sure that only new, deleted or changed files are
    actually modified in the Doxygen XML output. Latest HTML content is just
    moved.

    Args:
        doxyfile: Contents of the Doxyfile.
        new: Newest Doxygen build output directory.
        prev: Previous Doxygen build output directory.
    """

    generate_html = get_doxygen_option(doxyfile, "GENERATE_HTML")
    if generate_html[0] == "YES":
        html_output = get_doxygen_option(doxyfile, "HTML_OUTPUT")
        if not html_output:
            raise ValueError("No HTML_OUTPUT set in Doxyfile")

        new_htmldir = new / html_output[0]
        prev_htmldir = prev / html_output[0]

        if prev_htmldir.exists():
            shutil.rmtree(prev_htmldir)
        new_htmldir.rename(prev_htmldir)

    xml_output = get_doxygen_option(doxyfile, "XML_OUTPUT")
    if not xml_output:
        raise ValueError("No XML_OUTPUT set in Doxyfile")

    new_xmldir = new / xml_output[0]
    prev_xmldir = prev / xml_output[0]

    if prev_xmldir.exists():
        dcmp = filecmp.dircmp(new_xmldir, prev_xmldir)

        for file in dcmp.right_only:
            (Path(dcmp.right) / file).unlink()

        for file in dcmp.left_only + dcmp.diff_files:
            shutil.copy(Path(dcmp.left) / file, Path(dcmp.right) / file)

        shutil.rmtree(new_xmldir)
    else:
        new_xmldir.rename(prev_xmldir)

def doxygen_build(doxygen_file, doxygen_outdir):

    outdir = Path(doxygen_outdir)
    # print("------outdir:", outdir)

    outdir.mkdir(parents=True,exist_ok=True)

    # print("Preparing Doxyfile...")
    doxyfile = process_doxyfile(
        doxygen_file,
        outdir,
        doxyrunner_silent,
        True,
        doxyrunner_fmt_pattern,
        doxyrunner_fmt_vars,
        doxyrunner_outdir_var,
    )

    # print("Running Doxygen...")
    run_doxygen(
        doxyrunner_doxygen,
        doxyfile,
        doxyrunner_silent,
    )


def replace_relative_paths(file_path, num_levels=4):
    relative_path_pattern = r'\]\(((?!https?://)[^)]+)\)'

    def replace_func(match):
        relative_path = match.group(1)
        new_path = '/'.join(['..'] * num_levels + [relative_path])
        return f']({new_path})'
    
    def replace_last_section(text):
        pattern = r'##*'
        lines = text.split("\n")
        matched_index = []
        for i, line in enumerate(lines):
            if re.match(pattern, line):
                matched_index.append(i)
        units = []
        for i in range(len(matched_index)):
            start = matched_index[i]
            end = matched_index[i + 1] if i + 1 < len(matched_index) else None
            units.append(lines[start:end])

        units[-1][0] = '## Licensing'

        new_text = ''

        for unit in units:
            for line in unit:
                new_text += line + '\n'

        section_pattern = r':::{eval-rst}.*:::'
        replacement = 'HPM APP is permissively licensed using the BSD 3-clause license.'

        # 使用 re.split() 将文本分成多个部分
        parts = re.split(section_pattern, new_text, maxsplit=1, flags=re.DOTALL)

        # 如果找到了匹配的部分,则替换最后一个部分
        if len(parts) > 1:
            parts[-1] = replacement
            return ''.join(parts)
        else:
            return new_text

    with open(file_path, 'r', encoding='utf-8') as file:
        content = file.read()

    new_content = re.sub(relative_path_pattern, replace_func, content)
    new_content = replace_last_section(new_content)

    with open(file_path, 'w', encoding='utf-8') as file:
        file.write(new_content)

def doxygen_single_app(single_dir):
    item_path = single_dir
    markdown_zh_files = os.path.join(item_path, "README_zh.md")
    markdown_en_files = os.path.join(item_path, "README_en.md")
    if not os.path.exists(markdown_zh_files):
        print("\033[1;31mError: markdown_zh_files:{} 不存在 \033[0m".format(markdown_zh_files))
        sys.exit(1)
    
    if not os.path.exists(markdown_en_files):
        print("\033[1;31mError: markdown_en_files:{} 不存在 \033[0m".format(markdown_en_files))
        sys.exit(1)
    
    # print("markdown_zh_files:", markdown_zh_files)
    # print("markdown_en_files:", markdown_en_files)

    copy_zh_dir = os.path.normpath(os.path.join(item_path, "doc/mainpage/md/zh"))
    copy_en_dir = os.path.normpath(os.path.join(item_path, "doc/mainpage/md/en"))


    # 如果目标目录不存在,创建目录
    if not os.path.exists(copy_zh_dir):
        os.makedirs(copy_zh_dir)
    if not os.path.exists(copy_en_dir):
        os.makedirs(copy_en_dir)

    # 拷贝文件
    copy_zh_file = os.path.join(copy_zh_dir, "mainpage_zh.md")
    copy_en_file = os.path.join(copy_en_dir, "mainpage_en.md")
    shutil.copy2(markdown_zh_files, copy_zh_file)
    shutil.copy2(markdown_en_files, copy_en_file)

    replace_relative_paths(copy_zh_file)
    replace_relative_paths(copy_en_file)

    try:
        doxyrunner_doxyfile = os.path.normpath(os.path.join(item_path, "doc/doxygen/Doxyfile_zh"))
        doxyrunner_outdir = os.path.normpath(os.path.join(item_path, "doc/api/zh"))
        if os.path.isdir(doxyrunner_outdir):
            shutil.rmtree(doxyrunner_outdir)
        # print("doxyrunner_doxyfile:", doxyrunner_doxyfile)
        # print("doxyrunner_outdir:", doxyrunner_outdir)
        doxygen_build(doxyrunner_doxyfile, doxyrunner_outdir)

        doxyrunner_doxyfile = os.path.normpath(os.path.join(item_path, "doc/doxygen/Doxyfile_en"))
        doxyrunner_outdir = os.path.normpath(os.path.join(item_path, "doc/api/en"))
        if os.path.isdir(doxyrunner_outdir):
            shutil.rmtree(doxyrunner_outdir)
        # print("doxyrunner_doxyfile zh:", doxyrunner_doxyfile)
        # print("doxyrunner_outdir zh:", doxyrunner_outdir)
        doxygen_build(doxyrunner_doxyfile, doxyrunner_outdir)
    except Exception as error:
        print("\033[1;31mError:{} \033[0m".format(error))
        sys.exit(1)


def doxygen_all_auto(src_root_path):
    for item in os.listdir(src_root_path):
        item_path = os.path.join(src_root_path, item)
        if os.path.isdir(item_path):
            doxygen_single_app(item_path)


def doxygen_sphinx_run(app: Sphinx) -> None:
    """Doxyrunner entry point.

    Args:
        app: Sphinx application instance.
    """

    print("\033[92m:{}\033[0m".format("Build: doxygen start...."))

    print("doxyrunner_fmt_vars:", doxyrunner_fmt_vars)
    print("doxyrunner_outdir_var:", doxyrunner_outdir_var)
    print("doxyrunner_doxygen:", doxyrunner_doxygen)
    print("doxyrunner_fmt_pattern:", doxyrunner_fmt_pattern)

    if app.config.hpm_app_dir:
        outdir = Path(app.config.hpm_app_dir)
        doxygen_all_auto(outdir)
        
        for content in app.config.hpm_app_assign_app:
            prefix_src, glob = content
            for src in prefix_src.glob(glob):
                if src.is_dir():
                    doxygen_single_app(src)
                else:
                    print("\033[1;31mError:{} \033[0m".format("hpm_app hpm_app_assign_app error!"))
                    sys.exit(1)
        print("\033[92m:{}\033[0m".format("Done: doxygen over!"))
    else:
        print("\033[1;31mError:{} \033[0m".format("hpm_app dir error!"))
        sys.exit(1)


def setup(app: Sphinx) -> Dict[str, Any]:
    app.add_config_value("doxyrunner_doxygen", None, "env")
    app.add_config_value("doxyrunner_doxyfile", None, "env")
    app.add_config_value("doxyrunner_outdir", None, "env")
    app.add_config_value("doxyrunner_outsubdir", None, "env")
    app.add_config_value("doxyrunner_outdir_var", None, "env")
    app.add_config_value("doxyrunner_fmt", False, "env")
    app.add_config_value("doxyrunner_fmt_vars", {}, "env")
    app.add_config_value("doxyrunner_fmt_pattern", "@{}@", "env")
    app.add_config_value("doxyrunner_silent", True, "")
    if 'hpm_app_assign_app' not in app.config:
        app.add_config_value("hpm_app_assign_app", [], "env")

    if 'hpm_app_dir' not in app.config:
        app.add_config_value("hpm_app_dir", None, "env")

    app.connect("builder-inited", doxygen_sphinx_run)

    return {
        "version": __version__,
        "parallel_read_safe": True,
        "parallel_write_safe": True,
    }
                
# def update_doc_version(hpm_app_root):


def main(argv):
    print("doxygen run...")

    bsolute_path1 = os.path.abspath(argv[1])

    doxygen_all_auto(bsolute_path1)


if __name__ == '__main__':
    main(sys.argv)
    

    
