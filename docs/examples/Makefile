# Variables you need to set to run this Makefile:
#    LLVM_OBJ_ROOT=
#    BUILD_MODE=
#    CRISP_INSTALL_ROOT=

LLVM_OBJ_ROOT=$(HOME)/llvm-obj
BUILD_MODE=Debug+Asserts
CRISP_INSTALL_ROOT=$(HOME)/llvm-obj/projects/crisp/Debug+Asserts

all:: hicpp_3_3_13.bc
all:: hicpp_3_4_2.bc

.PRECIOUS: %.ll		# To prevent make deleting intermediate files.
%.ll: %.cpp
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/clang++ -cc1                         \
		-load $(CRISP_INSTALL_ROOT)/lib/crispclang.so                   \
		-add-plugin crisp-clang -plugin-arg-crisp-clang SomeHICPPrules  \
		-emit-llvm -fno-merge-all-constants -O0 $<

%.bc: %.ll
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/opt -analyze                         \
		-load $(CRISP_INSTALL_ROOT)/lib/crispllvm.so                    \
		-crisp-mod -crisp-rules-file SomeHICPPrules                     \
		-basicaa $< -o $@

.PHONY: clean clang-help opt-help
clean:
	rm -f *.ll *.bc *.pl *.s

clang-help:
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/clang++ -cc1 -help

opt-help:
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/opt -help
