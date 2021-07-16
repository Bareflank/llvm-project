//===--- LeftSideConstantsCheck.cpp - clang-tidy --------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LeftSideConstantsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "BslCheckUtils.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void LeftSideConstantsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    binaryOperator(
      hasAnyOperatorName("==", "!=")
    ).bind("op"),
    this
  );
}

void LeftSideConstantsCheck::check(const MatchFinder::MatchResult &Result) {
  auto const BO = Result.Nodes.getNodeAs<BinaryOperator>("op");
  if (stmtContainsErrors(BO, Result))
    return;

  if (!BO->isEqualityOp())
    return;

  auto const LHS = BO->getLHS();
  if (LHS->isValueDependent())
    return;

  auto const RHS = BO->getRHS();
  if (RHS->isValueDependent())
    return;

  if (!LHS->isIntegerConstantExpr(*Result.Context)) {
    if (RHS->isIntegerConstantExpr(*Result.Context)) {
      auto RHSLoc = RHS->getBeginLoc();
      if (RHSLoc.isInvalid())
        return;

      diag(RHSLoc, "Move the right hand side of the comparison to the left hand "
                   "side to prevent accidental assignments (mutable expressions "
                   "should be on the right hand side)");
    }
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
