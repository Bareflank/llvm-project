//===--- StmtSwitchDefaultBreakCheck.cpp - clang-tidy ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StmtSwitchDefaultBreakCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

static bool isSubStmtTerminal(const Stmt *SubStmt) {
  if (isa<BreakStmt>(SubStmt))
    return true;

  if (isa<CXXThrowExpr>(SubStmt))
    return true;

  if (!isa<CompoundStmt>(SubStmt))
    return false;

  const CompoundStmt *Body = dyn_cast<CompoundStmt>(SubStmt);
  const Stmt *Last = Body->body_back();

  if (!Last)
    return false;

  return isa<BreakStmt>(Last) || isa<CXXThrowExpr>(Last);
}

void StmtSwitchDefaultBreakCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(switchStmt().bind("switch"), this);
}

void StmtSwitchDefaultBreakCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Switch = Result.Nodes.getNodeAs<SwitchStmt>("switch");

  auto const Loc = Switch->getBeginLoc();
  if (Loc.isInvalid())
    return;

  const CompoundStmt *Body = dyn_cast<CompoundStmt>(Switch->getBody());
  if (!Body)
    return;

  CompoundStmt::const_body_iterator Child = Body->body_begin();
  while (Child != Body->body_end()) {
    const DefaultStmt *Def = dyn_cast<DefaultStmt>(*Child);
    if (!Def) {
      ++Child;
      continue;
    }

    if (isSubStmtTerminal(Def->getSubStmt()))
      return;

    bool TerminalSibling = false;

    // Now scan for a sibling break or throw. Note this assumes
    // that the check bsl-stmt-switch-default-last holds, so
    // we may assume that default is the last switch case.

    ++Child;
    while ((!TerminalSibling) && (Child != Body->body_end())) {
      TerminalSibling = isa<BreakStmt>(*Child) || isa<CXXThrowExpr>(*Child);
      ++Child;
    }

    if (TerminalSibling)
      return;

    diag(Def->getBeginLoc(), "default case must end with break or throw");
    break;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
