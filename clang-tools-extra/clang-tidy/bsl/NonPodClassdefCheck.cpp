//===--- NonPodClassdefCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NonPodClassdefCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXRecordDecl, isPOD) { return Node.isPOD(); }

void NonPodClassdefCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      declaratorDecl(anyOf(fieldDecl(), varDecl()), unless(isPrivate()),
                     hasParent(cxxRecordDecl(isClass(), unless(isPOD()))))
          .bind("private"),
      this);
}

void NonPodClassdefCheck::check(const MatchFinder::MatchResult &Result) {
  auto DD = Result.Nodes.getNodeAs<DeclaratorDecl>("private");
  if (nullptr == DD)
    return;

  auto Loc = DD->getLocation();
  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  diag(Loc, "non-POD class types should have private member data");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
