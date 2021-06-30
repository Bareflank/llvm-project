//===--- TemplateGenericParamCheck.cpp - clang-tidy -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TemplateGenericParamCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/TemplateName.h"
#include <iostream>

using namespace clang::ast_matchers;
using namespace std;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXMethodDecl, isConstructor) {
  return isa<CXXConstructorDecl>(Node);
}

void TemplateGenericParamCheck::registerMatchers(MatchFinder *Finder) {
  auto ForwardingRefParm =
      anyOf(parmVarDecl(
          hasType(templateTypeParmType(hasDeclaration(
                               templateTypeParmDecl().bind("type-parm-decl")))))
          .bind("parm-var"),
          parmVarDecl(
          hasType(qualType(references(templateTypeParmType(hasDeclaration(
                               templateTypeParmDecl().bind("type-parm-decl"))))
                                )))
          .bind("parm-var"));

  DeclarationMatcher findOverload =
      cxxConstructorDecl(
          hasParameter(0, ForwardingRefParm))
          .bind("ctor");
  Finder->addMatcher(findOverload, this);

  DeclarationMatcher findOpOverload =
      cxxMethodDecl(
          hasParameter(0, ForwardingRefParm), unless(isConstructor()))
          .bind("copy-assign");
  Finder->addMatcher(findOpOverload, this);
}

void TemplateGenericParamCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *ParmVar = Result.Nodes.getNodeAs<ParmVarDecl>("parm-var");
  auto const *TypeParmDecl =
      Result.Nodes.getNodeAs<TemplateTypeParmDecl>("type-parm-decl");

  // Get the FunctionDecl and FunctionTemplateDecl containing the function
  // parameter.
  auto const *FuncForParam = dyn_cast<FunctionDecl>(ParmVar->getDeclContext());
  if (!FuncForParam)
    return;
  const FunctionTemplateDecl *FuncTemplate =
      FuncForParam->getDescribedFunctionTemplate();
  if (!FuncTemplate)
    return;

  // Check that the template type parameter belongs to the same function
  // template as the function parameter of that type. (This implies that type
  // deduction will happen on the type.)
  const TemplateParameterList *Params = FuncTemplate->getTemplateParameters();
  if (!llvm::is_contained(*Params, TypeParmDecl))
    return;

  auto const *Ctor = Result.Nodes.getNodeAs<CXXConstructorDecl>("ctor");
  if (Ctor) {
    // Every parameter after the first must have a default value.
    for (auto Iter = Ctor->param_begin() + 1; Iter != Ctor->param_end(); ++Iter) {
      if (!(*Iter)->hasDefaultArg())
        return;
    }

    if (!Ctor->getParent()->hasUserDeclaredCopyConstructor()) {
      diag(Ctor->getLocation(), "a copy constructor should be declared when there is a template constructor with a single parameter that is a generic parameter.");
    }
  }

  auto const *CopyAssignOp = Result.Nodes.getNodeAs<CXXMethodDecl>("copy-assign");
  if (CopyAssignOp) {
    for (auto Iter = CopyAssignOp->param_begin() + 1; Iter != CopyAssignOp->param_end(); ++Iter) {
      if (!(*Iter)->hasDefaultArg())
        return;
    }
    if (!CopyAssignOp->getParent()->hasUserDeclaredCopyAssignment()) {
      diag(CopyAssignOp->getLocation(), "a copy assignment operator should be declared when there is a template assignment operator with a parameter that is a generic parameter.");
    }
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
