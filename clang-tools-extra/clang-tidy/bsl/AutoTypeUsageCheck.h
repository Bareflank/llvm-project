//===--- AutoTypeUsageCheck.h - clang-tidy ----------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_AUTOTYPEUSAGECHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_AUTOTYPEUSAGECHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace bsl {

/// Checks that the auto specifier is only used
/// (1) to declare that a variable has the same type as return type of a
///    function call
/// (2) to declare that a variable has the same type as
///    initializer of non-fundamental type
/// (3) to declare parameters of a generic
///    lambda expression
/// (4) to declare a function (template) using trailing return
///    type syntax.
/// Note: literal types (except strings) and list initializers are not allowed
///
/// For the user-facing documentation see:
/// http://clang.llvm.org/extra/clang-tidy/checks/bsl-auto-type-usage.html
class AutoTypeUsageCheck : public ClangTidyCheck {
public:
  AutoTypeUsageCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  void check_list(const ast_matchers::MatchFinder::MatchResult &Result);
  void check_fundamental(const ast_matchers::MatchFinder::MatchResult &Result);
  void check_trailing(const ast_matchers::MatchFinder::MatchResult &Result);
};

} // namespace bsl
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_AUTOTYPEUSAGECHECK_H
