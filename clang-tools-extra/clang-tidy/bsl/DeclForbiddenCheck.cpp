//===--- DeclForbiddenCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IsDefinedInATestFile.h"

#include "DeclForbiddenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void DeclForbiddenCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(decl(anyOf(tagDecl(isUnion()),
                                fieldDecl(isBitField()),
                                friendDecl())).bind("decl"),
                     this);
}

void DeclForbiddenCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *D = Result.Nodes.getNodeAs<Decl>("decl");

  auto const Loc = D->getBeginLoc();
  if (Loc.isInvalid())
    return;

  if (isDefinedInATestFile(Result.Context, Loc))
    return;

  auto const Tag = dyn_cast<TagDecl>(D);
  if (Tag && Tag->isUnion()) {
    diag(Loc, "unions are forbidden");
    return;
  }

  auto const Friend = dyn_cast<FriendDecl>(D);
  if (Friend) {
    auto const FriendLoc = Friend->getFriendLoc();
    diag(FriendLoc, "friends are forbidden");
    return;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
