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
  Finder->addMatcher(varDecl().bind("var-decl"), this);
  Finder->addMatcher(fieldDecl().bind("field-decl"), this);
}

void NonSafeIntegralTypesAreForbiddenCheck::check_var_decl(const MatchFinder::MatchResult &Result) {
  if (!Result.Context->getLangOpts().CPlusPlus)
    return;

  auto const *VD = Result.Nodes.getNodeAs<VarDecl>("var-decl");
  if (nullptr == VD)
    return;

  auto const Loc = VD->getBeginLoc();
  if (Loc.isInvalid())
    return;

  if (VD->isExternC())
    return;

  if (VD->hasExternalFormalLinkage())
    return;

  auto const QT = VD->getType().getNonReferenceType().getCanonicalType().getUnqualifiedType();
  if (!QT->isIntegerType())
    return;

  if (QT->isBooleanType())
    return;

  auto const name{QT.getAsString()};
  auto const qualified_name{VD->getType().getUnqualifiedType().getAsString()};

  if (qualified_name.find("enum ") != std::string::npos)
    return;

  if (name == "char")
    return;

  if (qualified_name == "bsl::exit_code")
    return;

  if (qualified_name == "bf_status_t::value_type")
    return;

  if (auto const *parent = VD->getParentFunctionOrMethod()) {
    if (auto const *FD = dyn_cast<FunctionDecl>(parent)) {
      if (FD->isExternC())
        return;

      if (FD->getNameAsString() == "main")
        return;
    }
  }

  FullSourceLoc FullLocation = Result.Context->getFullLoc(VD->getBeginLoc());
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  auto const filename{File->tryGetRealPathName()};
  if (filename.find(".h") != std::string::npos ||
      filename.find("add_lvalue_reference.hpp") != std::string::npos ||
      filename.find("add_pointer.hpp") != std::string::npos ||
      filename.find("add_rvalue_reference.hpp") != std::string::npos ||
      filename.find("aligned_union.hpp") != std::string::npos ||
      filename.find("alignment_of.hpp") != std::string::npos ||
      filename.find("basic_errc_type.hpp") != std::string::npos ||
      filename.find("carray.hpp") != std::string::npos ||
      filename.find("char_traits.hpp") != std::string::npos ||
      filename.find("construct_at.hpp") != std::string::npos ||
      filename.find("convert.hpp") != std::string::npos ||
      filename.find("cstdint.hpp") != std::string::npos ||
      filename.find("debug.hpp") != std::string::npos ||
      filename.find("debug_levels.hpp") != std::string::npos ||
      filename.find("discard.hpp") != std::string::npos ||
      filename.find("exit_code.hpp") != std::string::npos ||
      filename.find("extent_base.hpp") != std::string::npos ||
      filename.find("forward.hpp") != std::string::npos ||
      filename.find("fmt.hpp") != std::string::npos ||
      filename.find("fmt_impl_integral.hpp") != std::string::npos ||
      filename.find("integer.hpp") != std::string::npos ||
      filename.find("integer_sequence.hpp") != std::string::npos ||
      filename.find("integer_sequence_max.hpp") != std::string::npos ||
      filename.find("integer_sequence_min.hpp") != std::string::npos ||
      filename.find("integral_constant.hpp") != std::string::npos ||
      filename.find("is_nothrow_convertible.hpp") != std::string::npos ||
      filename.find("is_nothrow_destructible.hpp") != std::string::npos ||
      filename.find("move.hpp") != std::string::npos ||
      filename.find("numeric_limits.hpp") != std::string::npos ||
      filename.find("rank.hpp") != std::string::npos ||
      filename.find("safe_idx.hpp") != std::string::npos ||
      filename.find("safe_integral.hpp") != std::string::npos ||
      filename.find("source_location.hpp") != std::string::npos ||
      filename.find("swap.hpp") != std::string::npos) {
    return;
  }

  diag(Loc, "integral types like int, std::int32_t and bsl::int32 are forbidden. "
            "Use bsl::safe_integral instead of '%0'") << qualified_name;
}

void NonSafeIntegralTypesAreForbiddenCheck::check_field_decl(const MatchFinder::MatchResult &Result) {
  auto const *FD = Result.Nodes.getNodeAs<FieldDecl>("field-decl");
  if (nullptr == FD)
    return;

  auto const Loc = FD->getBeginLoc();
  if (Loc.isInvalid())
    return;

  auto const Record = FD->getParent();
  if (nullptr == Record)
    return;

  if (Record->isStruct())
    return;

  auto const QT = FD->getType().getNonReferenceType().getCanonicalType().getUnqualifiedType();
  if (!QT->isIntegerType())
    return;

  if (QT->isBooleanType())
    return;

  auto const name{QT.getAsString()};
  auto const qualified_name{FD->getType().getUnqualifiedType().getAsString()};

  if (qualified_name.find("enum ") != std::string::npos)
    return;

  if (name == "char")
    return;

  if (auto const *parent = FD->getParentFunctionOrMethod()) {
    if (auto const *FD = dyn_cast<FunctionDecl>(parent)) {
      if (FD->isExternC())
        return;
    }
  }

  FullSourceLoc FullLocation = Result.Context->getFullLoc(FD->getBeginLoc());
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  auto const filename{File->tryGetRealPathName()};
  if (filename.find("basic_errc_type.hpp") != std::string::npos ||
      filename.find("fmt.hpp") != std::string::npos ||
      filename.find("safe_idx.hpp") != std::string::npos ||
      filename.find("safe_integral.hpp") != std::string::npos ||
      filename.find("source_location.hpp") != std::string::npos ||
      filename.find("span.hpp") != std::string::npos) {
    return;
  }

  diag(Loc, "integral types like int, std::int32_t and bsl::int32 are forbidden. "
            "Use bsl::safe_integral instead of '%0'") << qualified_name;
}

void NonSafeIntegralTypesAreForbiddenCheck::check(const MatchFinder::MatchResult &Result) {
  check_var_decl(Result);
  check_field_decl(Result);
}

} // namespace bsl
} // namespace tidy
} // namespace clang
