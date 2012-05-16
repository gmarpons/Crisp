# Variables you need to set to run this Makefile:
#     LLVM_OBJ_ROOT=
#     BUILD_MODE=
#     CRISP_INSTALL_ROOT=

all:: hicpp_3_3_13.bc
all:: hicpp_3_4_2.bc

%.ll: %.cpp
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/clang++ -cc1                         \
		-load $(CRISP_INSTALL_ROOT)/lib/crispclang.so                   \
		-add-plugin crisp-clang -plugin-arg-crisp-clang SomeHICPPrules  \
		-emit-llvm $<

%.bc: %.ll
	$(LLVM_OBJ_ROOT)/$(BUILD_MODE)/bin/opt -analyze                         \
		-load $(CRISP_INSTALL_ROOT)/lib/crispllvm.so                    \
		-crisp-mod -crisp-rules-file SomeHICPPrules                     \
		-basicaa $< -o $@

.PHONY: clean
clean:
	rm -f *.ll *.bc *.pl *.s
