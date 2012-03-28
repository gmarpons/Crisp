// DeclExtractorPrologPredicates.cpp ---------------------------------*- C++ -*-

// Copyright (C) 2011, 2012 Guillem Marpons <gmarpons@babel.ls.fi.upm.es>
//
// This file is part of Crisp.
//
// Crisp is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// Crisp is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with Crisp.  If not, see <http://www.gnu.org/licenses/>.

#include "clang/AST/Decl.h"
#include "clang/AST/DeclCXX.h"

#include "crisp/PrologPredTemplatesForClangTypes.h"
#include "DeclExtractorPrologPredicates.h"

using namespace clang;

namespace crisp {

  namespace prolog {

    // Automatically generated function definitions.

#include "crisp/PrologPredDefinitionMacros.h"
#include "ClangFunctionsForBootstrapping.inc"

    // Manual function definitions.

    foreign_t
    pl_CXXBaseSpecifier_getBaseDecl(term_t CXXBaseSpecifierT,
                                    term_t CXXRecordDeclT) {
      // const CXXBaseSpecifier *S;
      // if ( !PL_get_pointer(CXXBaseSpecifierT, (void **) &S))
      //   return PL_warning("CXXBaseSpecifier::baseDecl/2: "
      //                     "instantiation fault on first arg");
      Retrieve<CXXBaseSpecifier>::argument_type Argument;
      if ( !Retrieve<CXXBaseSpecifier>::
           _(CXXBaseSpecifierT, &Argument, "CXXBaseSpecifier::baseDecl/2"))
        return FALSE;
      const RecordType *T = Argument->getType()->getAs<RecordType>();
      if ( !T) return FALSE;
        // return PL_warning("baseDecl/2: "
        //                   "CXXBaseSpecifier does not contain record type");
      CXXRecordDecl *D =
        cast_or_null<CXXRecordDecl>(T->getDecl()->getDefinition());
      if ( !D) return FALSE;
        // return PL_warning("baseDecl/2: "
        //                   "CXXBaseSpecifier does not contain record decl");
      return PL_unify_pointer(CXXRecordDeclT, D);
    }

    foreign_t pl_CXXMethodDecl_isConstQualified(term_t ArgumentT) {
      Retrieve<CXXMethodDecl>::argument_type Argument;
      if ( !Retrieve<CXXMethodDecl>::
           _(ArgumentT, &Argument, "CXXMethodDecl::constQualified/1"))
        return FALSE;
      Qualifiers Q = Qualifiers::fromCVRMask(Argument->getTypeQualifiers());
      return Check<Qualifiers, &Qualifiers::hasConst>::_(Q);
    }

  } // End namespace crisp::prolog

} // End namespace crisp
