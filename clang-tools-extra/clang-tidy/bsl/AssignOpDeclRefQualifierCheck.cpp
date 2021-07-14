//===--- AssignOpDeclRefQualifierCheck.cpp - clang-tidy -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AssignOpDeclRefQualifierCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void AssignOpDeclRefQualifierCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    cxxMethodDecl(
      hasAnyOverloadedOperatorName("=", "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", ">>=", "<<="),
      unless(
        isImplicit()
      )
    ).bind("decl"),
    this
  );
}

void AssignOpDeclRefQualifierCheck::check(const MatchFinder::MatchResult &Result) {
  auto const D = Result.Nodes.getNodeAs<CXXMethodDecl>("decl");
  if (D->isInvalidDecl())
    return;

  auto const Loc = D->getLocation();
  if (Loc.isInvalid())
    return;

  if (D->getRefQualifier() != RQ_None)
    return;

  diag(Loc, "assignment operators should be declared with the ref-qualifier &");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
