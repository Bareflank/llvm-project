//===--- ConstObjStdMoveCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ConstObjStdMoveCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void ConstObjStdMoveCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      callExpr(
          hasArgument(0, declRefExpr(hasType(qualType(isConstQualified()))).bind("ref")),
          callee(functionDecl(anyOf(hasName("::std::move"),
                                    hasName("::bsl::move"))).bind("decl"))
          ).bind("move"),
      this);
}

void ConstObjStdMoveCheck::check(const MatchFinder::MatchResult &Result) {
  auto const Call = Result.Nodes.getNodeAs<CallExpr>("move");
  auto const Loc = Call->getBeginLoc();

  if (Loc.isInvalid())
      return;

  auto const Func = Result.Nodes.getNodeAs<FunctionDecl>("decl");
  auto const Ref = Result.Nodes.getNodeAs<DeclRefExpr>("ref");
  auto const ArgName = Ref->getFoundDecl()->getName();
  auto const ArgLoc = Ref->getFoundDecl()->getBeginLoc();

  diag(Loc, "invoking %0 with const argument %1")
      << Func->getQualifiedNameAsString() << ArgName;

  diag(ArgLoc,
       "argument %0 declared const here", DiagnosticIDs::Note) << ArgName;
}

} // namespace bsl
} // namespace tidy
} // namespace clang
