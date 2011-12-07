##===- projects/CrispClangPlugin/lib/Makefile --------------*- Makefile -*-===##

#
# Relative path to the top of the source tree.
#
LEVEL=..

#
# List all of the subdirectories that we will compile.
#
DIRS=

LIBRARYNAME=crispclang
SHARED_LIBRARY=1
LOADABLE_MODULE=1

include $(LEVEL)/Makefile.common

LDFLAGS += ${SWIPL_LDFLAGS}
LIBS += ${SWIPL_LIBS}
CPPFLAGS += ${SWIPL_CPPFLAGS}
CXXFLAGS += ${CLANG_OBJ_CXXFLAGS}
CXXFLAGS += ${CLANG_SRC_CXXFLAGS}
CPPFLAGS += ${BOOST_CPPFLAGS}
