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

%% Modes:
%% 1. (Module, 'Module')
%% 2. (Instruction+, Sort)
%% 'Sort' is an atom.
isA_(Module, Sort) :-
        isA(Module, Sort), !.
isA_(Instruction, Sort) :-
        isA_computed(Instruction, Sort).

%% Pre: +Value has PointerType.
getLocation(Value, Location) :-
        (  containsUse(Value, User),
           ( isA_(User, 'StoreInst')
           ; isA_(User, 'LoadInst')
           )
        -> (  isA_(User, 'StoreInst')
           -> getLocationFromStoreUser(User, Location)
           ;  getLocationFromLoadUser(User, Location)
           )
        ;  createLocation(Value, Location)
        ).

%% FIXME: the following code doesn't belongs here

violation('HICPP 3.4.2', [FuncName], AliasResult) :-
        violationCandidate('HICPP 3.4.2', [FuncName]),
        isA_(Module, 'Module'),
        getFunction(Module, FuncName, Func),
        containsArgument(Func, This),
        getName(This, 'this'),
        containsInstruction(Func, StoreThis),
        isA_(StoreThis, 'StoreInst'),
        getValueOperand(StoreThis, This),
        getPointerOperand(StoreThis, ThisAddr),
        containsInstruction(Func, LoadThis),
        isA_(LoadThis, 'LoadInst'),
        getPointerOperand(LoadThis, ThisAddr),
        containsInstruction(Func, OffsetFromThis),
        ( isA_(OffsetFromThis, 'GetElementPtrInst'),
          getPointerOperand(OffsetFromThis, LoadThis)
        ; isA_(OffsetFromThis, 'BitCastInst'),
          containsOp(OffsetFromThis, LoadThis)
        ),
        getLocation(OffsetFromThis, OffsetFromThisLoc),
        containsInstruction(Func, Return),
        isA_(Return, 'ReturnInst'),
        containsOp(Return, UsedByReturn),
        getLocation(UsedByReturn, ReturnLoc),
        alias(OffsetFromThisLoc, ReturnLoc, AliasResult).
