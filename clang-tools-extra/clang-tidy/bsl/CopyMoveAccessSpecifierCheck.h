//===--- CopyMoveAccessSpecifierCheck.h - clang-tidy ------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_COPYMOVEACCESSSPECIFIERCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_COPYMOVEACCESSSPECIFIERCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace bsl {

/// Checks that copy and move constructors and copy assignment and move
/// assignment operators are declared protected or deleted in base class
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/bsl-copy-move-access-specifier.html
class CopyMoveAccessSpecifierCheck : public ClangTidyCheck {
public:
  CopyMoveAccessSpecifierCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
};

} // namespace bsl
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_COPYMOVEACCESSSPECIFIERCHECK_H
