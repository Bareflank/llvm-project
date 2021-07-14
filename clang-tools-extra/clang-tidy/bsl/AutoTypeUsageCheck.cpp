//===--- AutoTypeUsageCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "AutoTypeUsageCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(ValueDecl, isFundamentalType) {
  return Node.getType().getTypePtr()->isFundamentalType();
}

AST_MATCHER(ParmVarDecl, isTemplate) {
  return Node.isTemplated();
}

void AutoTypeUsageCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    valueDecl(
      hasType(autoType()),
      hasDescendant(cxxStdInitializerListExpr())
    ).bind("list"),
    this
  );

  Finder->addMatcher(
    valueDecl(
      hasType(autoType()),
      isFundamentalType(),
      unless(
        has(callExpr())
      )
    ).bind("type"),
    this
  );

  Finder->addMatcher(
    functionDecl(
      returns(autoType()),
      unless(
        allOf(
          hasDescendant(parmVarDecl(isTemplate())),
          hasTrailingReturn()
        )
      )
    ).bind("trail"),
    this
  );
}

void AutoTypeUsageCheck::check_list(const MatchFinder::MatchResult &Result)
{
  auto const VD = Result.Nodes.getNodeAs<ValueDecl>("list");
  if (nullptr == VD)
    return;

  if (VD->isInvalidDecl())
    return;

  auto const Loc = VD->getLocation();
  if (Loc.isInvalid())
    return;

  diag(Loc, "auto cannot be used for list initializers");
}

void AutoTypeUsageCheck::check_fundamental(const MatchFinder::MatchResult &Result)
{
  auto const VD = Result.Nodes.getNodeAs<ValueDecl>("type");
  if (nullptr == VD)
    return;

  if (VD->isInvalidDecl())
    return;

  auto const Loc = VD->getLocation();
  if (Loc.isInvalid())
    return;

  diag(Loc, "auto cannot be used to declare variable of fundamental type");
}

void AutoTypeUsageCheck::check_trailing(const MatchFinder::MatchResult &Result)
{
  auto const FD = Result.Nodes.getNodeAs<FunctionDecl>("trail");
  if (nullptr == FD)
    return;

  if (FD->isInvalidDecl())
    return;

  auto const Loc = FD->getLocation();
  if (Loc.isInvalid())
    return;

  if(FD->getReturnType()->isVoidType())
    return;

  if (auto const MD = dyn_cast<CXXMethodDecl>(FD)) {
    if (MD->getParent()->isLambda())
      return;
  }

  diag(Loc, "auto can only be used for declaring function templates with a trailing return");
}

void AutoTypeUsageCheck::check(const MatchFinder::MatchResult &Result)
{
  check_list(Result);
  check_fundamental(Result);
  check_trailing(Result);
}

} // namespace bsl
} // namespace tidy
} // namespace clang
