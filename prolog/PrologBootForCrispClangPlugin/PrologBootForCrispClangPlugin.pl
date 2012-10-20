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

:- use_module(library(lists), [append/2]). % Needed by rule files

:- multifile violation/3.
% :- multifile violation_candidate/2.
:- multifile violation_llvm/3.

%% Argument is 'DebugFlag'.
init_msg(true) :-
        write('Initializing Prolog engine.'), nl.
init_msg(false).

%% TODO: factorize with identical pred. in PrologBootForDeclExtractor.pl
welcome_msg_and_prolog :-
        write('Welcome to the Crisp interactive interface!'), nl,
        write('Enter Ctrl-D to exit.'), nl,
        prolog.

load_file(FileBaseName, RulesDir) :-
        set_prolog_flag(verbose_file_search, true),
        assertz(file_search_path(rules, '.')),
        assertz(file_search_path(rules, RulesDir)),
        file_name_extension(FileBaseName, pl, FileName),
        absolute_file_name(rules(FileName), AbsoluteFileName),
        access_file(AbsoluteFileName, read),
        ensure_loaded(rules(FileName)).

run_translation_unit_analysis(TUMainFileName) :-
        atom_concat(TUMainFileName, '.pl', ViolationCandidatesFileName),
        report_all_violations(ViolationCandidatesFileName).

report_all_violations(FileName) :-
        open(FileName, write, _Stream, [alias(violation_candidates_stream)]),
        forall(violation(Rule, Diagnostics, EmitLlvmFacts),
               (report_violation(Rule, Diagnostics, EmitLlvmFacts))),
        close(violation_candidates_stream).

%% This predicate is called from C++ as part of report_violation/2.
write_violation_candidate(Rule, Culprits, DiagsOffsetFinal, DiagsLength) :-
        reverse(Culprits, CulpritsRev),
        D = diags_file_segment(DiagsOffsetFinal, DiagsLength),
        portray_clause(violation_candidates_stream,
                       violation_candidate(Rule, CulpritsRev, D)).

%% TODO: take into accout (e.g. with a warning) symbols (constants)
%% that are inlined in LLVM IR code and, as a result, do not always
%% appear there as named symbols. The following predicate defines
%% symbols that *can* be inlined.
% is_inlined_in_llvm(Decl) :-
%         isA(Decl, 'VarDecl'),
%         'ValueDecl::type'(Decl, Type),
%         'QualType::canonicalType'(Type, CanonicalType),
%         'QualType::is_constQualified'(CanonicalType),
%         'VarDecl::has_globalStorage'(Decl),
%         'QualType::typePtr'(CanonicalType, TypePointer),
%         \+ isA(TypePointer, 'PointerType'),
%         \+ 'VarDecl::has_externalStorage'(Decl),
%         \+ 'VarDecl::is_staticDataMember'(Decl).

%% TODO: use debug info to handle variable name clashes.

%% Clause for functions.
llvm_name(Decl, LlvmName) :-
        %% FIXME: The 4 following cases are necessary because concept
        %% subsumtion is not implemented yet.
        ( isA(Decl, 'FunctionDecl')
        ; isA(Decl, 'CXXMethodDecl')
        ; isA(Decl, 'CXXConversionDecl')
        ),
        !,
        llvmName(Decl, Name),
        LlvmName = function(Name).
%% Clause for constructors and destructors
llvm_name(Decl, LlvmName) :-
        ( isA(Decl, 'CXXConstructorDecl')
        ; isA(Decl, 'CXXDestructorDecl')
        ),
        !,
        llvmName(Decl, MangledName), % Returns mangled name for
                                     % 'complete' structor.
        'NamedDecl::nameAsString'(Decl, Name),
        atom_codes(MangledName, MangledNameCodes),
        reverse(MangledNameCodes, ReversedMangledNameCodes),
        atom_codes(ReversedMangledName, ReversedMangledNameCodes),
        atom_concat(Name, 'C1', C1Name),
        atom_codes(C1Name, C1NameCodes),
        reverse(C1NameCodes, ReversedC1NameCodes),
        atom_codes(ReversedC1Name, ReversedC1NameCodes),
        atom_length(C1Name, C1Length),
        sub_atom(ReversedMangledName, Start, C1Length, After, ReversedC1Name),
        !,
        sub_atom(MangledName, 0, After, _, NamePrefixAux),
        sub_atom(MangledName, _, Start, 0, NameSuffix),
        atom_concat(NamePrefixAux, Name, NamePrefix),
        LlvmName = structor(NamePrefix, NameSuffix).
%% Clause for global variables.
llvm_name(Decl, LlvmName) :-
        isA(Decl, 'VarDecl'),
        'VarDecl::has_globalStorage'(Decl),
        !,
        llvmName(Decl, Name),
        LlvmName = global_var(Name).
%% Clause for local automatic variables and function arguments.
%% FIXME: some cases need to be eliminated when concept subsumption is
%% implemented.
llvm_name(Decl, LlvmName) :-
        ( isA(Decl, 'ParmVarDecl')
        ; isA(Decl, 'ImplicitParamDecl')
        ; isA(Decl, 'VarDecl'),
          'VarDecl::is_localVarDecl'(Decl)
        ),
        !,
        llvmName(Decl, Name),
        'Decl::declContext'(Decl, FuncContext),
        llvmName(FuncContext, FuncName),
        LlvmName = local_var(FuncName, Name).
