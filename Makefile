##===- projects/Crisp/Makefile -----------------------------*- Makefile -*-===##
#
# This is the main makefile for the "Crisp Clang Plugin project"
#
##===----------------------------------------------------------------------===##

#
# Indicates our relative path to the top of the project's root directory.
#
LEVEL = .
DIRS = lib tools prolog
EXTRA_DIST = include

#
# Include the Master Makefile that knows how to build all.
#
include $(LEVEL)/Makefile.common

