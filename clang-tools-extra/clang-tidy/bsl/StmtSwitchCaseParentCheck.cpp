//===--- StmtSwitchCaseParentCheck.cpp - clang-tidy ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StmtSwitchCaseParentCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void StmtSwitchCaseParentCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(switchCase(unless(anyOf(
      hasParent(caseStmt()),
      hasParent(compoundStmt(hasParent(switchStmt())))
  ))).bind("switch-case"),
  this);
}

void StmtSwitchCaseParentCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Case = Result.Nodes.getNodeAs<SwitchCase>("switch-case");
  auto const Loc = Case->getBeginLoc();
  if (Loc.isInvalid())
    return;

  diag(Loc, "switch case's parent is neither case label nor switch body");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
