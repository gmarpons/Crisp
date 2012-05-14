## lib/CrispClangPlugin/Makefile --------------------------------*- Makefile -*-

#
# Relative path to the top of the source tree.
#
LEVEL=../..

#
# List all of the subdirectories that we will compile.
#
DIRS=

#
# Manual SOURCES list to not compile ClangDeclarations.cpp into shared
# library.
#
SOURCES =       ClangPrologPredicateRegistration.c \
		ClangPrologPredicates.cpp \
		CompilationInfo.cpp \
		CrispASTAction.cpp

DECLARATIONSFILENAME := $(strip ClangDeclarations)

BUILT_SOURCES = $(DECLARATIONSFILENAME).inc

LIBRARYNAME = crispclang
SHARED_LIBRARY = 1
LOADABLE_MODULE = 1
USEDLIBS = PrologEngineWrapper.a

include $(LEVEL)/Makefile.common

LDFLAGS += ${SWIPL_LDFLAGS}
LIBS += ${SWIPL_LIBS}
CPPFLAGS += ${SWIPL_CPPFLAGS}
CXXFLAGS += ${CLANG_OBJ_CXXFLAGS}
CXXFLAGS += ${CLANG_SRC_CXXFLAGS}
CPPFLAGS += ${BOOST_CPPFLAGS}

#
# Build ClangDeclarations.inc.
# Vars that can be easily changed: CLANGXX, CLANG_BUILD_MODE
#
CLANG_BUILD_MODE = $(BuildMode)
CLANGXX = $(LLVM_OBJ_ROOT)/$(CLANG_BUILD_MODE)/bin/clang++
PLUGINNAME = $(strip declextractor.so)
CLANGPLUGIN = $(LibDir)/$(PLUGINNAME)
CLANGPLUGINFLAGS = -Xclang -load -Xclang $(CLANGPLUGIN) \
	-Xclang -add-plugin -Xclang decl-extractor \
	-Xclang -plugin-arg-decl-extractor -Xclang init_clang_base_types
EXTRAFLAGS = -emit-llvm \
	$(patsubst %,-I%/include, $(LLVM_OBJ_ROOT) $(LLVM_SRC_ROOT)) \
	-D_GNU_SOURCE -D__STDC_CONSTANT_MACROS \
	-D__STDC_FORMAT_MACROS -D__STDC_LIMIT_MACROS
PROLOGSAVEDSTATEFILE = PrologBootForDeclExtractor.sh
PROLOGSAVEDSTATEPATH = $(DataDir)/$(strip $(PROLOGSAVEDSTATEFILE))

.PHONY: decl-extraction clean-decl-extraction

decl-extraction: $(DECLARATIONSFILENAME).inc

$(DECLARATIONSFILENAME).inc: $(CLANGPLUGIN) $(PROLOGSAVEDSTATEPATH)
$(DECLARATIONSFILENAME).inc: $(DECLARATIONSFILENAME).cpp
	$(Echo) Extracting declarations from Clang\'s source code \(takes some time\)
	$(Verb) $(CLANGXX) $(CXXFLAGS) $(CPPFLAGS) $(EXTRAFLAGS) \
		$(CLANGPLUGINFLAGS) -o /dev/null -S $<

clean-local:: clean-decl-extraction

clean-decl-extraction:
	-$(Verb) $(RM) -f $(DECLARATIONSFILENAME).inc

# The following rule is necessary because the general rule in
# Makefile.llvm.rules doesn't work for loadable modules (because of
# the lib prefix).
uninstall-local::
	$(Echo) Uninstalling $(BuildMode) Shared Library $(DestSharedLib)
	-$(Verb) $(RM) -f $(DestSharedLibDir)/$(LIBRARYNAME).*
