%% PrologBootForCrispClangPlugin.pl -------------------------------*- Prolog -*-

%% Copyright (C) 2011, 2012 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
%%
%% This file is part of Crisp.
%%
%% Crisp is free software: you can redistribute it and/or modify
%% it under the terms of the GNU General Public License as published by
%% the Free Software Foundation, either version 3 of the License, or
%% (at your option) any later version.
%%
%% Crisp is distributed in the hope that it will be useful,
%% but WITHOUT ANY WARRANTY; without even the implied warranty of
%% MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
%% GNU General Public License for more details.
%%
%% You should have received a copy of the GNU General Public License
%% along with Crisp.  If not, see <http://www.gnu.org/licenses/>.

:- multifile violation/3.
:- multifile violation_candidate/2.
:- multifile violation_llvm/3.

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
        forall(violation_candidate(Rule, Culprits),
               portray_clause(Stream, violation_candidate(Rule, Culprits))),
        close(Stream).

report_all_violations :-
        forall(violation(Rule, Message, Culprits),
               report_violation(Rule, Message, Culprits)).
