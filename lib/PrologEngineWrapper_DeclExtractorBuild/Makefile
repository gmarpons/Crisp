## lib/PrologEngineWrapper_DeclExtractorBuild/Makefile ----------*- Makefile -*-

#
# Relative path to the top of the source tree.
#
LEVEL=../..

#
# List all of the subdirectories that we will compile.
#
DIRS=

SOURCES=	ClangPrologQueries.cpp \
		PrologUtilityFunctions.cpp \
		RunPrologEngine.cpp

NO_INSTALL=1
LIBRARYNAME=PrologEngineWrapper_DeclExtractorBuild
BUILD_ARCHIVE=1

# Force use of local build tree data
ENABLE_DATA_OBJ_ROOT=1

include $(LEVEL)/Makefile.common

VPATH = $(PROJ_SRC_ROOT)/lib/PrologEngineWrapper

LDFLAGS += ${SWIPL_LDFLAGS}
LIBS += ${SWIPL_LIBS}
CPPFLAGS += ${SWIPL_CPPFLAGS}
CXXFLAGS += ${CLANG_OBJ_CXXFLAGS}
CXXFLAGS += ${CLANG_SRC_CXXFLAGS}
CXXFLAGS += -DNDEBUG		# No debug info
CPPFLAGS += ${BOOST_CPPFLAGS}

