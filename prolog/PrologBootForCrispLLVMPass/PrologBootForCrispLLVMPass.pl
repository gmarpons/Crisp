init_msg :-
        write('Initializing Prolog engine.'), nl.

welcome_msg_and_prolog :-
        write('Welcome to the Crisp interactive interface!'), nl,
        write('Enter Ctrl-D to exit.'), nl,
        % set_prolog_flag(verbose, normal),
        prolog.

readModuleFacts(ModuleFileName) :-
        llvmFactsFileName(ModuleFileName, LLVMFactsFileName),
        readAllViolationCandidates(LLVMFactsFileName).

llvmFactsFileName(ModuleFileName, PrologName) :-
        file_base_name(ModuleFileName, LlvmName),
        file_name_extension(Base, _, LlvmName),
        file_name_extension(Base, pl, PrologName).

readAllViolationCandidates(FileName) :-
        ensure_loaded(FileName).

%% FIXME: the following code doesn't belongs here

violation('HICPP 3.4.2', [Func]) :-
        violationCandidate('HICPP 3.4.2', [Func]),
        hasArgument(Func, This), % Uses an iterator
        hasName(This, "this"),
        hasInstruction(Func, StoreThis), % Uses an iterator
        isA(StoreThis, 'StoreInst'),
        getValueOperand(StoreThis, This),
        getPointerOperand(StoreThis, ThisAddr),
        hasIntruction(Func, LoadThis),
        isA(LoadThis, 'LoadInst'),
        getPointerOperand(LoadThis, ThisAddr),
        hasInstruction(Func, OffsetFromThis),
        ( isA(OffsetFromThis, 'GetElementPtrInst'),
          getPointerOperand(OffsetFromThis, LoadThis)
        ; isA(OffsetFromThis, 'BitCastInst'),
          hasOperand(OffsetFromThis, LoadThis) % Uses an iterator
        ),
                                % OffsetFromThis has necessarily
                                % pointer type, so it has a location.
        hasLocation(OffsetFromThis, OffsetFromThisLoc),
        hasInstruction(Func, Return),
        isA(Return, 'ReturnInst'),
                                % That Func returns pointer (Clang
                                % knows) guarantees there is a
                                % location for 'Return'
        hasLocation(Return, ReturnLoc),
        aliasLessThan(ReturnLoc, OffsetFromThisLoc, 'NoAlias').
