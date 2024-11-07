# Minimal makefile for Sphinx documentation
#

# You can set these variables from the command line, and also
# from the environment for the first two.
SPHINXOPTS    ?=
SPHINXBUILD   ?= sphinx-build
#SPHINXBUILD   ?= python -m sphinx.cmd.build

# Catch-all target: route all unknown targets to Sphinx using the new
# "make mode" option.  $(O) is meant as a shortcut for $(SPHINXOPTS).
html: Makefile
	@$(SPHINXBUILD) -M $@ "zh" "output/zh" $(SPHINXOPTS) $(O)
	@$(SPHINXBUILD) -M $@ "en" "output/en" $(SPHINXOPTS) $(O)

clean:
	rm -rf output
	rm -rf en/_static
	rm -rf zh/_static
	rm -rf en/apps
	rm -rf zh/apps
	rm -rf en/middleware
	rm -rf zh/middleware

