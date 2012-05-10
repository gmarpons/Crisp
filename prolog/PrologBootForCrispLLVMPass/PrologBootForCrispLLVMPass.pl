:- multifile violation/2.

init_msg :-
        write('Initializing Prolog engine.'), nl.

welcome_msg_and_prolog :-
        write('Welcome to the Crisp interactive interface!'), nl,
        write('Enter Ctrl-D to exit.'), nl,
        % set_prolog_flag(verbose, normal),
        prolog.

load_file(FileBaseName, RulesDir) :-
        set_prolog_flag(verbose_file_search, true),
        assertz(file_search_path(rules, '.')),
        assertz(file_search_path(rules, RulesDir)),
        file_name_extension(FileBaseName, pl, FileName),
        absolute_file_name(rules(FileName), AbsoluteFileName),
        access_file(AbsoluteFileName, read),
        ensure_loaded(rules(FileName)).

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
        (  'Value::use'(Value, User),
           ( isA_(User, 'StoreInst')
           ; isA_(User, 'LoadInst')
           )
        -> (  isA_(User, 'StoreInst')
           -> getLocationFromStoreUser(User, Location)
           ;  getLocationFromLoadUser(User, Location)
           )
        ;  createLocation(Value, Location)
        ).
