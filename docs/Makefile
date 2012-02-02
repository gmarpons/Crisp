## docs/Makefile ------------------------------------------------*- Makefile -*-

LEVEL = ..
DIRS =

#
# Include the Master Makefile that knows how to build all.
#
include $(LEVEL)/Makefile.common

HTML       := $(wildcard $(PROJ_SRC_DIR)/*.html) \
              $(wildcard $(PROJ_SRC_DIR)/*.css)
DOXYFILES  := doxygen.cfg.in \
              doxygen.css \
              doxygen.footer \
              doxygen.header \
              doxygen.intro
EXTRA_DIST := $(HTML) $(DOXYFILES)

.PHONY: install-doxygen doxygen

ifeq ($(ENABLE_DOXYGEN),1)
  install-local:: install-doxygen
else
  install-local::
endif

install-doxygen: doxygen
	$(Echo) Installing doxygen documentation
	$(Verb) $(MKDIR) $(DESTDIR)$(PROJ_docsdir)/html
	$(Verb) $(MKDIR) $(DESTDIR)$(PROJ_docsdir)/html/doxygen
	$(Verb) $(DataInstall) $(HTML) $(DESTDIR)$(PROJ_docsdir)/html
	$(Verb) cd $(PROJ_OBJ_DIR)/doxygen && \
	  $(FIND) . -type f -exec \
	    $(DataInstall) {} $(DESTDIR)$(PROJ_docsdir)/html/doxygen \;

doxygen: $(PROJ_OBJ_DIR)/doxygen.cfg
	$(Echo) Building doxygen documentation
	$(Verb) if test -e $(PROJ_OBJ_DIR)/doxygen ; then \
	  $(RM) -rf $(PROJ_OBJ_DIR)/doxygen ; \
	fi
	$(Verb) $(DOXYGEN) $(PROJ_OBJ_DIR)/doxygen.cfg

$(PROJ_OBJ_DIR)/doxygen.cfg: doxygen.cfg.in
	cat $< | sed \
	  -e "s|@abs_srcdir@|$(PROJ_SRC_DIR)|g" \
	  -e "s|@abs_top_srcdir@|$(PROJ_SRC_ROOT)|g" \
	  -e "s|@DOT@|$(DOT)|g" \
	  -e "s|@PACKAGE_VERSION@|$(PROJ_VERSION)|" \
	  -e "s|@abs_builddir@|$(PROJ_OBJ_DIR)|g" > $@

clean-local::
	-$(Verb) $(RM) -rf $(PROJ_OBJ_DIR)/doxygen $(PROJ_OBJ_DIR)/doxygen.cfg

uninstall-local::
	$(Echo) Uninstalling Documentation
	$(Verb) $(RM) -rf $(DESTDIR)$(PROJ_docsdir)
