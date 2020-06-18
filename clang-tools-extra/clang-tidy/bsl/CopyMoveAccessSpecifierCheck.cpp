//===--- CopyMoveAccessSpecifierCheck.cpp - clang-tidy --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "CopyMoveAccessSpecifierCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void CopyMoveAccessSpecifierCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxMethodDecl(anyOf(isCopyAssignmentOperator(), isMoveAssignmentOperator()),
                                  unless(anyOf(isDeleted(), isProtected()))).bind("op"), this);
  Finder->addMatcher(cxxConstructorDecl(anyOf(isCopyConstructor(), isMoveConstructor()),
                                  unless(anyOf(isDeleted(), isProtected()))).bind("ctor"), this);
}

void CopyMoveAccessSpecifierCheck::check(const MatchFinder::MatchResult &Result) {
  auto Mgr = Result.SourceManager;

  const auto *VDecl = Result.Nodes.getNodeAs<CXXConstructorDecl>("ctor");
  if (VDecl) {
    auto LocV = VDecl->getLocation();
    if (Mgr->getFileID(LocV) != Mgr->getMainFileID())
      return;
    if (!VDecl->getParent()->isEffectivelyFinal())
      diag(LocV, "Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final");
  }

  const auto *MDecl = Result.Nodes.getNodeAs<CXXMethodDecl>("op");
  if (MDecl) {
    auto LocM = MDecl->getLocation();
    if (Mgr->getFileID(LocM) != Mgr->getMainFileID())
      return;
    if (!MDecl->getParent()->isEffectivelyFinal())
      diag(LocM, "Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final");
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
