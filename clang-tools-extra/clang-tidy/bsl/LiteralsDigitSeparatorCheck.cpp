//===--- LiteralsDigitSeparatorCheck.cpp - clang-tidy ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LiteralsDigitSeparatorCheck.h"
#include "BslCheckUtils.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "llvm/Support/Regex.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void LiteralsDigitSeparatorCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(integerLiteral().bind("lit"), this);

  Hex = std::make_unique<llvm::Regex>("^0[xX]([[:xdigit:]]){1,2}('([[:xdigit:]]){2})+[ulUL]*$");
  Bin = std::make_unique<llvm::Regex>("^0[bB]([01]){1,4}('([01]){4})+[ulUL]*$");
  Dec = std::make_unique<llvm::Regex>("^([[:digit:]]){1,3}('([[:digit:]]){3})+[ulUL]*$");
}

void LiteralsDigitSeparatorCheck::check(const MatchFinder::MatchResult &Result) {
  // Separators were introduced by c++14. Bail early if we are checking pre-14 code.
  if (!Result.Context->getLangOpts().CPlusPlus14)
    return;

  const auto Lit = Result.Nodes.getNodeAs<IntegerLiteral>("lit");
  if (!Lit)
    return;

  const auto Loc = Lit->getBeginLoc();
  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  const auto Str = getRawTokenStr(Loc, Result);
  if (!Str.contains("'"))
    return;

  // Hex literal separators must be every two digits
  if (Str.startswith_lower("0x")) {
    if (!Hex->isValid())
      return;

    if (Hex->match(Str))
      return;

    diag(Loc, "separators not two hex digits apart "
              "(starting from least-significant digit)");
    return;
  }

  // Binary literal separators must be every four digits
  if (Str.startswith_lower("0b")) {
    if (!Bin->isValid())
      return;

    if (Bin->match(Str))
      return;

    diag(Loc, "separators not four binary digits apart "
              "(starting from least-significant digit)");
    return;
  }

  // Decimal literal separators must be every three digits
  if (!Dec->isValid())
    return;

  if (Dec->match(Str))
    return;

  diag(Loc, "separators not three decimal digits apart "
            "(starting from least-significant digit)");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
