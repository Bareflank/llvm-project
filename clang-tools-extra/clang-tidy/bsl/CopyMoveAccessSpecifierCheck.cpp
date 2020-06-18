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
  // isDeleted, isProtected
  //  isCopyConstructor,  isMoveConstructor
  // isCopyAssignmentOperator or isMoveAssignmentOperator
  Finder->addMatcher(cxxMethodDecl(anyOf(isCopyConstructor(), isMoveConstructor(), isCopyAssignmentOperator(), isMoveAssignmentOperator()),
                                  unless(anyOf(isDeleted(), isProtected()))).bind("x"), this);
}

void CopyMoveAccessSpecifierCheck::copyMoveProtectedOrDelete() {

}


void CopyMoveAccessSpecifierCheck::check(const MatchFinder::MatchResult &Result) {
  // FIXME: Add callback implementation.
  const auto *MatchedDecl = Result.Nodes.getNodeAs<FunctionDecl>("x");
  forallBases(
      [BaseClass](const CXXRecordDecl *Cur) { return Cur != BaseClass; });


  diag(MatchedDecl->getLocation(), "function %0 is insufficiently awesome")
      << MatchedDecl;
}

} // namespace bsl
} // namespace tidy
} // namespace clang
