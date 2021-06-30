//===--- OpEqualityVirtMemfnNullptrCheck.cpp - clang-tidy -----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "OpEqualityVirtMemfnNullptrCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void OpEqualityVirtMemfnNullptrCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(expr(anyOf(
    binaryOperator(
      hasAnyOperatorName("==", "!="),
      hasLHS(
        unaryOperator(
          hasOperatorName("&"),
          hasUnaryOperand(
            declRefExpr(hasDeclaration(cxxMethodDecl(isVirtual())))
          )
        )
      ),
      hasRHS(expr(unless(ignoringParenImpCasts(cxxNullPtrLiteralExpr()))))
    ),
    binaryOperator(
      hasAnyOperatorName("==", "!="),
      hasLHS(expr(unless(ignoringParenImpCasts(cxxNullPtrLiteralExpr())))),
      hasRHS(
        unaryOperator(
          hasOperatorName("&"),
          hasUnaryOperand(
            declRefExpr(hasDeclaration(cxxMethodDecl(isVirtual())))
          )
        )
      )
    )
  )).bind("op"),
  this);
}

void OpEqualityVirtMemfnNullptrCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Op = Result.Nodes.getNodeAs<BinaryOperator>("op");
  auto const Loc = Op->getOperatorLoc();

  if (Loc.isInvalid())
    return;

  diag(Loc, "comparing virtual member function with expression that is not 'nullptr'");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
