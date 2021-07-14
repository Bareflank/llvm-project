//===--- DestructorAccessSpecifierCheck.cpp - clang-tidy ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "DestructorAccessSpecifierCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXDestructorDecl, isUnion) {
  return Node.getParent()->isUnion();
}

AST_MATCHER(CXXDestructorDecl, isPublicVirtual) {
  return Node.getAccess() == AS_public && Node.isVirtual();
}

AST_MATCHER(CXXDestructorDecl, isProtectedNonVirtual) {
  return Node.getAccess() == AS_protected && !Node.isVirtual();
}

void DestructorAccessSpecifierCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    cxxDestructorDecl(
      unless(
        anyOf(
          isUnion(),
          isPublicVirtual(),
          isProtectedNonVirtual()
        )
      )
    ).bind("destructor"),
    this);
}

void DestructorAccessSpecifierCheck::check(
    const MatchFinder::MatchResult &Result) {
  auto const *MatchedDecl =
      Result.Nodes.getNodeAs<CXXDestructorDecl>("destructor");
  if (MatchedDecl) {
    auto Loc = MatchedDecl->getLocation();

    auto const Parent = MatchedDecl->getParent();
    if (Parent->isLambda() || (MatchedDecl->getAccess() == AS_public &&
                               Parent->isEffectivelyFinal())) {
      return;
    }

    diag(Loc, "base class destructor must be public virtual, public override, "
              "or protected non-virtual. If public destructor is nonvirtual, "
              "then class must be declared final.");
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
