//===--- ClassVirtualBaseCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClassVirtualBaseCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXRecordDecl, hasVirtualBases) {
  CXXRecordDecl *Def = Node.getDefinition();
  return Def->getNumVBases() > 0;
}

void ClassVirtualBaseCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    cxxRecordDecl(hasDefinition(), hasVirtualBases()).bind("class"),
    this);
}

void ClassVirtualBaseCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *D = Result.Nodes.getNodeAs<CXXRecordDecl>("class");

  auto const Loc = D->getBeginLoc();
  if (Loc.isInvalid())
    return;

  diag(Loc, "class inherits virtual base class");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
