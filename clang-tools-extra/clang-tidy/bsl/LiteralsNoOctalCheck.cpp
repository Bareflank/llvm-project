//===--- LiteralsNoOctalCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "LiteralsNoOctalCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"
#include "llvm/Support/Regex.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

static bool isOctalInteger(SourceLocation Loc,
                           const MatchFinder::MatchResult &Result) {
  auto Mgr = Result.SourceManager;
  auto Ctx = Result.Context;

  Token Tok;
  if (Lexer::getRawToken(Loc, Tok, *Mgr, Ctx->getLangOpts(), false))
    return false;

  if (!Tok.isLiteral())
    return false;

  auto Buf = Tok.getLiteralData();
  if (!Buf)
    return false;

  StringRef Str(Buf, Tok.getLength());

  if (!Str.startswith("0") || Str.startswith_lower("0x") ||
      Str.startswith_lower("0b"))
    return false;

  return Str.find_first_of("01234567", 1) != StringRef::npos;
}

static bool containsOctalEscape(SourceLocation Loc,
                                const MatchFinder::MatchResult &Res) {
  auto Mgr = Res.SourceManager;
  auto Ctx = Res.Context;

  Token Tok;
  if (Lexer::getRawToken(Loc, Tok, *Mgr, Ctx->getLangOpts(), false))
    return false;

  if (!Tok.isLiteral())
    return false;

  auto Buf = Tok.getLiteralData();
  if (!Buf)
    return false;

  StringRef Str(Buf, Tok.getLength());

  llvm::Regex OctalEsc1("[\\][1-7]");
  llvm::Regex OctalEsc2("[\\][0-7][0-7]");
  llvm::Regex OctalEsc3("[\\][0-7][0-7][0-7]");

  return OctalEsc3.match(Str) || OctalEsc2.match(Str) || OctalEsc1.match(Str);
}

void LiteralsNoOctalCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(stringLiteral().bind("str-lit"), this);
  Finder->addMatcher(integerLiteral().bind("int-lit"), this);
  Finder->addMatcher(characterLiteral().bind("char-lit"), this);
}

void LiteralsNoOctalCheck::check(const MatchFinder::MatchResult &Result) {
  checkString(Result);
  checkInteger(Result);
  checkCharacter(Result);
}

void LiteralsNoOctalCheck::checkInteger(const MatchFinder::MatchResult &Res) {
  auto Lit = Res.Nodes.getNodeAs<IntegerLiteral>("int-lit");
  if (!Lit)
    return;

  auto Loc = Lit->getBeginLoc();
  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  if (!isOctalInteger(Loc, Res))
    return;

  diag(Loc, "octal literal");
}

void LiteralsNoOctalCheck::checkString(const MatchFinder::MatchResult &Res) {
  auto Lit = Res.Nodes.getNodeAs<StringLiteral>("str-lit");
  if (!Lit)
    return;

  for (auto i = 0U; i < Lit->getNumConcatenated(); i++) {
    auto Loc = Lit->getStrTokenLoc(i);
    if (Loc.isInvalid() || Loc.isMacroID())
      continue;

    if (!containsOctalEscape(Loc, Res))
      continue;

    diag(Loc, "octal escape sequence");
  }
}

void LiteralsNoOctalCheck::checkCharacter(const MatchFinder::MatchResult &Res) {
  auto Lit = Res.Nodes.getNodeAs<CharacterLiteral>("char-lit");
  if (!Lit)
    return;

  auto Loc = Lit->getBeginLoc();
  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  if (!containsOctalEscape(Loc, Res))
    return;

  diag(Loc, "octal escape sequence");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
