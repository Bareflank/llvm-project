//===--- NonSafeIntegralTypesAreForbiddenCheck.cpp - clang-tidy -----------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NonSafeIntegralTypesAreForbiddenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void NonSafeIntegralTypesAreForbiddenCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(varDecl().bind("decl"), this);
}

void NonSafeIntegralTypesAreForbiddenCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *VD = Result.Nodes.getNodeAs<VarDecl>("decl");
  auto const QT = VD->getType().getNonReferenceType().getCanonicalType().getUnqualifiedType();

  if (QT->isIntegerType()) {
    if (QT->isBooleanType())
      return;

    auto const name{QT.getAsString()};
    if (name == "char")
      return;

    auto const qualified_name{VD->getType().getUnqualifiedType().getAsString()};
    if (qualified_name == "bsl::exit_code")
      return;

    if (qualified_name == "syscall::bf_status_t")
      return;

    if (auto const *parent = VD->getParentFunctionOrMethod()) {
      if (auto const *FD = dyn_cast<FunctionDecl>(parent)) {
        if (FD->isExternC())
          return;

        if (FD->getNameAsString() == "main")
          return;
      }
    }

    if (!VD->getBeginLoc().isValid())
      return;

    FullSourceLoc FullLocation = Result.Context->getFullLoc(VD->getBeginLoc());
    if (auto const File = FullLocation.getFileEntry()) {
      auto const filename{File->tryGetRealPathName()};
      if (filename.find("add_lvalue_reference.hpp") != std::string::npos ||
          filename.find("add_pointer.hpp") != std::string::npos ||
          filename.find("add_rvalue_reference.hpp") != std::string::npos ||
          filename.find("aligned_union.hpp") != std::string::npos ||
          filename.find("alignment_of.hpp") != std::string::npos ||
          filename.find("basic_errc_type.hpp") != std::string::npos ||
          filename.find("byte.hpp") != std::string::npos ||
          filename.find("char_traits.hpp") != std::string::npos ||
          filename.find("construct_at.hpp") != std::string::npos ||
          filename.find("convert.hpp") != std::string::npos ||
          filename.find("debug.hpp") != std::string::npos ||
          filename.find("discard.hpp") != std::string::npos ||
          filename.find("exit_code.hpp") != std::string::npos ||
          filename.find("extent_base.hpp") != std::string::npos ||
          filename.find("forward.hpp") != std::string::npos ||
          filename.find("fmt_impl_integral.hpp") != std::string::npos ||
          filename.find("integer_sequence.hpp") != std::string::npos ||
          filename.find("integer_sequence_max.hpp") != std::string::npos ||
          filename.find("integer_sequence_min.hpp") != std::string::npos ||
          filename.find("integral_constant.hpp") != std::string::npos ||
          filename.find("is_nothrow_convertible.hpp") != std::string::npos ||
          filename.find("is_nothrow_destructible.hpp") != std::string::npos ||
          filename.find("move.hpp") != std::string::npos ||
          filename.find("numeric_limits.hpp") != std::string::npos ||
          filename.find("rank.hpp") != std::string::npos ||
          filename.find("safe_integral.hpp") != std::string::npos ||
          filename.find("source_location.hpp") != std::string::npos ||
          filename.find("swap.hpp") != std::string::npos) {
        return;
      }
    }
    else {
      return;
    }

    diag(VD->getBeginLoc(), "integral types like int, std::int32_t and bsl::int32 are forbidden. "
                            "Use bsl::safe_integral instead of %0") << qualified_name;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
