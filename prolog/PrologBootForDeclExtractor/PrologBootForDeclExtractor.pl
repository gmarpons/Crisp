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

toLowerFirst(Atom, ToLowerFirst) :-
        atom_codes(Atom, [U1|Tail1]),
        (  Tail1 \= [],
           Tail1 = [U2|_Tail2],
           char_type(U2, upper) % Atom begins with acronym
        -> ToLowerFirst = Atom
        ;  to_lower(U1, L),
           atom_codes(ToLowerFirst, [L|Tail1])
        ).

simpleName(Name, Simple) :-
        atom_concat(get, Suffix, Name),
        !,
        toLowerFirst(Suffix, Simple).
simpleName(Name, Simple) :-
        ( atom_concat(is, Suffix, Name)
        ; atom_concat(has, Suffix, Name)
        ),
        !,
        toLowerFirst(Suffix, Simple).
simpleName(Name, Name).

topClassName('Decl').
topClassName('QualType').
topClassName('Type').
topClassName('Stmt').
% topClassName('ASTContext').

topClass(Class) :-
        isA(Class, 'CXXRecordDecl'),
        'NamedDecl::nameAsString'(Class, Name),
        topClassName(Name).

topClassHeir(Class) :-
        topClass(Class).
topClassHeir(Class) :-
        topClass(Top),
        isA(Class, 'CXXRecordDecl'),
        base(Class, Top).

candidateMethod(Class, Method) :-
        'CXXRecordDecl::method'(Class, Method),
        'CXXMethodDecl::constQualified'(Method),
        \+ 'CXXMethodDecl::static'(Method),
        'Decl::access'(Method, public),
        'FunctionDecl::numParams'(Method, 0),
        'CXXMethodDecl::size_overridden_methods'(Method, 0).

isOperatorName(Name) :-
        atom_concat(operator, _, Name).

removeSubAtom(Atom, SubAtom, Result) :-
        atom_length(SubAtom, L),
        (  sub_atom(Atom, Before, L, _, SubAtom)
        -> ( sub_atom(Atom, 0, Before, _, Prefix),
             After is Before + L,
             sub_atom(Atom, After, _, 0, Suffix),
             atom_concat(Prefix, Suffix, Result)
           )
        ;  Result = Atom
        ).

isInterestingResultType(_, 'clang::QualType').
isInterestingResultType(_, 'const clang::QualType').
isInterestingResultType(Type, TypeName) :-
        'Type::builtinType'(Type),
                                % filter out modifier functions
        TypeName \= 'void',
                                % filter out property checkers
        TypeName \= '_Bool'.
isInterestingResultType(Type, _) :-
        'Type::pointerType'(Type),
        'PointerType::pointeeType'(Type, PointeeType),
        'QualType::canonicalType'(PointeeType, CanonicalPointeeType),
        'QualType::typePtr'(CanonicalPointeeType, PointeeTypePtr),
        'Type::recordType'(PointeeTypePtr),
        'TagType::decl'(PointeeTypePtr, Record),
        topClassHeir(Record).

interestingDecl(pl_get_one(SimpleName, ClassName, ResTypeName, MethodName)) :-
        topClassHeir(Class),
        candidateMethod(Class, Method),
        'NamedDecl::nameAsString'(Method, MethodName),
        \+ isOperatorName(MethodName),
        'FunctionDecl::resultType'(Method, ResType),
        'QualType::asString'(ResType, ResTypeNameAux),
                                % remove "class " and "struct " from type name
        removeSubAtom(ResTypeNameAux, 'class ', ResTypeNameAux2),
        removeSubAtom(ResTypeNameAux2, 'struct ', ResTypeName),
                                % filter out iterators
        \+ sub_atom(ResTypeName, _, _, _, iterator),
        'QualType::canonicalType'(ResType, CanonicalResType),
        'QualType::typePtr'(CanonicalResType, ResTypePtr),
        isInterestingResultType(ResTypePtr, ResTypeName),
        'NamedDecl::nameAsString'(Class, ClassName),
        simpleName(MethodName, SimpleName).
interestingDecl(pl_check_property(SimpleName, ClassName, MethodName)) :-
        topClassHeir(Class),
        candidateMethod(Class, Method),
        'FunctionDecl::resultType'(Method, ResType),
        'QualType::asString'(ResType, '_Bool'),
        'NamedDecl::nameAsString'(Class, ClassName),
        'NamedDecl::nameAsString'(Method, MethodName),
        simpleName(MethodName, SimpleName).
