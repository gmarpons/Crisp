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
isA_(Entity, Sort) :-
        ( isA(Entity, Sort)          % Only for Modules
        ; isA_computed(Entity, Sort) % At time being, only for Instructions
        ).

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

violation('HICPP 3.4.2', [Func]) :-
        violationCandidate('HICPP 3.4.2', [FuncName]),
        isA_(Module, 'Module'),
                                % 'violationCandidate' guarantees that
                                % 'FuncName' is the name of an
                                % LLVMFunction.
        getFunction(Module, FuncName, Func),
        containsArgument(Func, This), % Uses an iterator
        getName(This, 'this'),
        containsInstruction(Func, StoreThis), % Uses an iterator
        isA_(StoreThis, 'StoreInst'),
        getValueOperand(StoreThis, This),
        getPointerOperand(StoreThis, ThisAddr),
        containsIntruction(Func, LoadThis), % Uses an iterator
        isA_(LoadThis, 'LoadInst'),
        getPointerOperand(LoadThis, ThisAddr),
        containsInstruction(Func, OffsetFromThis), % Uses an iterator
        ( isA_(OffsetFromThis, 'GetElementPtrInst'),
          getPointerOperand(OffsetFromThis, LoadThis)
        ; isA_(OffsetFromThis, 'BitCastInst'),
          containsOp(OffsetFromThis, Use), % Uses an iterator
          get(Use, LoadThis)               % Something to hide in Crisp
        ),
                                % OffsetFromThis has necessarily
                                % pointer type, so it has a location.
        getLocation(OffsetFromThis, OffsetFromThisLoc),
        containsInstruction(Func, Return), % Uses an iterator
        isA_(Return, 'ReturnInst'),
                                % 'violationCandidate' guarantees that
                                % Func returns a pointer, so there is
                                % a location for 'Return'
        getLocation(Return, ReturnLoc),
        aliasLessThan(ReturnLoc, OffsetFromThisLoc, 'NoAlias').
