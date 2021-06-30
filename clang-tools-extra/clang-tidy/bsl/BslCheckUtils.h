//===--- BslCheckUtils.h - clang-tidy-------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_CHECK_UTILS_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_CHECK_UTILS_H

#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/ASTMatchers/ASTMatchers.h"
#include "clang/Basic/AttrKinds.h"
#include "clang/Basic/CharInfo.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Basic/SourceLocation.h"
#include "clang/Basic/TargetInfo.h"
#include "clang/Lex/Lexer.h"

namespace clang {
namespace tidy {
namespace bsl {

// Returns the StringRef of the raw Token at the given SourceLocation
StringRef getRawTokenStr(SourceLocation Loc,
                         ast_matchers::MatchFinder::MatchResult const &Result);

Token getTokenAtLoc(SourceLocation Loc,
                    ast_matchers::MatchFinder::MatchResult const &Result,
                    IdentifierTable &IdentTable);

bool stmtContainsErrors(Stmt const *const stmt,
                        ast_matchers::MatchFinder::MatchResult const &Result);

} // namespace bsl
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_CHECK_UTILS_H
