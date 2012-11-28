# Variables you need to set to run this Makefile:
#    LLVM_OBJ_ROOT=
#    BUILD_MODE=
#    CRISP_INSTALL_ROOT=
#    LIB_PREFIX=

BUILD_MODE=
# BUILD_MODE_STRING=debug
BUILD_MODE_STRING=release
LLVM_OBJ_ROOT=$(HOME)/llvm-cmake-$(BUILD_MODE_STRING)
CRISP_INSTALL_ROOT=/home/gmarpons/tmp/crisp/$(BUILD_MODE_STRING)/
LIB_PREFIX=lib


all:: hicpp_3_3_13.bc
all:: hicpp_3_4_2.bc

.PRECIOUS: %.ll		# To prevent make deleting intermediate files.
%.ll: %.cpp
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/clang++ -cc1                         \
		-g								\
		-load $(CRISP_INSTALL_ROOT)/lib/$(LIB_PREFIX)crispclang.so      \
		-add-plugin crisp-clang -plugin-arg-crisp-clang SomeHICPPrules  \
		-emit-llvm -fno-merge-all-constants $<                          \
		# -plugin-arg-crisp-clang -debug

%.bc: %.ll
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/opt -analyze                         \
		-load $(CRISP_INSTALL_ROOT)/lib/$(LIB_PREFIX)crispllvm.so       \
		-crisp-mod -crisp-rules-file SomeHICPPrules                     \
		-basicaa $< -o $@

.PHONY: clean clang-help opt-help
clean:
	rm -f *.ll *.bc *.pl *.s *.diags

clang-help:
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/clang++ -cc1                         \
		-load $(CRISP_INSTALL_ROOT)/lib/$(LIB_PREFIX)crispclang.so      \
		-help

opt-help:
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/opt                                  \
		-load $(CRISP_INSTALL_ROOT)/lib/$(LIB_PREFIX)crispllvm.so       \
		-help
