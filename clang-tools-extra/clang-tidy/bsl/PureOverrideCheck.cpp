//===--- PureOverrideCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "PureOverrideCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXMethodDecl, isUserProvided) {
  return Node.isUserProvided();
}

void PureOverrideCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    cxxMethodDecl(isUserProvided(), isOverride(), isPure()).bind("pure"), this);
}

void PureOverrideCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Pure = Result.Nodes.getNodeAs<CXXMethodDecl>("pure");

  auto const Loc = Pure->getLocation();
  if (Loc.isInvalid())
    return;

  for (auto const OM : Pure->overridden_methods()) {
    if (OM->isPure())
      continue;

    diag(Loc, "pure function %0 overrides non-pure function") << Pure;

    diag(OM->getLocation(), "found non-pure overridden function here",
         DiagnosticIDs::Note);

    return;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
