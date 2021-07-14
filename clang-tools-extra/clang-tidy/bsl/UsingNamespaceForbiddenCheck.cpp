//===--- UsingNamespaceForbiddenCheck.cpp - clang-tidy --------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UsingNamespaceForbiddenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void UsingNamespaceForbiddenCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    usingDirectiveDecl(
      unless(
        isImplicit()
      )
    ).bind("decl"),
    this
  );
}

void UsingNamespaceForbiddenCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *D = Result.Nodes.getNodeAs<UsingDirectiveDecl>("decl");
  auto const Loc = D->getUsingLoc();

  if (Loc.isInvalid())
    return;

  diag(Loc, "using directives are forbidden");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
