//===--- PassByReferenceCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "PassByReferenceCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void PassByReferenceCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(parmVarDecl().bind("decl"), this);
}

void PassByReferenceCheck::check(const MatchFinder::MatchResult &Result) {
  auto const VD = Result.Nodes.getNodeAs<ParmVarDecl>("decl");
  if (nullptr == VD)
    return;

  if (VD->isInvalidDecl())
    return;

  if (VD->isInStdNamespace())
    return;

  auto const name = VD->getName();
  if (name.empty())
    return;

  auto const type = VD->getType();
  if (type->isDependentType())
    return;

  if (type->isMemberPointerType())
    return;

  if (type->isMemberFunctionPointerType())
    return;

  if (type->isMemberDataPointerType())
    return;

  if (!type->isObjectType())
    return;

  auto const DC = VD->getParentFunctionOrMethod();
  if (nullptr == DC)
    return;

  auto const FD = dyn_cast<FunctionDecl>(DC);
  if (nullptr == FD)
    return;

  if (FD->isInvalidDecl())
    return;

  if (FD->isDeleted())
    return;

  auto const Loc = VD->getLocation();
  if (Loc.isInvalid())
    return;

  auto const non_ref_type = type.getNonReferenceType();
  if (non_ref_type->isDependentType())
    return;

  auto const non_ref_type_name = non_ref_type.getCanonicalType().getAsString();
  if (non_ref_type_name.find("std::") != std::string::npos)
    return;

  auto const size = Result.Context->getTypeSize(non_ref_type);
  if (0 == size)
    return;

  if (size > 64) {
    if (type->isReferenceType())
      return;

    diag(Loc, "%0 is %1 bytes in size which is larger than 8 bytes and should be passed by reference, not value")
        << type << std::to_string(size / 8);
  }
  else {
    if (!type->isReferenceType())
      return;

    if (!non_ref_type.isConstQualified())
      return;

    diag(Loc, "%0 is %1 bytes in size which is 8 bytes or smaller and should be passed by value, not reference")
        << type << std::to_string(size / 8);
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
