:- multifile violation/2.
:- multifile violation/3.
:- multifile violationCandidate/2.

%% Argument is 'DebugFlag'.
init_msg(true) :-
        write('Initializing Prolog engine.'), nl.
init_msg(false).

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

runTranslationUnitAnalysis(TUMainFileName) :-
        clangFactsFileName(TUMainFileName, ClangFactsFileName),
        writeAllViolationCandidates(ClangFactsFileName),
        report_all_violations.

clangFactsFileName(TUMainFileName, PrologName) :-
        file_base_name(TUMainFileName, CppName),
        file_name_extension(Base, _, CppName),
        file_name_extension(Base, pl, PrologName).

writeAllViolationCandidates(FileName) :-
        open(FileName, write, Stream),
        forall(violationCandidate(Rule, Culprits),
               portray_clause(Stream, violationCandidate(Rule, Culprits))),
        close(Stream).

report_all_violations :-
        forall(violation(Rule, Message, Culprits),
               report_violation(Rule, Message, Culprits)).
