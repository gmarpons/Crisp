## lib/RunPrologEngine/Makefile ---------------------------------*- Makefile -*-

#
# Relative path to the top of the source tree.
#
LEVEL=../..

#
# List all of the subdirectories that we will compile.
#
DIRS=

LIBRARYNAME=RunPrologEngine
BUILD_ARCHIVE=1

include $(LEVEL)/Makefile.common

LDFLAGS += ${SWIPL_LDFLAGS}
LIBS += ${SWIPL_LIBS}
CPPFLAGS += ${SWIPL_CPPFLAGS}
CXXFLAGS += ${CLANG_OBJ_CXXFLAGS}
CXXFLAGS += ${CLANG_SRC_CXXFLAGS}
CPPFLAGS += ${BOOST_CPPFLAGS}
