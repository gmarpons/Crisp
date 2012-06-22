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
               portray_clause(Stream, violation_candidate_(Rule, Culprits))),
        close(Stream).

report_all_violations :-
        forall(violation(Rule, Message, Culprits),
               report_violation(Rule, Message, Culprits)).

%% The following clause discards symbols (constants) that are inlined
%% in LLVM IR code, and as a result do not always appear there as
%% named symbols.
% llvm_name(Decl, _) :-           % Global constants
%         is_inlined_in_llvm(Decl),
%         !,
%         write('Ignoring variable inlined in LLVM IR: '),
%         'NamedDecl::name'(Decl, Name),
%         write(Name),
%         nl,
%         fail.
%% FIXME: what to do with names hidden by an inner declaration
%% context? Debug info needed.
% llvm_name(Decl, _) :-
%         has_name_clash(Decl),
%         !,
%         write('Ignoring variable with a name clash: '),
%         'NamedDecl::name'(Decl, Name),
%         write(Name),
%         nl,
%         fail.
%% Clause for functions.
llvm_name(Decl, LlvmName) :-
        %% FIXME: The 4 following cases are necessary because concept
        %% subsumtion is not implemented yet.
        ( isA(Decl, 'FunctionDecl')
        ; isA(Decl, 'CXXMethodDecl')
        ; isA(Decl, 'CXXConstructorDecl')
        ; isA(Decl, 'CXXDestructorDecl')
        ; isA(Decl, 'CXXConversionDecl')
        ),
        !,
        llvm_function_name(Decl, LlvmFuncName),
        LlvmName = function(LlvmFuncName).
%% Clause for static function/class variables and globals into a
%% namespace. Static function/class variables are represented with a
%% global_var/1 term because they are declared globally in LLVM IR.
llvm_name(Decl, LlvmName) :-
        isA(Decl, 'VarDecl'),
        ( 'VarDecl::is_staticLocal'(Decl)
        ; 'VarDecl::is_staticDataMember'(Decl)
        ; 'Decl::declContext'(Decl, DeclContext),
          isA(DeclContext, 'NamespaceDecl')
        ),
        !,
        mangleName(Decl, MangledName),
        LlvmName = global_var(MangledName).
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
        'NamedDecl::name'(Decl, Name),
        'Decl::declContext'(Decl, FuncContext),
        llvm_function_name(FuncContext, FuncName),
        LlvmName = local_var(FuncName, Name).
%% Clause for global constant variables that have not external
%% linkage. In these cases LLVM use mangled names.
llvm_name(Decl, LlvmName) :-
        isA(Decl, 'VarDecl'),
        'VarDecl::has_globalStorage'(Decl),
        'ValueDecl::type'(Decl, Type),
        'QualType::canonicalType'(Type, CanonicalType),
        'QualType::is_constQualified'(CanonicalType),
        \+ 'VarDecl::has_externalStorage'(Decl),
        !,
        mangleName(Decl, MangledName),
        LlvmName = global_var(MangledName).
%% Clause for global non-constant variables or constants with external
%% linkage.
llvm_name(Decl, LlvmName) :-
        isA(Decl, 'VarDecl'),
        'VarDecl::has_globalStorage'(Decl),
        !,
        'NamedDecl::name'(Decl, Name),
        LlvmName = global_var(Name).

%% Given a declaration for a function, returns the LLVM IR name for
%% that function.
llvm_function_name(FunctionDecl, LlvmName) :-
        mangleName(FunctionDecl, MangledName),
        (  MangledName = '_Z4main'
        -> 'NamedDecl::name'(FunctionDecl, LlvmName)
        ;  LlvmName = MangledName
        ).

%% FIXME: mangleName/2 is not implemented for constructors and
%% destructors, fireing a runtime error. Use
%% [lib/CodeGen/CodeGenModule.cpp]CodeGenModule::getMangledName.
% llvm_name(Decl, LlvmName) :-
%         (  needs_mangling(Decl)
%         -> mangleName(Decl, LlvmName)
%         ;  'NamedDecl::name'(Decl, LlvmName)
%         ).

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

% needs_mangling(Decl) :- isA(Decl, 'FunctionDecl'), !.
% %% FIXME: The 4 following cases are necessary because concept
% %% subsuming is not implemented yet.
% needs_mangling(Decl) :- isA(Decl, 'CXXMethodDecl'), !.
% needs_mangling(Decl) :- isA(Decl, 'CXXConstructorDecl'), !.
% needs_mangling(Decl) :- isA(Decl, 'CXXDestructorDecl'), !.
% needs_mangling(Decl) :- isA(Decl, 'CXXConversionDecl'), !.
% needs_mangling(Decl) :-
%         isA(Decl, 'VarDecl'), 'VarDecl::is_staticLocal'(Decl), !.
% needs_mangling(Decl) :-
%         isA(Decl, 'VarDecl'), 'VarDecl::is_staticDataMember'(Decl), !.
