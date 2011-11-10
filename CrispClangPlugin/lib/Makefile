##===- projects/CrispClangPlugin/lib/Makefile --------------------*- Makefile -*-===##

# TODO: put the following library dependencies as configuration
# options using automake/autoconf
CPPFLAGS += "-I/srv/llvm-build/boost_1_46_0"
CPPFLAGS += "-I/usr/lib/swi-prolog/include/"
CPPFLAGS += "-I/srv/llvm-build/llvm-obj/tools/clang/include/"
CPPFLAGS += "-I/home/gmarpons/babel/LLVMtoolsAndProjects/clang-gmarpons.git/include/"
CPPFLAGS += "-fexceptions"

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

# TODO: put the following library dependencies as configuration
# options using automake/autoconf
LDFLAGS += -L/usr/lib/swi-prolog/lib
LIBS += -lswipl
