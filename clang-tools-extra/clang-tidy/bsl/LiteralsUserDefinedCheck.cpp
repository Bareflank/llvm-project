//===--- LiteralsUserDefinedCheck.cpp - clang-tidy ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LiteralsUserDefinedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void LiteralsUserDefinedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(userDefinedLiteral().bind("literal"), this);
}

void LiteralsUserDefinedCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *UDL = Result.Nodes.getNodeAs<UserDefinedLiteral>("literal");

  if (nullptr == UDL) {
    return;
  }

  auto const suffix = UDL->getUDSuffix();
  if (nullptr == suffix) {
    return;
  }

  if (suffix->getName() == "_u8" ||
      suffix->getName() == "_u16" ||
      suffix->getName() == "_u32" ||
      suffix->getName() == "_u64" ||
      suffix->getName() == "_umx" ||
      suffix->getName() == "_i8" ||
      suffix->getName() == "_i16" ||
      suffix->getName() == "_i32" ||
      suffix->getName() == "_i64" ||
      suffix->getName() == "_imx" ||
      suffix->getName() == "_idx")
  {
    return;
  }

  auto const Loc = UDL->getBeginLoc();
  if (Loc.isInvalid())
    return;

  diag(Loc, "user-defined literals are forbidden");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
