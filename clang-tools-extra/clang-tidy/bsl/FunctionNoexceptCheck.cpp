//===--- FunctionNoexceptCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "FunctionNoexceptCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void FunctionNoexceptCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    functionDecl(
      unless(
        isImplicit()
      )
    ).bind("decl"),
    this
  );
}

void FunctionNoexceptCheck::check(const MatchFinder::MatchResult &Result) {
  if (!Result.Context->getLangOpts().CPlusPlus)
    return;

  auto const *FD = Result.Nodes.getNodeAs<FunctionDecl>("decl");
  if (nullptr == FD)
    return;

  auto const Loc = FD->getLocation();
  if (Loc.isInvalid())
    return;

  if (FD->getExceptionSpecType() != EST_None)
    return;

  if (isa<CXXDeductionGuideDecl>(FD))
    return;

  diag(Loc, "every function should be marked as noexcept or noexcept(false)");
}

// clang::ExceptionSpecificationType::
// EST_None


} // namespace bsl
} // namespace tidy
} // namespace clang
