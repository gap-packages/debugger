#
# debugger: Debugging support for GAP
#
# This file is part of the build system of a GAP kernel extension.
# Requires automake.
#
ACLOCAL_AMFLAGS = -I m4

BINARCHDIR = bin/$(GAPARCH)
GAPINSTALLLIB = $(BINARCHDIR)/debugger.so

lib_LTLIBRARIES = debugger.la

debugger_la_SOURCES = src/debugger.cc
debugger_la_CPPFLAGS = $(GAP_CPPFLAGS)
debugger_la_CFLAGS = $(GAP_CFLAGS)
debugger_la_LDFLAGS = $(GAP_LDFLAGS) -module -avoid-version
if SYS_IS_CYGWIN
debugger_la_LDFLAGS += -no-undefined -version-info 0:0:0 
debugger_la_LDFLAGS += -Wl,$(GAPROOT)/bin/$(GAPARCH)/gap.dll
endif

all-local: $(GAPINSTALLLIB)

$(GAPINSTALLLIB): debugger.la
	$(mkdir_p) $(BINARCHDIR)
if SYS_IS_CYGWIN
	cp .libs/debugger.dll $(GAPINSTALLLIB)
else
	cp .libs/debugger.so $(GAPINSTALLLIB)
endif

clean-local:
	rm -f $(GAPINSTALLLIB)

distclean-local:
	rm -rf bin/*

doc:
	($(GAPROOT)/bin/gap.sh -A makedoc.g)
