# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import os.path
import subprocess
import shutil
import re
import sys
from pathlib import Path
import sphinx_rtd_theme

HPM_APP_BASE = Path(__file__).resolve().parents[2]
HTML_STATIC_DIR = Path(__file__).resolve().parents[0] / "_static"
DOXY_OUT = HPM_APP_BASE / "apps"
DOXY_OUT_SUB = "doc/api/zh"

sys.path.insert(0, str(HPM_APP_BASE / "docs" / "_ext"))

project = 'HPMicro Software Development Kit'
copyright = '2020-2023, HPMicro'
author = '先楫半导体软件组'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    "update_version",
    "doxygen_all",
    'sphinx_rtd_theme',
    'myst_parser',
    "sphinx.ext.todo",
    "sphinx.ext.extlinks",
    'sphinx.ext.duration',
    'sphinx_inline_tabs',
    "sphinx.ext.viewcode",
    "external_content",
    "copy_api",
]

templates_path = ['_templates']

DOXY_OUT.mkdir(parents = True, exist_ok = True)
# doxyrunner_doxygen = os.environ.get("DOXYGEN_EXECUTABLE", "doxygen")
hpm_app_base = HPM_APP_BASE
hpm_app_dir = HPM_APP_BASE / "apps"
doxyrunner_doxygen = HPM_APP_BASE / "docs" / "doxygen" / "doxygen_windows" / "doxygen"
doxyrunner_doxyfile = HPM_APP_BASE / "docs" / "doxygen" / "Doxyfile_zh" 
doxyrunner_outdir = DOXY_OUT
doxyrunner_outsubdir = DOXY_OUT_SUB
doxyrunner_fmt = True
doxyrunner_fmt_vars = {"HPM_APP_BASE": str(HPM_APP_BASE)}
doxyrunner_outdir_var = "DOXYGEN_OUTPUT_DIR"

# List of patterns, relative to source directory, that match files and
# directories to ignore when looking for source files.
# This pattern also affects html_static_path and html_extra_path.
exclude_patterns = []
html_extra_path = []

source_suffix = {
    '.rst': 'restructuredtext',
    '.md': 'markdown',
}

external_content_contents = [
    (HPM_APP_BASE / "docs/zh", "[!_]*"),
    (HPM_APP_BASE, "apps/**/tool"),
    (HPM_APP_BASE, "apps/**/*_zh.md",),
    (HPM_APP_BASE, "apps/**/doc/*.*"),
    (HPM_APP_BASE, "apps/**/doc/api/assets"),
    (HPM_APP_BASE, "apps/**/hardware/*.*"),
    (HPM_APP_BASE, "middleware/*_zh.md",),
    (HPM_APP_BASE, "middleware/hpm_motor/tool"),
    (HPM_APP_BASE, "middleware/hpm_motor/*_zh.md",),
    (HPM_APP_BASE, "middleware/hpm_motor/doc/*.*"),
    (HPM_APP_BASE, "middleware/hpm_motor/doc/api/assets"),
    (HPM_APP_BASE, "middleware/hpm_motor/hardware/*.*"),
]

hpm_app_assign_app = [
    (HPM_APP_BASE, "middleware/hpm_motor"),
]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme_options = {
    "logo_only": True,
    "prev_next_buttons_location": None
}
html_show_sphinx = False
html_logo = r'../assets/logo.png'
# html_static_path = [str(HTML_STATIC_DIR)]
html_last_updated_fmt = "%b %d, %Y"
html_domain_indices = False
html_split_index = True
html_show_sphinx = False

suppress_warnings = ['toc.excluded',
                    'toc.not_readable',
                    'toc.secnum','toc.circular','epub.duplicated_toc_entry','autosectionlabel.*',
                    'app.add_source_parser']

myst_heading_anchors = 2

myst_enable_extensions = [
    "amsmath",
    "colon_fence",
    "deflist",
    "dollarmath",
    "fieldlist",
    "html_admonition",
    "html_image",
    "replacements",
    "smartquotes",
    "strikethrough",
    "substitution",
    "tasklist",
]

