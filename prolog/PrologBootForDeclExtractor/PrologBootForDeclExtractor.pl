init_msg.
        % write('Initializing Prolog engine.'), nl.

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
               (write_term(Stream, Decl, [quoted(false)]), nl(Stream))),
        close(Stream).

directBase(Class, Base) :-
        'CXXRecordDecl::definition'(Class),
        'CXXRecordDecl::base'(Class, BaseSpecifier),
        'CXXBaseSpecifier::baseDecl'(BaseSpecifier, Base).

base(Class, Base) :-
        directBase(Class, Base).
base(Class, Base) :-
        directBase(Class, Aux), base(Aux, Base).

simpleName(Name, Simple) :-
        ( atom_concat(is, Suffix, Name)
        ; atom_concat(has, Suffix, Name)
        ),
        !,
        atom_codes(Suffix, [U1|Tail1]),
        (  Tail1 \= [],
           Tail1 = [U2|_Tail2],
           char_type(U2, upper) % Suffix begins with acronym
        -> Simple = Suffix
        ;  to_lower(U1, L),
           atom_codes(Simple, [L|Tail1])
        ).
simpleName(Name, Name).

% interestingDecl(pl_get_one(Name, ArgTypeStr, ResTypeStr, Name)) :-
%         isA(Class, 'CXXRecordDecl'),
%         base(Class, Base),
%         nameAsString(Base, 'Decl'),
%         method(Class, Method),
%         constQualified(Method),
%         numParams(Method, 0),
%         resultType(Method, ResType),
%         asString(ResType, ResTypeStr),
%         nameAsString(Class, ArgTypeStr),
%         nameAsString(Method, Name).

        % isA(Method, 'CXXMethodDecl'),
        % constQualified(Method),
        % declContext(Method, Class),

interestingDecl(pl_check_property(SimpleName, ClassName, MethodName)) :-
        isA(Class, 'CXXRecordDecl'),
        base(Class, Base),
        'NamedDecl::nameAsString'(Base, 'Decl'),
        'CXXRecordDecl::method'(Class, Method),
        'CXXMethodDecl::constQualified'(Method),
        \+ 'CXXMethodDecl::static'(Method),
        'FunctionDecl::numParams'(Method, 0),
        'Decl::access'(Method, public),
        'FunctionDecl::resultType'(Method, ResType),
        'CXXMethodDecl::size_overridden_methods'(Method, 0),
        'QualType::asString'(ResType, '_Bool'),
        'NamedDecl::nameAsString'(Class, ClassName),
        'NamedDecl::nameAsString'(Method, MethodName),
        simpleName(MethodName, SimpleName).

        % atomic_list_concat([ClassName, '_', MethodName], Name).
