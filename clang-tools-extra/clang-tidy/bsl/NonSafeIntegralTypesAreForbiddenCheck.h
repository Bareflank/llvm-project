//===--- NonSafeIntegralTypesAreForbiddenCheck.h - clang-tidy ---*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_NONSAFEINTEGRALTYPESAREFORBIDDENCHECK_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_NONSAFEINTEGRALTYPESAREFORBIDDENCHECK_H

#include "../ClangTidyCheck.h"

namespace clang {
namespace tidy {
namespace bsl {

class NonSafeIntegralTypesAreForbiddenCheck : public ClangTidyCheck {
public:
  NonSafeIntegralTypesAreForbiddenCheck(StringRef Name, ClangTidyContext *Context)
      : ClangTidyCheck(Name, Context) {}
  void registerMatchers(ast_matchers::MatchFinder *Finder) override;
  void check(const ast_matchers::MatchFinder::MatchResult &Result) override;
  void check_var_decl(const ast_matchers::MatchFinder::MatchResult &Result);
  void check_field_decl(const ast_matchers::MatchFinder::MatchResult &Result);
};

} // namespace bsl
} // namespace tidy
} // namespace clang

#endif // LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_NONSAFEINTEGRALTYPESAREFORBIDDENCHECK_H
