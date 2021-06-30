//===--- StmtSwitchDefaultLastCheck.cpp - clang-tidy ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StmtSwitchDefaultLastCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <map>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void StmtSwitchDefaultLastCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(switchStmt().bind("switch"), this);
}

void StmtSwitchDefaultLastCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Switch = Result.Nodes.getNodeAs<SwitchStmt>("switch");

  auto const Loc = Switch->getBeginLoc();
  if (Loc.isInvalid())
    return;

  std::map<SourceLocation, bool> CaseMap;

  const SwitchCase *C = Switch->getSwitchCaseList();
  bool DefaultFound = false;
  SourceLocation DefaultLoc;

  while (C) {
    SourceLocation Loc = C->getBeginLoc();

    if (isa<DefaultStmt>(C)) {
      CaseMap[Loc] = true;
      DefaultFound = true;
      DefaultLoc = Loc;
    } else {
      CaseMap[Loc] = false;
    }

    C = C->getNextSwitchCase();
  }

  bool DefaultLast = CaseMap.rbegin()->second;
  if (DefaultFound && !DefaultLast) {
    diag(DefaultLoc, "default is not last case in switch");
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
