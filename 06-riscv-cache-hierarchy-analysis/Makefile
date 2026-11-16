# ==============================================================================
# RISC-V Cache & Memory Hierarchy Analysis (CORE-V Wally) Makefile
# ==============================================================================

.PHONY: all build-report clean

all: build-report

build-report:
	cd report && if command -v tectonic >/dev/null 2>&1; then \
		tectonic report.tex; \
	else \
		pdflatex -interaction=nonstopmode report.tex && pdflatex -interaction=nonstopmode report.tex; \
		rm -f report.aux report.log report.out; \
	fi

clean:
	rm -f report/*.aux report/*.log report/*.out report/*.synctex.gz report/*.toc
