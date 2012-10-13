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
%%
%% - We need to catch also virtual methods indirectly called throug a
%%   chain of non-virtual methods.
%%
%% - Invocations on objects different from "this" should not be
%%   tracked.
%%
%% Both points are taken into accout in the following formalization,
%% that relies on auxiliary predicates "calls" and "has
%% functionCall". Also, the intent is that repeated violations
%% involving the same "(Caller, Callee)" pair are ignored, as the call
%% point ("CallExpr") is not part of the signature ("vars" directive)
%% of the rule predicate. Current Prolog implementation does not
%% fulfil this.
%%
%% Tentative formalization in CRISP:
%%
%%   pred  Func has functionCall Call
%%   vars  Func is FunctionDecl
%%         Call is CallExpr
%%   meet  Func has body Body       -- Optional: Body is Stmt
%%         Body has child+ Call
%%
%%   pred  Caller calls Callee [at CallPoint]
%%   vars  Caller, Callee is FunctionDecl
%%         CallPoint is CallExpr
%%   meet  Caller has functionCall CallPoint
%%         CallPoint has directCallee Callee
%%
%%   rule  HICPP 3.3.13
%%   warn  "ctor/dtor %Caller calls (maybe indirectly) virtual method %Callee",
%%         show Caller location
%%   note  "called virtual method %Callee", show %Callee location
%%   vars  Caller is CXXMethodDecl
%%         Callee is CXXMethodDecl
%%   meet  Record is CXXRecordDecl
%%         Record has ctor or destructor Caller
%%         Record has method Callee
%%         Callee is virtual
%%         Caller calls'+ Callee where   -- inline preds can refer to outer vars
%%               pred  Caller' calls' Callee' at CallPoint
%%               meet  Caller' calls Callee' at CallPoint
%%                     Caller', Callee' is CXXMethodDecl
%%                     CallPoint is CallExpr  -- redundant due to def of 'calls'
%%                     Caller' is not virtual -- implies Caller' \= Callee'
%%                     CallPoint has implicitObjectArgument MemberExpr
%%                     MemberExpr is CXXThisExpr
%%               note  "method %Caller' calls method %Callee'",
%%                     show CallPoint range

%% Executable formalization in Prolog:

'child+'(Stmt, DirectChild) :-
        'Stmt::child'(Stmt, DirectChild).
'child+'(Stmt, IndirectChild) :-
        'Stmt::child'(Stmt, DirectChild),
        'child+'(DirectChild, IndirectChild).

has_functionCall(Function, Call) :-
        (  isA(Function, 'FunctionDecl') % TODO: del other cases when
                                         % concept subsumption is
                                         % implemented.
        ;  isA(Function, 'CXXMethodDecl')
        ;  isA(Function, 'CXXConstructorDecl')
        ;  isA(Function, 'CXXDestructorDecl')
        ;  isA(Function, 'CXXConversionDecl')
        ),
        'Decl::body'(Function, Body),
        'child+'(Body, Call),
        'Stmt::stmtClassName'(Call, 'CXXMemberCallExpr'). % TODO:
                                                          % implement
                                                          % isA/2 for
                                                          % statements

calls(Caller, Callee, CallPoint) :-
        % isA(Caller, 'FunctionDecl'),   % implied by has_functionCall/2
        % isA(Callee, 'FunctionDecl'),   % implied by directCallee/2
        % isA(CallPoint, 'CallExpr'),    % implied by has_functionCall/2
        has_functionCall(Caller, CallPoint),
        'CallExpr::directCallee'(CallPoint, Callee).

'calls+'(Caller, Callee, CallPoint, Meet, [Diagnostic]) :-
        calls(Caller, Callee, CallPoint),
        call(Meet, Caller, Callee, CallPoint, Diagnostic).
'calls+'( Caller, IndirectCallee, IndirectCallPoint, Meet
        , [DirectDiagnostic| IndirectDiagnostic]) :-
        calls(Caller, DirectCallee, DirectCallPoint),
        call(Meet, Caller, DirectCallee, DirectCallPoint, DirectDiagnostic),
        'calls+'( DirectCallee, IndirectCallee, IndirectCallPoint, Meet
                , IndirectDiagnostic).

meet(Caller, Callee, CallPoint, Diagnostic) :-
        Diagnostic =
        note( 'method %0 calls method %1 here'
            , 'Stmt'(CallPoint)
            , [ 'NamedDecl'(Caller), 'NamedDecl'(Callee) ]
            , 'Stmt'(CallPoint)
            ),
        % isA(Caller, 'CXXMethodDecl'),  % Implied by the call ctxt
        % isA(Callee, 'CXXMethodDecl'),  % Idem
        % isA(CallPoint, 'CallExpr'),    % Idem
        \+ 'CXXMethodDecl::is_virtual'(Caller),
        'CXXMemberCallExpr::implicitObjectArgument'(CallPoint, MemberExpr),
        'Stmt::stmtClassName'(MemberExpr, 'CXXThisExpr'). % replaces is/2 call

violation('HICPP 3.3.13', Diagnostic) :-
        GlobalDiagnostic =
        [ warn( 'ctor/dtor %0 calls (maybe indirectly) virtual method %1'
              , 'Decl'(Caller)
              , [ 'NamedDecl'(Caller), 'NamedDecl'(Callee) ]
              , 'Null'
              ),
          note( 'called virtual method %0 declared here'
              , 'Decl'(Callee)
              , [ 'NamedDecl'(Callee) ]
              , 'Null'
              )
        ],
        % isA(Caller, 'CXXMethodDecl'),  % implied by ctor/2 or destructor/2
        % isA(Callee, 'CXXMethodDecl'),  % implied by method/2
        isA(Record, 'CXXRecordDecl'),
        ( 'CXXRecordDecl::ctor'(Record, Caller)
        ; 'CXXRecordDecl::destructor'(Record, Caller)
        ),
        'CXXRecordDecl::method'(Record, Callee),
        'CXXMethodDecl::is_virtual'(Callee), % implies Caller \= Callee
        'calls+'(Caller, Callee, _, meet, CallsDiagnostic),
        append([GlobalDiagnostic, CallsDiagnostic], Diagnostic).


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
%%   warn  "return statement in const member function %Method returns a
%%         non-const handle to class data", show Return range
%%   note  "const member function %Method declared here", show Method location
%%   vars  Method is CXXMethodDecl
%%         Return is ReturnStmt                 -- has pointer type
%%   meet  Method has canonical type MethodType -- 'canonical':opt if a T is ret
%%         MethodType is const
%%         MethodType has canonical resultType ResultType
%%         ResultType is PointerType
%%         ResultType has canonical pointeeType PointeeType
%%         PointeeType is not const
%%         Method has body Body
%%         Body has child+ Return
%%         Return has retValueMemLoc ReturnLoc      -- binds to LLVM location
%%         Method has thisMemLoc ThisLoc            -- binds to LLVM location
%%         cannotDiscardAliasBetween ReturnLoc ThisLoc

%% Low-level CRISP:

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

%% FIXME: The following rule implementation has become obsolete, it
%% has to be adapted to new diagnostic facilities and CRISP syntax.

%% Executable formalization in Prolog:

violation('HICPP 3.4.2', Diagnostic) :-
        GlobalDiagnostic =
        [ warn( 'return statement in const member function %0 returns a non-const handle to class data'
              , 'Stmt'(Return)
              , [ 'NamedDecl'(Method) ]
              , 'Stmt'(Return)
              ),
          note( 'const member function %0 declared here'
              , 'Decl'(Method)
              , [ 'NamedDecl'(Method) ]
              , 'Null'
              )
        ],
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
        'Decl::body'(Method, Body),
        'child+'(Body, Return),
        'Stmt::stmtClassName'(Return, 'ReturnStmt'),
        append([GlobalDiagnostic], Diagnostic).

% violation_candidate('HICPP 3.4.2', [MethodRepr]) :-
%         isA(Method, 'CXXMethodDecl'),
%         'ValueDecl::type'(Method, MethodType),
%         'QualType::canonicalType'(MethodType, CanonicalMethodType),
%         isConstFunctionProtoType(CanonicalMethodType),
%         'FunctionType::resultType'(CanonicalMethodType, ResultType),
%         'QualType::canonicalType'(ResultType, CanonicalResultType),
%         isA(CanonicalResultType, 'PointerType'),
%         'QualType::typePtr'(CanonicalResultType, ResultTypePtr),
%         'PointerType::pointeeType'(ResultTypePtr, PointeeType),
%         'QualType::canonicalType'(PointeeType, CanonicalPointeeType),
%         \+ 'QualType::is_constQualified'(CanonicalPointeeType),

%                                 % This part should be generated by
%                                 % CRISP compiler
%         mangleName(Method, MethodRepr).

% violation_llvm('HICPP 3.4.2',
%                'const member function %0 returns a non-const handle to object data',
%                ['Function'(Func)]) :-
%         violation_candidate_('HICPP 3.4.2', [FuncName]),
%         % isA_(Module, 'Module'),
%         getFunction(FuncName, Func),
%         'Function::arg'(Func, This),
%         'Value::name'(This, 'this'),
%         'Function::instruction'(Func, StoreThis),
%         isA_(StoreThis, 'StoreInst'),
%         'StoreInst::valueOperand'(StoreThis, This),
%         'StoreInst::pointerOperand'(StoreThis, ThisAddr),
%         'Function::instruction'(Func, LoadThis),
%         isA_(LoadThis, 'LoadInst'),
%         'LoadInst::pointerOperand'(LoadThis, ThisAddr),
%         'Function::instruction'(Func, OffsetFromThis),
%         ( isA_(OffsetFromThis, 'GetElementPtrInst'),
%           'GetElementPtrInst::pointerOperand'(OffsetFromThis, LoadThis)
%         ; isA_(OffsetFromThis, 'BitCastInst'),
%           'User::op'(OffsetFromThis, LoadThis)
%         ),
%         getLocation(OffsetFromThis, OffsetFromThisLoc),
%         'Function::instruction'(Func, Return),
%         isA_(Return, 'ReturnInst'),
%         'User::op'(Return, UsedByReturn),
%         getLocation(UsedByReturn, ReturnLoc),
%         alias(OffsetFromThisLoc, ReturnLoc, AliasResult),
%         AliasResult > 0.

% violation_candidate('AliasTest 1', [Var1Llvm, Var2Llvm]) :-
%         isA(Var1, 'VarDecl'),
%         isA(Var2, 'VarDecl'),
%         Var1 \= Var2,
%                                 % This part should be generated by
%                                 % CRISP compiler
%         llvm_name(Var1, Var1Llvm),
%         llvm_name(Var2, Var2Llvm).

% violation_candidate('TestVariableAndFunctionNameTranslation', [VarOrFuncLlvm]) :-
%         ( isA(VarOrFunc, 'VarDecl')
%         ; isA(VarOrFunc, 'ParmVarDecl')
%         ; isA(VarOrFunc, 'FunctionDecl')
%         ; isA(VarOrFunc, 'CXXMethodDecl')
%         ; isA(VarOrFunc, 'CXXConstructorDecl')
%         ; isA(VarOrFunc, 'CXXDestructorDecl')
%         ; isA(VarOrFunc, 'CXXConversionDecl')
%         ),
%         'Decl::is_canonicalDecl'(VarOrFunc),
%         llvm_name(VarOrFunc, VarOrFuncLlvm).
