//===--- OpMixedIncrementDecrementCheck.cpp - clang-tidy ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "OpMixedIncrementDecrementCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void OpMixedIncrementDecrementCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(unaryOperator(hasAnyOperatorName("++", "--"),
                                   unless(anyOf(hasParent(varDecl()),
                                                hasParent(forStmt()),
                                                hasParent(compoundStmt()),
                                                hasParent(cxxForRangeStmt())))
                                  ).bind("op"),
                     this);
}

void OpMixedIncrementDecrementCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Op = Result.Nodes.getNodeAs<UnaryOperator>("op");
  auto const Loc = Op->getOperatorLoc();

  if (Loc.isInvalid())
    return;

  if (Op->isIncrementOp())
    diag(Loc, "use of '++' is mixed with other operations");
  else
    diag(Loc, "use of '--' is mixed with other operations");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
