//===--- ClassBaseCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClassBaseCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXRecordDecl, hasAtLeastTwoBases) {
  return Node.getNumBases() >= 2;
}

bool ClassBaseCheck::isInterfaceMethod(const CXXMethodDecl *MD)
{
  if (MD->isCopyAssignmentOperator())
    return true;

  if (MD->isMoveAssignmentOperator())
    return true;

  if (isa<CXXConstructorDecl>(MD))
    return true;

  if (isa<CXXDestructorDecl>(MD))
    return MD->isVirtual();

  return MD->getAccess() == AccessSpecifier::AS_public &&
         MD->isPure() &&
         MD->isVirtual();
}

bool ClassBaseCheck::isInterfaceVar(const VarDecl *VD)
{
  return VD->getAccess() == AccessSpecifier::AS_public &&
         VD->isConstexpr() &&
         VD->isStaticDataMember();
}

bool ClassBaseCheck::isInterfaceClass(const CXXRecordDecl *Base)
{
  Base = Base->getCanonicalDecl();

  if (Base->hasAnyDependentBases())
    return false;

  if (!Base->isAbstract())
    return false;

  if (Base->hasDirectFields())
    return false;

  for (auto const &D : Base->decls()) {
    auto const M = dyn_cast<CXXMethodDecl>(D);
    if (M) {
      if (!isInterfaceMethod(M))
        return false;
    }

    auto const V = dyn_cast<VarDecl>(D);
    if (V) {
      if (!isInterfaceVar(V))
        return false;
    }
  }

  if (Base->getNumBases() == 0)
    return true;

  return Base->forallBases([this](const CXXRecordDecl *B) -> bool {
    return isInterfaceClass(B);
  });
}

void ClassBaseCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    cxxRecordDecl(
      unless(isLambda()),
      hasDefinition(),
      hasAtLeastTwoBases()).bind("record"),
    this);
}

void ClassBaseCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *RD = Result.Nodes.getNodeAs<CXXRecordDecl>("record");

  auto const Loc = RD->getBeginLoc();
  if (Loc.isInvalid())
    return;

  InterfaceBases = 0;

  RD->forallBases([this] (const CXXRecordDecl *Base) -> bool {
    if (isInterfaceClass(Base))
      InterfaceBases++;

    return true;
  });

  if (InterfaceBases >= RD->getNumBases() - 1)
    return;

  diag(Loc, "class %0 has %1 interface base classes; at least %2 required")
        << RD->getName() << InterfaceBases << (RD->getNumBases() - 1);

  diag(Loc, "a class is an interface if"
      " (1) it has a virtual destructor,"
      " (2) it has at least one public pure virtual method,"
      " (3) its non-special methods are public pure virtual,"
      " (4) it has no dependent bases,"
      " (5) its base classes (if any) are interfaces, and"
      " (6) its data members (if any) are public static constexpr",
      DiagnosticIDs::Note);
}

} // namespace bsl
} // namespace tidy
} // namespace clang
