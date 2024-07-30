# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
#SPHINXBUILD   ?= sphinx-build
SPHINXBUILD   ?= python3 -m sphinx.cmd.build

CLEAN_DOC     ?= python3 _ext/clean_docs.py

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
html: Makefile
	@$(SPHINXBUILD) -M $@ "zh" "output/zh" $(SPHINXOPTS) $(O)
	@$(SPHINXBUILD) -M $@ "en" "output/en" $(SPHINXOPTS) $(O)

clean:
	@$(CLEAN_DOC)

