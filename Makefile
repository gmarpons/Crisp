## Makefile - Main makefile for the Crisp Project ---------------*- Makefile -*-

#
# Indicates our relative path to the top of the project's root directory.
#
LEVEL = .
DIRS = docs lib prolog tools
EXTRA_DIST = include

#
# Include the Master Makefile that knows how to build all.
#
include $(LEVEL)/Makefile.common

