//===--- EmptyIfElseCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EmptyIfElseCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "BslCheckUtils.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void EmptyIfElseCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    compoundStmt(
      forEach(
        ifStmt(
          stmt(),
          unless(isConstexpr())
        ).bind("if")
      )
    ),
    this);
}

void EmptyIfElseCheck::check(const MatchFinder::MatchResult &Result) {
  auto const IS = Result.Nodes.getNodeAs<IfStmt>("if");
  if (stmtContainsErrors(IS, Result))
    return;

  if (auto const IfThen = IS->getThen()) {
    auto const Loc = IS->getIfLoc();
    if (Loc.isInvalid())
      return;

    if (stmtContainsErrors(IfThen, Result))
      return;

    if (IfThen->children().empty()) {
      diag(Loc, "Empty 'if' statements are forbidden");
    }
  }

  if (auto const IfElse = IS->getElse()) {
    auto const Loc = IS->getElseLoc();
    if (Loc.isInvalid())
      return;

    if (stmtContainsErrors(IfElse, Result))
      return;

    if (IfElse->children().empty()) {
      diag(Loc, "Empty 'else' statements are forbidden");
    }
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
