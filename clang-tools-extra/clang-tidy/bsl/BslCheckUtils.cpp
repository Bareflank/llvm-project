//===--- BslCheckUtils.cpp - clang-tidy -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "BslCheckUtils.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

StringRef getRawTokenStr(SourceLocation Loc,
                         const MatchFinder::MatchResult &Result) {
  if (Loc.isInvalid() || Loc.isMacroID())
    return StringRef();

  auto Mgr = Result.SourceManager;
  auto Ctx = Result.Context;

  Token Tok;
  if (Lexer::getRawToken(Loc, Tok, *Mgr, Ctx->getLangOpts(), false))
    return StringRef();

  if (!Tok.isLiteral())
    return StringRef();

  auto Buf = Tok.getLiteralData();
  if (!Buf)
    return StringRef();

  return StringRef(Buf, Tok.getLength());
}

// This function is taken from ../google/IntegerTypesCheck.cpp. The check
// below is similar except that is matches additional keywords and doesn't
// provide a suggested replacement.
Token getTokenAtLoc(SourceLocation Loc,
                           const MatchFinder::MatchResult &MatchResult,
                           IdentifierTable &IdentTable) {
  Token Tok;

  if (Lexer::getRawToken(Loc, Tok, *MatchResult.SourceManager,
                         MatchResult.Context->getLangOpts(), false))
    return Tok;

  if (Tok.is(tok::raw_identifier)) {
    IdentifierInfo &Info = IdentTable.get(Tok.getRawIdentifier());
    Tok.setIdentifierInfo(&Info);
    Tok.setKind(Info.getTokenID());
  }

  return Tok;
}

} // namespace bsl
} // namespace tidy
} // namespace clang
