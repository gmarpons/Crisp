%%
%% Initial goals.
%%

%% To be executed as initial goal when clang predicates are to be
%% extracted. top_class_name/1 identifies the name of a class on top
%% of hierarchy of classes to analyse.
init_clang_base_types :-
        assertz(top_class_name('Decl')),
        assertz(top_class_name('QualType')),
        assertz(top_class_name('Type')),
        assertz(top_class_name('Stmt')).

%% To be executed as initial goal when LLVM predicates are to be
%% extracted. top_class_name/1 identifies the name of a class on top
%% of hierarchy of classes to analyse.
init_llvm_base_types :-
        assertz(top_class_name('Value')).

welcome_msg_and_prolog :-
        write('Welcome to the Crisp interactive interface!'), nl,
        write('Enter Ctrl-D to exit.'), nl,
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


%%
%%
%%

top_class(Class) :-
        isA(Class, 'CXXRecordDecl'),
        'NamedDecl::nameAsString'(Class, Name),
        top_class_name(Name).

top_class_heir(Class) :-
        top_class(Class).
top_class_heir(Class) :-
        top_class(Top),
        isA(Class, 'CXXRecordDecl'),
        base(Class, Top).

direct_base(Class, Base) :-
        'CXXRecordDecl::has_definition'(Class),
        'CXXRecordDecl::base'(Class, BaseSpecifier),
        BaseSpecifier \= 0,
        'CXXBaseSpecifier::type'(BaseSpecifier, Type),
        'QualType::canonicalType'(Type, CanonicalType),
        'QualType::typePtr'(CanonicalType, TypePtr),
        'Type::is_recordType'(TypePtr),
        'TagType::decl'(TypePtr, Decl),
        'RecordDecl::definition'(Decl, Base).

base(Class, Base) :-
        direct_base(Class, Base).
base(Class, Base) :-
        direct_base(Class, Aux), base(Aux, Base).


%%
%%
%%

toLowerFirst(Atom, ToLowerFirst) :-
        atom_codes(Atom, [U1|Tail1]),
        (  Tail1 \= [],
           Tail1 = [U2|_Tail2],
           char_type(U2, upper) % Atom begins with acronym
        -> ToLowerFirst = Atom
        ;  to_lower(U1, L),
           atom_codes(ToLowerFirst, [L|Tail1])
        ).

remove_get(Name, Result) :-
        atom_concat(get, Suffix, Name),
        !,
        toLowerFirst(Suffix, Result).
remove_get(Name, Name).

%% Fails if Name doesn't begin with neither 'is' nor 'has'.
remove_is_or_has(Name, (IsOrHas, NewName)) :-
        ( atom_concat(is, Suffix, Name) -> IsOrHas = is
        ; atom_concat(has, Suffix, Name) -> IsOrHas = has
        ),
        toLowerFirst(Suffix, NewName).

candidateMethod(Class, Method) :-
        'CXXRecordDecl::method'(Class, Method),
        'CXXMethodDecl::constQualified'(Method),
        \+ 'CXXMethodDecl::is_static'(Method),
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
isInterestingResultType(_, 'llvm::StringRef').
isInterestingResultType(Type, TypeName) :-
        'Type::is_builtinType'(Type),
                                % filter out modifier functions
        TypeName \= 'void',
                                % filter out property checkers
        TypeName \= '_Bool'.
isInterestingResultType(Type, _) :-
        'Type::is_pointerType'(Type),
        'PointerType::pointeeType'(Type, PointeeType),
        'QualType::canonicalType'(PointeeType, CanonicalPointeeType),
        'QualType::typePtr'(CanonicalPointeeType, PointeeTypePtr),
        'Type::is_recordType'(PointeeTypePtr),
        'TagType::decl'(PointeeTypePtr, Record),
        top_class_heir(Record).

%% Case already qualified
qualifiedMemberName(_Class, Member, QualifiedMember) :-
        sub_atom(Member, _, _, _, '::'),
        !,
        QualifiedMember = Member.
%% Case non-qualified
qualifiedMemberName(Class, Member, QualifiedMember) :-
        atom_concat(Class, '::', Aux),
        atom_concat(Aux, Member, QualifiedMember).

interestingDecl(pl_get_one(Name, ArgType, ResTypeName, CXXName)) :-
        top_class_heir(Class),
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
        'NamedDecl::nameAsString'(Class, ArgType),
        remove_get(MethodName, Name),
        qualifiedMemberName(ArgType, MethodName, CXXName).
interestingDecl(pl_check_property(Verb, Name, ArgType, CXXName)) :-
        top_class_heir(Class),
        candidateMethod(Class, Method),
        'FunctionDecl::resultType'(Method, ResType),
        'QualType::asString'(ResType, '_Bool'),
        'NamedDecl::nameAsString'(Class, ArgType),
        'NamedDecl::nameAsString'(Method, MethodName),
        remove_is_or_has(MethodName, (Verb, Name)),
        qualifiedMemberName(ArgType, MethodName, CXXName).
interestingDecl(pl_get_many(Name, ArgType, ItType, ItBegin, ItEnd)) :-
        top_class_heir(Class),
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
        qualifiedMemberName(ArgType, ResTypeName, ItType),

                                % FIXME: ConstCaseIt needs a constructor
                                % and an iterator_traits specialization.
        ItType \= 'SwitchInst::ConstCaseIt',

        atom_concat(ArgType, '::', Qualification),
        atom_concat(Qualification, BeginMethodName, ItBegin),
        atom_concat(Qualification, EndMethodName, ItEnd).
