//===--- VarBracedInitCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "VarBracedInitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(Expr, isValid) {
  return !Node.containsErrors();
}

AST_MATCHER(VarDecl, isCXXForRangeDecl) {
  return Node.isCXXForRangeDecl();
}

AST_MATCHER(VarDecl, hasListInitStyle) {
  return Node.getInitStyle() == VarDecl::InitializationStyle::ListInit;
}

void VarBracedInitCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    varDecl(
      hasInitializer(
        expr(
          isValid()
        )
      ),
      unless(
        anyOf(
          isImplicit(),
          parmVarDecl(),
          isCXXForRangeDecl(),
          hasListInitStyle()
        )
      )
    ).bind("var"),
    this
  );
}

void VarBracedInitCheck::check(const MatchFinder::MatchResult &Result) {
  auto const VD = Result.Nodes.getNodeAs<VarDecl>("var");
  if (VD->isInvalidDecl())
    return;

  auto const Loc = VD->getLocation();
  if (Loc.isInvalid())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(Loc);
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  auto const filename{File->tryGetRealPathName()};
  if (filename.find(".h") != std::string::npos)
    return;

  diag(Loc, "variable '%0' is not initialized via direct list initialization") << VD;
}

} // namespace bsl
} // namespace tidy
} // namespace clang
