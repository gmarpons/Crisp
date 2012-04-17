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

%% Case already qualified
qualifiedTypeName(_, TypeName, QualifiedTypeName) :-
        sub_atom(TypeName, _, _, _, '::'),
        !,
        QualifiedTypeName = TypeName.
%% Case non-qualified
qualifiedTypeName(Class, TypeName, QualifiedTypeName) :-
        atom_concat(Class, '::', Aux),
        atom_concat(Aux, TypeName, QualifiedTypeName).

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
interestingDecl(pl_get_many(Name, ArgType, ItType, ItBegin, ItEnd, CXXName)) :-
        topClassHeir(Class),
        candidateMethod(Class, BeginMethod),
        'NamedDecl::nameAsString'(BeginMethod, BeginMethodName),
        sub_atom(BeginMethodName, Before, 5, After, begin),
        (  Before == 0
        -> ( sub_atom(BeginMethodName, 5, After, 0, NameAux),
             atom_concat(end, NameAux, EndMethodName),
             atom_concat('_', Name, NameAux)
           )
        ;  ( sub_atom(BeginMethodName, 0, Before, 5, NameAux),
             atom_concat(NameAux, end, EndMethodName),
             atom_concat(Name, '_', NameAux)
           )
        ),
        candidateMethod(Class, EndMethod),
        'NamedDecl::nameAsString'(EndMethod, EndMethodName),
        'NamedDecl::nameAsString'(Class, ArgType),
        'FunctionDecl::resultType'(BeginMethod, ResType),

                                % FIXME: ConstExprIterator needs an
                                % iterator_traits specialization.
        'QualType::canonicalType'(ResType, CanonicalResType),
        'QualType::asString'(CanonicalResType, CanonicalResTypeName),
        CanonicalResTypeName \= 'class clang::ConstExprIterator',

        'QualType::asString'(ResType, ResTypeNameAux),
                                % remove "class " and "struct " from type name
        removeSubAtom(ResTypeNameAux, 'class ', ResTypeNameAux2),
        removeSubAtom(ResTypeNameAux2, 'struct ', ResTypeName),
        qualifiedTypeName(ArgType, ResTypeName, ItType),
        atom_concat(ArgType, '::', Qualification),
        atom_concat(Qualification, BeginMethodName, ItBegin),
        atom_concat(Qualification, EndMethodName, ItEnd),
        CXXName = Name.
