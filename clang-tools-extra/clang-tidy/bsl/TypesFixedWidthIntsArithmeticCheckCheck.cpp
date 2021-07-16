//===--- TypesFixedWidthIntsArithmeticCheckCheck.cpp - clang-tidy ---------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "TypesFixedWidthIntsArithmeticCheckCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Basic/IdentifierTable.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void TypesFixedWidthIntsArithmeticCheckCheck::registerMatchers(MatchFinder *Finder) {
  auto BinaryOps = hasAnyOperatorName("+", "-", "*", "/", "%", "^", "&", "|",
    "~", "<", ">", "+=", "-=", "*=", "/=", "%=", "^=", "&=", "|=", "<<",
    ">>", ">>=", "<<=", "++", "--");

  Finder->addMatcher(binaryOperator(BinaryOps, unless(isTypeDependent())).bind("op"), this);
}

void TypesFixedWidthIntsArithmeticCheckCheck::check(const MatchFinder::MatchResult &Result) {
  if (!Result.Context->getLangOpts().CPlusPlus)
    return;

  auto Op = Result.Nodes.getNodeAs<BinaryOperator>("op");
  auto Loc = Op->getOperatorLoc();

  if (Loc.isInvalid() || Loc.isMacroID())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(Loc);
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  // These have to have non-fixed width types to work at all
  auto const filename{File->tryGetRealPathName()};
  if (filename.find(".h") != std::string::npos ||
      filename.find("basic_errc_type.hpp") != std::string::npos ||
      filename.find("carray.hpp") != std::string::npos ||
      filename.find("convert.hpp") != std::string::npos ||
      filename.find("debug.hpp") != std::string::npos ||
      filename.find("extent_base.hpp") != std::string::npos ||
      filename.find("integer_sequence_max.hpp") != std::string::npos ||
      filename.find("integer_sequence_min.hpp") != std::string::npos ||
      filename.find("numeric_limits.hpp") != std::string::npos ||
      filename.find("safe_integral.hpp") != std::string::npos) {
    return;
  }

  auto LHS = Op->getLHS()->IgnoreImpCasts();
  auto RHS = Op->getRHS()->IgnoreImpCasts();

  QualType LHSType = LHS->getType().getNonReferenceType().getCanonicalType().getUnqualifiedType();
  QualType RHSType = RHS->getType().getNonReferenceType().getCanonicalType().getUnqualifiedType();

  auto const LHSName{LHSType.getAsString()};
  auto const RHSName{RHSType.getAsString()};

  if (LHSName == "char" && RHSName == "char")
    return;

  if (!LHSType->isIntegerType())
    return;

  if (!RHSType->isIntegerType())
    return;

  if (LHSType->isBooleanType())
    return;

  if (RHSType->isBooleanType())
    return;

  diag(Loc, "replace %0 or %1 with a safe_integral type") << LHS->getType() << RHS->getType();
}

} // namespace bsl
} // namespace tidy
} // namespace clang
