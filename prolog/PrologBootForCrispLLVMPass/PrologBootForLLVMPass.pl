init_msg :-
        write('Initializing Prolog engine.'), nl.

welcome_msg_and_prolog :-
        write('Welcome to the Crisp interactive interface!'), nl,
        write('Enter Ctrl-D to exit.'), nl,
        % set_prolog_flag(verbose, normal),
        prolog.

llvmFactsFileName(PlName) :-
        llvmModuleFileName(AbsPathName),
        file_base_name(AbsPathName, CppName),
        file_name_extension(Base, _, CppName),
        file_name_extension(Base, pl, PlName).

