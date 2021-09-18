//===--- LambdaImplicitCaptureCheck.cpp - clang-tidy ----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IsDefinedInATestFile.h"

#include "LambdaImplicitCaptureCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void LambdaImplicitCaptureCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(lambdaExpr().bind("lambda"), this);
}

void LambdaImplicitCaptureCheck::check(const MatchFinder::MatchResult &Result) {
  auto Lambda = Result.Nodes.getNodeAs<LambdaExpr>("lambda");

  if (isDefinedInATestFile(Result.Context, Lambda->getBeginLoc()))
    return;

  auto CaptureLoc = Lambda->getIntroducerRange();
  if (CaptureLoc.isInvalid())
    return;

  auto Begin = CaptureLoc.getBegin();
  if (Begin.isInvalid() || Begin.isMacroID())
    return;

  auto CapList = Lambda->implicit_captures();

  for (auto const &Cap : CapList) {
    auto Var = Cap.getCapturedVar();
    if (!Var)
      continue;

    if (Var->getStorageDuration() != SD_Automatic)
      continue;

    auto Warn = "implicit capture of variable with automatic storage duration";
    diag(Cap.getLocation(), Warn);
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
