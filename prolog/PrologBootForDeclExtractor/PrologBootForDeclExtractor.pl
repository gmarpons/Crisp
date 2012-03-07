init_msg :-
        write('Initializing Prolog engine.'), nl.

welcome_msg_and_prolog :-
        write('Welcome to the Crisp interactive interface!'), nl,
        write('Enter Ctrl-D to exit.'), nl,
        % set_prolog_flag(verbose, normal),
        prolog.

runTranslationUnitAnalysis(TUMainFileName) :-
        clangFactsFileName(TUMainFileName, ClangFactsFileName),
        writeAllInterestingDecls(ClangFactsFileName).

clangFactsFileName(TUMainFileName, PrologName) :-
        file_base_name(TUMainFileName, CppName),
        file_name_extension(Base, _, CppName),
        file_name_extension(Base, inc, PrologName).

writeAllInterestingDecls(FileName) :-
        open(FileName, write, Stream),
        forall(interestingDecl(Decl),
               portray_clause(Stream, interestingDecl(Decl))),
        close(Stream).

interestingDecl(pl_get_one(Name, ArgType, restype, Name)) :-
        isA(Method, 'CXXMethodDecl'),
        declContext(Method, Class),
        nameAsString(Class, ArgType),
        nameAsString(Method, Name).