//===--- TernaryOperatorForbiddenCheck.cpp - clang-tidy -------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TernaryOperatorForbiddenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void TernaryOperatorForbiddenCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(conditionalOperator(stmt()).bind("op"), this);
}

void TernaryOperatorForbiddenCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Op = Result.Nodes.getNodeAs<ConditionalOperator>("op");
  auto const Loc = Op->getBeginLoc();

  if (Loc.isInvalid())
    return;

  diag(Loc, "the ternary operator is forbidden");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
