//===--- LiteralsUnsignedSuffixCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LiteralsUnsignedSuffixCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void LiteralsUnsignedSuffixCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    stmt(integerLiteral().bind("lit"),
         unless(anyOf(hasAncestor(isImplicit()),
                      hasAncestor(substNonTypeTemplateParmExpr())))),
    this);
}

void LiteralsUnsignedSuffixCheck::check(const MatchFinder::MatchResult &Result) {
  auto Lit = Result.Nodes.getNodeAs<IntegerLiteral>("lit");
  if (!Lit)
    return;

  auto Loc = Lit->getBeginLoc();
  auto Mgr = Result.SourceManager;
  auto Ctx = Result.Context;

  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  auto Type = Lit->getType().getTypePtrOrNull();
  if (Type && !Type->isUnsignedIntegerType())
    return;

  Token Tok;
  if (Lexer::getRawToken(Loc, Tok, *Mgr, Ctx->getLangOpts(), false))
    return;

  if (!Tok.isLiteral())
    return;

  auto Buf = Tok.getLiteralData();
  if (!Buf)
    return;

  StringRef Str(Buf, Tok.getLength());

  if (Str.contains_lower('u'))
    return;

  if (!Str.startswith("0")) {
    diag(Loc, "unsigned decimal literal does not end with 'U'");
  } else if (Str.startswith_lower("0x")) {
      diag(Loc, "unsigned hex literal does not end with 'U'");
  } else if (Str.startswith_lower("0b")) {
      diag(Loc, "unsigned binary literal does not end with 'U'");
  } else {
      diag(Loc, "unsigned octal literal does not end with 'U'");
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
