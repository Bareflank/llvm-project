//===--- StmtForbiddenCheck.cpp - clang-tidy -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StmtForbiddenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void StmtForbiddenCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(stmt(anyOf(gotoStmt(), doStmt())).bind("stmt"), this);
}

void StmtForbiddenCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *S = Result.Nodes.getNodeAs<Stmt>("stmt");

  auto const Loc = S->getBeginLoc();
  if (Loc.isInvalid())
    return;

  if (isa<GotoStmt>(S)) {
    diag(Loc, "goto statement is forbidden");
    return;
  }

  if (isa<DoStmt>(S)) {
    diag(Loc, "do statement is forbidden");
    return;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
