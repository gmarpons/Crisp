## prolog/Rules/Makefile ----------------------------------------*- Makefile -*-

#
# Relative path to the top of the source tree.
#
LEVEL=../..

#
# List all of the subdirectories that we will compile.
#
DIRS=

include $(LEVEL)/Makefile.common

all-local:: $(DataDir)/.dir
	$(Echo) Copying rule files
	$(Verb) $(CP) -f $(PROJ_SRC_DIR)/*.pl $(DataDir)

clean-local::
	-$(Verb) $(RM) -f $(DataDir)/*.pl

$(DESTDIR)$(PROJ_datadir)::
	$(Verb) $(MKDIR) $@

install-local:: $(DESTDIR)$(PROJ_datadir)
	$(Echo) Installing rule files
	$(Verb) $(CP) -f $(PROJ_SRC_DIR)/*.pl $(DESTDIR)$(PROJ_datadir)

uninstall-local::
	$(Echo) Uninstalling rule files
	-$(Verb) $(RM) -f $(DESTDIR)$(PROJ_datadir)/*.pl
