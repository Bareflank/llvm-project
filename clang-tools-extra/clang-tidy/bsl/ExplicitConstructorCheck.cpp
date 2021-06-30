//===--- ExplicitConstructorCheck.cpp - clang-tidy ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ExplicitConstructorCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXConstructorDecl, isOutOfLine) {
  return Node.isOutOfLine();
}

AST_MATCHER(CXXConstructorDecl, hasZeroArgs) {
  return Node.getNumParams() == 0;
}

AST_MATCHER(CXXConstructorDecl, hasAtLeastTwoArgs) {
  return Node.getMinRequiredArguments() > 1;
}

AST_MATCHER(CXXConstructorDecl, firstParamIsParamPack) {
  return Node.getNumParams() == 1 && Node.getParamDecl(0)->isParameterPack();
}

AST_MATCHER(CXXConstructorDecl, isConcrete) {
  auto Parent = Node.getParent();
  bool IsTemplateClass = Parent->getDescribedClassTemplate() != nullptr;

  return !IsTemplateClass || Node.isTemplateInstantiation();
}

void ExplicitConstructorCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxConstructorDecl(isConcrete(),
                         unless(anyOf(isOutOfLine(),
                                      isExplicit(),
                                      isImplicit(),
                                      isDeleted(),
                                      isCopyConstructor(),
                                      isMoveConstructor(),
                                      hasZeroArgs(),
                                      hasAtLeastTwoArgs(),
                                      firstParamIsParamPack()))).bind("ctor"),
      this);
}

void ExplicitConstructorCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Ctor = Result.Nodes.getNodeAs<CXXConstructorDecl>("ctor");
  auto const ArgQualType = Ctor->getParamDecl(0)->getType();
  auto const *ArgType = ArgQualType.getTypePtrOrNull();

  if (!ArgType)
    return;

  if (!ArgType->isBuiltinType())
    return;

  auto const Loc = Ctor->getLocation();

  diag(Loc, "constructor %0 with single argument of builtin type"
            " must be marked explicit")
      << Ctor
      << FixItHint::CreateInsertion(Loc, "explicit ");

  return;
}

} // namespace bsl
} // namespace tidy
} // namespace clang
