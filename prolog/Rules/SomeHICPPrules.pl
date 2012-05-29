%% This file contains formalizations in both CRISP and Prolog of some
%% coding rules of the High Integrity C++ Coding Standard (HICPP).
%%
%% Rules in HICPP (and most of other standard rule sets) are defined
%% in English, which bears ambiguity problems. Here we show the
%% natural language definition of every rule, and its justification
%% (both taken from
%% http://www.codingstandard.com/HICPPCM/index.html). A possible
%% formalization of the rules in CRISP is shown, to give a taste of
%% the language, but the CRISP compiler is not implemented, yet. The
%% only executable parts of this file are the Prolog formalization of
%% the rules, that in the future will be automatically generate from
%% CRISP.


%% HICPP 3.3.13
%% ============
%%
%% URL: http://www.codingstandard.com/HICPPCM/High_Integrity_CPP_Rule_3.3.13.html
%%
%% Non-formal definition: "Do not invoke virtual methods of the
%% declared class in a constructor or destructor."
%%
%% Justification: Invoking virtual methods in a constructor always
%% invokes the method for the current class, or its base, even when
%% the constructor is invoked as part of the construction of a derived
%% class. This also applies to virtual methods called in a destructor.
%%
%% Comments: The non-formal definition does not say two important
%% things:
%% - We need to catch also virtual methods indirectly called throug a
%%   chain of non-virtual methods.
%% - Invocations on objects different from "this" should not be
%%   tracked.
%% Both points are taken into accout in the following formalization,
%% that relies on an auxiliary predicate "calls".
%%
%% Tentative formalization in CRISP:
%%
%%   pred  Caller calls Callee
%%   vars  Caller, Callee is FunctionDecl
%%   def   Caller has body Body
%%         Body has child+ CallExpr
%%         CallExpr is CallExpr
%%         CallExpr has directCallee Callee
%%
%%   rule  HICPP 3.3.13
%%   warn  "ctor/dtor %0 calls (maybe indirectly) virtual method %1"
%%   vars  Caller is CXXMethodDecl, note "caller %0 declared here"
%%         Callee is CXXMethodDecl, note "callee %0 declared here"
%%   def   Record is CXXRecordDecl
%%         Record has ctor or destructor Caller
%%         Record has method Callee
%%         Callee is virtual
%%         Caller calls+ Callee where
%%             ( CallExpr is CXXMemberCallExpr
%%               CallExpr has implicitObjectArgument MemberExpr
%%               MemberExpr is CXXThisExpr )
%%
%% Executable formalization in Prolog:

'child+'(Stmt, DirectChild) :-
        'Stmt::child'(Stmt, DirectChild).
'child+'(Stmt, IndirectChild) :-
        'Stmt::child'(Stmt, DirectChild),
        'child+'(DirectChild, IndirectChild).

calls_to_this(Caller, Callee) :-
        'Decl::body'(Caller, Body),
        'child+'(Body, CallExpr),
        'Stmt::stmtClassName'(CallExpr, 'CXXMemberCallExpr'),
        'CXXMemberCallExpr::implicitObjectArgument'(CallExpr, MemberExpr),
        'Stmt::stmtClassName'(MemberExpr, 'CXXThisExpr'),
        'CallExpr::directCallee'(CallExpr, Callee).

'calls_to_this+'(Caller, Callee) :-
        calls_to_this(Caller, Callee).
'calls_to_this+'(Caller, IndirectCallee) :-
        calls_to_this(Caller, DirectCallee),
        'calls_to_this+'(DirectCallee, IndirectCallee).

violation('HICPP 3.3.13',
          'ctor/dtor %0 calls (maybe indirectly) virtual method %1',
          [ 'NamedDecl'(Caller, 'caller %0 declared here')
          , 'NamedDecl'(Callee, 'callee %0 declared here')]) :-
        isA(Record, 'CXXRecordDecl'),
        ( 'CXXRecordDecl::ctor'(Record, Caller)
        ; 'CXXRecordDecl::destructor'(Record, Caller)
        ),
        'CXXRecordDecl::method'(Record, Callee),
        'CXXMethodDecl::is_virtual'(Callee), % implies Caller \= Callee
        'calls_to_this+'(Caller, Callee).


%% HICPP 3.4.2
%% ===========
%%
%% URL: http://www.codingstandard.com/HICPPCM/High_Integrity_CPP_Rule_3.4.2.html
%%
%% Non-formal definition: "Do not return non-const handles to class
%% data from const member functions."
%%
%% Justification: Non-const handles returned from const member
%% functions indirectly allow modification of class data. Const
%% functions returning pointers or references to member data should
%% return const pointers or references.
%%
%% Tentative formalization in CRISP:
%%
%%   rule  HICPP 3.4.2.
%%   warn  "const member function %0 returns a non-const handle to object data"
%%   vars  Method is CXXMethodDecl, note "const member function %0 declared here"
%%   def   Method has type Type
%%         Type has canonicalType CanonicalMethodType
%%         CanonicalMethodType is const
%%         CanonicalMethodType has resultType ResultType
%%         ResultType has canonicalType CanonicalResultType
%%         CanonicalResultType is PointerType
%%         CanonicalResultType has pointeeType PointeeType
%%         PointeeType has canonicalType CanonicalPointeeType
%%         CanonicalPointeeType is not const
%%         Method has arg This
%%         This has name "this"
%%         Method has instruction StoreThis, LoadThis, OffsetFromThis, Return
%%         StoreThis is StoreInst
%%         StoreThis has valueOperand This, pointerOperand ThisAddr
%%         LoadThis is LoadInst
%%         LoadThis has pointerOperand ThisAddr
%%         ( OffsetFromThis is GetElementPtrInst and has pointerOperand LoadThis
%%             or OffsetFromThis is BitCastInst and has op LoadThis )
%%         OffsetFromThis has location OffsetFromThisLoc
%%         Return is ReturnInst
%%         Return has op UsedByReturn
%%         UsedByReturn has location ReturnLoc
%%         OffsetFromThisLoc alias ReturnLoc /<= NoAlias
%%
%% Executable formalization in Prolog:

violation_candidate('HICPP 3.4.2', [MethodRepr]) :-
        isA(Method, 'CXXMethodDecl'),
        'ValueDecl::type'(Method, MethodType),
        'QualType::canonicalType'(MethodType, CanonicalMethodType),
        isConstFunctionProtoType(CanonicalMethodType),
        'FunctionType::resultType'(CanonicalMethodType, ResultType),
        'QualType::canonicalType'(ResultType, CanonicalResultType),
        isA(CanonicalResultType, 'PointerType'),
        'QualType::typePtr'(CanonicalResultType, ResultTypePtr),
        'PointerType::pointeeType'(ResultTypePtr, PointeeType),
        'QualType::canonicalType'(PointeeType, CanonicalPointeeType),
        \+ 'QualType::is_constQualified'(CanonicalPointeeType),

                                % This part should be generated by
                                % CRISP compiler
        mangleName(Method, MethodRepr).

violation_llvm('HICPP 3.4.2',
               'const member function %0 returns a non-const handle to object data',
               ['Function'(Func)]) :-
        violation_candidate_('HICPP 3.4.2', [FuncName]),
        % isA_(Module, 'Module'),
        getFunction(FuncName, Func),
        'Function::arg'(Func, This),
        'Value::name'(This, 'this'),
        'Function::instruction'(Func, StoreThis),
        isA_(StoreThis, 'StoreInst'),
        'StoreInst::valueOperand'(StoreThis, This),
        'StoreInst::pointerOperand'(StoreThis, ThisAddr),
        'Function::instruction'(Func, LoadThis),
        isA_(LoadThis, 'LoadInst'),
        'LoadInst::pointerOperand'(LoadThis, ThisAddr),
        'Function::instruction'(Func, OffsetFromThis),
        ( isA_(OffsetFromThis, 'GetElementPtrInst'),
          'GetElementPtrInst::pointerOperand'(OffsetFromThis, LoadThis)
        ; isA_(OffsetFromThis, 'BitCastInst'),
          'User::op'(OffsetFromThis, LoadThis)
        ),
        getLocation(OffsetFromThis, OffsetFromThisLoc),
        'Function::instruction'(Func, Return),
        isA_(Return, 'ReturnInst'),
        'User::op'(Return, UsedByReturn),
        getLocation(UsedByReturn, ReturnLoc),
        alias(OffsetFromThisLoc, ReturnLoc, AliasResult),
        AliasResult > 0.
