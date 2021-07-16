//===--- TypesFixedWidthIntsCheck.cpp - clang-tidy---------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TypesFixedWidthIntsCheck.h"
#include "BslCheckUtils.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void TypesFixedWidthIntsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(typeLoc(loc(isInteger())).bind("loc"), this);
  IdentTable = std::make_unique<IdentifierTable>(getLangOpts());
}

void TypesFixedWidthIntsCheck::check(const MatchFinder::MatchResult &Result) {
  auto TL = *Result.Nodes.getNodeAs<TypeLoc>("loc");
  auto Loc = TL.getBeginLoc();

  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(Loc);
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  // These have to have non-fixed width types to work at all
  auto const filename{File->tryGetRealPathName()};
  if (filename.find(".h") != std::string::npos ||
      filename.find("carray.hpp") != std::string::npos ||
      filename.find("char_type.hpp") != std::string::npos ||
      filename.find("cstdint.hpp") != std::string::npos) {
    return;
  }

  if (auto QL = TL.getAs<QualifiedTypeLoc>())
    TL = QL.getUnqualifiedLoc();

  auto Builtin = TL.getAs<BuiltinTypeLoc>();
  if (!Builtin)
    return;

  auto Tok = getTokenAtLoc(Loc, Result, *IdentTable);
  if (!Tok.isOneOf(tok::kw_short, tok::kw_long, tok::kw_signed,
                   tok::kw_unsigned, tok::kw_char, tok::kw_int,
                   tok::kw_wchar_t))
    return;

  diag(Loc, "replace %0 with a fixed-width type") << Builtin.getType();
}

} // namespace bsl
} // namespace tidy
} // namespace clang
