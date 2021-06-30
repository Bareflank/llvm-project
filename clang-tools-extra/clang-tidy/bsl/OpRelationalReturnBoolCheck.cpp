//===--- OpRelationalReturnBoolCheck.cpp - clang-tidy ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "OpRelationalReturnBoolCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void OpRelationalReturnBoolCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    functionDecl(
      hasAnyOverloadedOperatorName("==", "!=", "<", ">", "<=", ">="),
      unless(returns(booleanType()))
    ).bind("comp"),
    this);
}

void OpRelationalReturnBoolCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Func = Result.Nodes.getNodeAs<FunctionDecl>("comp");

  auto const Loc = Func->getLocation();
  if (Loc.isInvalid())
    return;

  diag(Loc, "comparison operator does not return bool");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
