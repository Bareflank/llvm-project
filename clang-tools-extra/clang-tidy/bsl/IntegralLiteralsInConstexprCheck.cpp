//===--- IntegralLiteralsInConstexprCheck.cpp - clang-tidy ----------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IntegralLiteralsInConstexprCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void IntegralLiteralsInConstexprCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    varDecl(
      hasInitializer(
        integerLiteral().bind("int-lit")
      )
    ),
  this);

  Finder->addMatcher(
    varDecl(
      hasInitializer(
        initListExpr(
          hasInit(
            0, integerLiteral().bind("int-lit")
          )
        )
      )
    ),
  this);

  Finder->addMatcher(
    varDecl(
      hasInitializer(
        userDefinedLiteral()
      ),
      unless(
        isConstexpr()
      )
    ).bind("decl"),
  this);

  Finder->addMatcher(
    varDecl(
      hasInitializer(
        initListExpr(
          hasInit(
            0, userDefinedLiteral()
          )
        )
      ),
      unless(
        isConstexpr()
      )
    ).bind("decl"),
  this);
}

void IntegralLiteralsInConstexprCheck::check_integral_literals(const MatchFinder::MatchResult &Result) {
  auto IL = Result.Nodes.getNodeAs<IntegerLiteral>("int-lit");
  if (nullptr == IL)
    return;

  auto const Loc = IL->getExprLoc();
  if (Loc.isInvalid())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(Loc);
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  auto const filename{File->tryGetRealPathName()};
  if (filename.find(".h") != std::string::npos ||
      filename.find(".c") != std::string::npos ||
      filename.find("add_lvalue_reference.hpp") != std::string::npos ||
      filename.find("add_pointer.hpp") != std::string::npos ||
      filename.find("add_rvalue_reference.hpp") != std::string::npos ||
      filename.find("array.hpp") != std::string::npos ||
      filename.find("basic_errc_type.hpp") != std::string::npos ||
      filename.find("basic_string_view.hpp") != std::string::npos ||
      filename.find("char_traits.hpp") != std::string::npos ||
      filename.find("convert.hpp") != std::string::npos ||
      filename.find("cstring.hpp") != std::string::npos ||
      filename.find("debug.hpp") != std::string::npos ||
      filename.find("exit_code.hpp") != std::string::npos ||
      filename.find("float_denorm_style.hpp") != std::string::npos ||
      filename.find("float_round_style.hpp") != std::string::npos ||
      filename.find("fmt.hpp") != std::string::npos ||
      filename.find("fmt_align.hpp") != std::string::npos ||
      filename.find("fmt_fsm.hpp") != std::string::npos ||
      filename.find("fmt_impl_align.hpp") != std::string::npos ||
      filename.find("fmt_impl_bool.hpp") != std::string::npos ||
      filename.find("fmt_impl_char_type.hpp") != std::string::npos ||
      filename.find("fmt_impl_integral.hpp") != std::string::npos ||
      filename.find("fmt_impl_integral_helpers.hpp") != std::string::npos ||
      filename.find("fmt_impl_integral_info.hpp") != std::string::npos ||
      filename.find("fmt_options.hpp") != std::string::npos ||
      filename.find("fmt_sign.hpp") != std::string::npos ||
      filename.find("fmt_type.hpp") != std::string::npos ||
      filename.find("from_chars.hpp") != std::string::npos ||
      filename.find("likely.hpp") != std::string::npos ||
      filename.find("numeric_limits.hpp") != std::string::npos ||
      filename.find("safe_integral.hpp") != std::string::npos ||
      filename.find("source_location.hpp") != std::string::npos ||
      filename.find("reverse_iterator.hpp") != std::string::npos ||
      filename.find("unlikely_contract.hpp") != std::string::npos ||
      filename.find("unlikely_assert.hpp") != std::string::npos ||
      filename.find("unlikely.hpp") != std::string::npos ||
      filename.find("ut.hpp") != std::string::npos) {
    return;
  }

  diag(Loc, "integral literals are not allowed. use the safe_integral literals from "
            "'bsl/convert.hpp' such as _u8/16/32/64/max and _i8/16/32/64/max");
}

void IntegralLiteralsInConstexprCheck::check_user_defined_literals(const MatchFinder::MatchResult &Result) {
  auto const *VD = Result.Nodes.getNodeAs<VarDecl>("decl");
  if (nullptr == VD)
    return;

  auto const Loc = VD->getBeginLoc();
  if (Loc.isInvalid())
    return;

  diag(Loc, "literals must be used in a constexpr");
}

void IntegralLiteralsInConstexprCheck::check(const MatchFinder::MatchResult &Result) {
  check_integral_literals(Result);
  check_user_defined_literals(Result);
}

} // namespace bsl
} // namespace tidy
} // namespace clang
