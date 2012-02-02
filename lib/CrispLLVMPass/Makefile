## lib/CrispLLVMPass/Makefile -----------------------------------*- Makefile -*-

#
# Relative path to the top of the source tree.
#
LEVEL=../..

#
# List all of the subdirectories that we will compile.
#
DIRS=

LIBRARYNAME=crispllvm
SHARED_LIBRARY=1
LOADABLE_MODULE=1
USEDLIBS=SWIPrologInterface.a

include $(LEVEL)/Makefile.common

LDFLAGS += ${SWIPL_LDFLAGS}
LIBS += ${SWIPL_LIBS}
CPPFLAGS += ${SWIPL_CPPFLAGS}
CPPFLAGS += ${BOOST_CPPFLAGS}
