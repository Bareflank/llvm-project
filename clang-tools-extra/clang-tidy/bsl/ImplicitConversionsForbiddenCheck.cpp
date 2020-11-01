//===--- ImplicitConversionsForbiddenCheck.cpp - clang-tidy ---------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ImplicitConversionsForbiddenCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void ImplicitConversionsForbiddenCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(implicitCastExpr().bind("cast"), this);
}

void ImplicitConversionsForbiddenCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *ICE = Result.Nodes.getNodeAs<ImplicitCastExpr>("cast");

  // If we are making an explicit cast using static_cast, the implicit case can
  // be safely ignored.
  if (ICE->isPartOfExplicitCast())
    return;

  // The following casts are ok and can be ignored. For more information
  // on these, please see:
  // https://github.com/llvm-mirror/clang/blob/master/include/clang/AST/OperationKinds.def
  if (ICE->getCastKind() == CK_Dependent ||
      ICE->getCastKind() == CK_BitCast ||
      ICE->getCastKind() == CK_LValueToRValue ||
      ICE->getCastKind() == CK_NoOp ||
      ICE->getCastKind() == CK_UncheckedDerivedToBase ||
      ICE->getCastKind() == CK_FunctionToPointerDecay ||
      ICE->getCastKind() == CK_NullToPointer ||
      ICE->getCastKind() == CK_NullToMemberPointer ||
      ICE->getCastKind() == CK_MemberPointerToBoolean ||
      ICE->getCastKind() == CK_PointerToBoolean ||
      ICE->getCastKind() == CK_ToVoid ||
      ICE->getCastKind() == CK_BuiltinFnToFnPtr ||
      ICE->getCastKind() == CK_ConstructorConversion)
    return;

  // Decaying a C-Style string is ok so long as it is a literal.
  if (ICE->getCastKind() == CK_ArrayToPointerDecay) {
    if (isa<StringLiteral>(ICE->getSubExpr()))
      return;
  }

  // If we are casting from a boolean or char type to an int implicitly,
  // that is ok. Note that a signed or unsigned char type is not allowed.
  if (ICE->getType()->isIntegerType()) {
    if (auto const *SE = ICE->getSubExpr()) {
      if (SE->getType()->isBooleanType())
        return;

      if (SE->getType().getAsString() == "char")
        return;

      if (SE->getType().getAsString() == "bsl::char_type")
        return;
    }
  }

  // Boolean conversion operators are treated as implicit even thorugh they
  // are marked as explicit, so we ignore them here.
  if (auto const *SE = ICE->getSubExpr()) {
    if (auto const *CXXMCE = dyn_cast<CXXMemberCallExpr>(SE)) {
      if (auto const *M = CXXMCE->getMethodDecl()) {
        if (M->getNameAsString() == "operator bool")
          return;
      }
    }
  }

  if (ICE->getCastKind() == CK_IntegralCast) {
    diag(ICE->getBeginLoc(), "implicit conversions are forbidden (%0 from %1 to %2)")
      << ICE->getCastKindName() << ICE->getSubExpr()->getType().getAsString() << ICE->getType().getAsString();
  }
  else {
    diag(ICE->getBeginLoc(), "implicit conversions are forbidden (%0)") << ICE->getCastKindName();
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
