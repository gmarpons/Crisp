##===- projects/CrispClangPlugin/Makefile ------------------*- Makefile -*-===##
#
# This is the main makefile for the "Crisp Clang Plugin project"
#
##===----------------------------------------------------------------------===##

# FIXME: put the following library dependencies as configuration
# options using automake/autoconf
CPPFLAGS += "-I/srv/llvm-build/boost_1_46_1"
CPPFLAGS += "-I/usr/lib/swi-prolog/include/"
CPPFLAGS += "-I/srv/llvm-build/llvm-obj/tools/clang/include/"
CPPFLAGS += "-I/home/gmarpons/babel/LLVMtoolsAndProjects/clang-gmarpons.git/include/"
CPPFLAGS += "-fexceptions"

#
# Indicates our relative path to the top of the project's root directory.
#
LEVEL = .
DIRS = lib tools
EXTRA_DIST = include

#
# Include the Master Makefile that knows how to build all.
#
include $(LEVEL)/Makefile.common

