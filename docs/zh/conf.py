# Configuration file for the Sphinx documentation builder.
#
# For the full list of built-in configuration values, see the documentation:
# https://www.sphinx-doc.org/en/master/usage/configuration.html

# -- Project information -----------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#project-information

import os
import os.path
import sys
from pathlib import Path
import sphinx_rtd_theme

HPM_APP_BASE = Path(__file__).resolve().parents[2]
HTML_STATIC_DIR = Path(__file__).resolve().parents[0] / "_static"

os.environ["HPM_APP_BASE"] = str(HPM_APP_BASE)
sys.path.insert(0, str(HPM_APP_BASE / "docs" / "_ext"))
HTML_STATIC_DIR.mkdir(parents = True, exist_ok = True)

project = 'HPMicro Software Development Kit'
copyright = '2020-2023, HPMicro'
author = 'HPMicro Software Team'

# -- General configuration ---------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#general-configuration

extensions = [
    'sphinx_rtd_theme',
    'myst_parser',
    "sphinx.ext.todo",
    "sphinx.ext.extlinks",
    'sphinx.ext.duration',
    'sphinx_inline_tabs',
    "sphinx.ext.viewcode",
    "external_content",
    "update_version",
    "doxygenrun_zh",
]

templates_path = ['_templates']

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
    (HPM_APP_BASE, "apps/**/*_zh.md"),
    (HPM_APP_BASE, "apps/**/doc"),
    (HPM_APP_BASE, "apps/**/hardware/*.pdf"),
    (HPM_APP_BASE, "middleware/*_zh.md",),
    (HPM_APP_BASE, "middleware/hpm_motor/*_zh.md",),
    (HPM_APP_BASE, "middleware/hpm_motor/doc"),
    (HPM_APP_BASE, "middleware/hpm_motor/hardware/*.*"),
    (HPM_APP_BASE, "middleware/hpm_monitor/*_zh.md",),
    (HPM_APP_BASE, "middleware/hpm_monitor/doc"),
    (HPM_APP_BASE, "middleware/hpm_monitor/hardware/*.*"),
]

# -- Options for HTML output -------------------------------------------------
# https://www.sphinx-doc.org/en/master/usage/configuration.html#options-for-html-output

html_theme = 'sphinx_rtd_theme'
html_theme_path = [sphinx_rtd_theme.get_html_theme_path()]
html_theme_options = {
    "logo_only": True,
    "prev_next_buttons_location": None
}
html_logo = str(HPM_APP_BASE / "docs" / "assets" / "logo.png")
html_static_path = [str(HTML_STATIC_DIR)]
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

