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
  auto const *ICE = Result.Nodes.getNodeAs<ImplicitCastExpr>("cast");
  if (nullptr == ICE)
    return;

  auto const Loc = ICE->getBeginLoc();
  if (Loc.isInvalid())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(Loc);
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  auto const filename{File->tryGetRealPathName()};

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

  // Decaying a C-Style string is ok so long as it is a literal. Also,
  // bsl::array needs to do this to work so that others do not have to do this.
  if (ICE->getCastKind() == CK_ArrayToPointerDecay) {
    if (isa<StringLiteral>(ICE->getSubExpr()))
      return;

    if (filename.find(".h") != std::string::npos ||
        filename.find(".c") != std::string::npos ||
        filename.find("array.hpp") != std::string::npos ||
        filename.find("fmt.hpp") != std::string::npos) {
      return;
    }
  }

  // Some BSL capabilites require implicit casts to function properly
  if (ICE->getCastKind() == CK_DerivedToBase) {
    if (filename.find("invoke_impl_mfp_o.hpp") != std::string::npos ||
        filename.find("invoke_impl_mfp_p.hpp") != std::string::npos ||
        filename.find("invoke_impl_mfp_r.hpp") != std::string::npos ||
        filename.find("invoke_impl_mop_o.hpp") != std::string::npos ||
        filename.find("invoke_impl_mop_p.hpp") != std::string::npos ||
        filename.find("invoke_impl_mop_r.hpp") != std::string::npos) {
      return;
    }
  }

  // Some BSL capabilites require implicit casts to function properly
  if (ICE->getCastKind() == CK_IntegralToBoolean) {
    if (filename.find(".h") != std::string::npos ||
        filename.find(".c") != std::string::npos ||
        filename.find("add_lvalue_reference.hpp") != std::string::npos ||
        filename.find("add_pointer.hpp") != std::string::npos ||
        filename.find("add_rvalue_reference.hpp") != std::string::npos ||
        filename.find("is_nothrow_convertible.hpp") != std::string::npos ||
        filename.find("is_nothrow_destructible.hpp") != std::string::npos) {
      return;
    }
  }

  // Some BSL capabilites require implicit casts to function properly
  if (ICE->getCastKind() == CK_IntegralCast) {
    if (filename.find("integer.hpp") != std::string::npos ||
        filename.find("is_nothrow_convertible.hpp") != std::string::npos ||
        filename.find("out_line.hpp") != std::string::npos ||
        filename.find("safe_idx.hpp") != std::string::npos ||
        filename.find("safe_integral.hpp") != std::string::npos) {
      return;
    }
  }

  // If we are casting from a boolean or char type to an int implicitly,
  // that is ok. Note that a signed or unsigned char type is not allowed.
  if (ICE->getType()->isIntegerType()) {
    if (auto const *SE = ICE->getSubExpr()) {
      if (SE->getType()->isBooleanType())
        return;

      auto const name{SE->getType().getAsString()};
      if (name.find("enum ") != std::string::npos)
        return;

      if (name == "char")
        return;

      if (name == "bsl::char_type")
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
    diag(Loc, "implicit conversions are forbidden (%0 from '%1' to '%2')")
      << ICE->getCastKindName()
      << ICE->getSubExpr()->getType().getAsString()
      << ICE->getType().getAsString();
  }
  else {
    diag(Loc, "implicit conversions are forbidden (%0)")
      << ICE->getCastKindName();
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
