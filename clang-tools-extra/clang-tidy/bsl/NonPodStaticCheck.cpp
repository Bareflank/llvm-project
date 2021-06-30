//===--- NonPodStaticCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NonPodStaticCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void NonPodStaticCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    varDecl(
      hasStaticStorageDuration(),
      unless(
        anyOf(
          isConstexpr(),
          hasAttr(attr::ConstInit)
        )
      )
    ).bind("decl"),
    this
  );
}

void NonPodStaticCheck::check(const MatchFinder::MatchResult &Result) {
  auto const VD = Result.Nodes.getNodeAs<VarDecl>("decl");
  if (VD->isInvalidDecl())
    return;

  auto const Loc = VD->getLocation();
  if (Loc.isInvalid())
    return;

  if (!VD->hasInit())
    return;

  if (VD->getType().isCXX11PODType(*Result.Context))
    return;

  diag(Loc, "non-pod type with static storage duration");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
