//===--- SpecialMemberFunctionsCheck.cpp - clang-tidy ---------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SpecialMemberFunctionsCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void SpecialMemberFunctionsCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    cxxRecordDecl(
      hasDefinition(),
      isClass(),
      unless(
        isImplicit()
      )
    ).bind("class"),
    this
  );
}

void SpecialMemberFunctionsCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *D = Result.Nodes.getNodeAs<CXXRecordDecl>("class");
  auto const Loc = D->getLocation();

  if (Loc.isInvalid())
    return;

  bool hasDefaultConstructor{false};
  bool hasCopyConstructor{false};
  bool hasMoveConstructor{false};
  bool hasCopyAssignment{false};
  bool hasMoveAssignment{false};
  bool hasDestructor{false};

  for (auto const &M : D->methods()) {
    if (M->isImplicit())
      continue;

    if (auto d{dyn_cast<CXXConstructorDecl>(M)}) {
      if (d->isDefaultConstructor())
        hasDefaultConstructor = true;
      if (d->isCopyConstructor())
        hasCopyConstructor = true;
      if (d->isMoveConstructor())
        hasMoveConstructor = true;

      continue;
    }

    if (auto d{dyn_cast<CXXDestructorDecl>(M)}) {
      hasDestructor = true;
      continue;
    }

    if (M->isCopyAssignmentOperator())
      hasCopyAssignment = true;
    if (M->isMoveAssignmentOperator())
      hasMoveAssignment = true;
  }

  if (hasDefaultConstructor) {
    if (!hasCopyConstructor ||
        !hasMoveConstructor ||
        !hasCopyAssignment ||
        !hasMoveAssignment ||
        !hasDestructor) {
      diag(Loc, "if a default constructor is declared, a copy/move constructor, "
                 "a copy/move assignment operator and a destructor must also be "
                 "provided");
      return;
    }
  }

  if (hasCopyConstructor) {
    if (!hasMoveConstructor ||
        !hasCopyAssignment ||
        !hasMoveAssignment ||
        !hasDestructor) {
      diag(Loc, "if a copy constructor is declared, a move constructor, "
                 "a copy/move assignment operator and a destructor must also be "
                 "provided");
      return;
    }
  }

  if (hasMoveConstructor) {
    if (!hasCopyConstructor ||
        !hasCopyAssignment ||
        !hasMoveAssignment ||
        !hasDestructor) {
      diag(Loc, "if a move constructor is declared, a copy constructor, "
                 "a copy/move assignment operator and a destructor must also be "
                 "provided");
      return;
    }
  }

  if (hasCopyAssignment) {
    if (!hasCopyConstructor ||
        !hasMoveConstructor ||
        !hasMoveAssignment ||
        !hasDestructor) {
      diag(Loc, "if a copy assignment operator is declared, a copy/move constructor, "
                 "a move assignment operator and a destructor must also be "
                 "provided");
      return;
    }
  }

  if (hasMoveAssignment) {
    if (!hasCopyConstructor ||
        !hasMoveConstructor ||
        !hasCopyAssignment ||
        !hasDestructor) {
      diag(Loc, "if a move assignment operator is declared, a copy/move constructor, "
                 "a copy assignment operator and a destructor must also be "
                 "provided");
      return;
    }
  }

  if (hasDestructor) {
    if (!hasCopyConstructor ||
        !hasMoveConstructor ||
        !hasCopyAssignment ||
        !hasMoveAssignment) {
      diag(Loc, "if a destructor is declared, a copy/move constructor "
                 "and a copy/move assignment operator must also be provided");
      return;
    }
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
