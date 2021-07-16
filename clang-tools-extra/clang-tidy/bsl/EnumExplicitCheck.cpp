//===--- EnumExplicitCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "EnumExplicitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void EnumExplicitCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(enumDecl().bind("enum"), this);
}

void EnumExplicitCheck::check(const MatchFinder::MatchResult &Result) {
  auto Enum = Result.Nodes.getNodeAs<EnumDecl>("enum");

  auto Loc = Enum->getLocation();
  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(Loc);
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  auto const filename{File->tryGetRealPathName()};
  if (filename.find(".h") != std::string::npos)
    return;

  auto Range = Enum->getIntegerTypeRange();
  if (Range.isValid())
    return;

  diag(Loc, "enum declaration does not explicitly state underlying type");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
