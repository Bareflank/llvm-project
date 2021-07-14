//===--- ClassMemberRedefinedCheck.cpp - clang-tidy -----------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClassMemberRedefinedCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXRecordDecl, hasBases) {
  return Node.getNumBases() > 0 || Node.getNumVBases() > 0;
}

AST_MATCHER(CXXMethodDecl, isConstructor) {
  return isa<CXXConstructorDecl>(&Node);
}

AST_MATCHER(CXXMethodDecl, isDestructor) {
  return isa<CXXDestructorDecl>(&Node);
}

void ClassMemberRedefinedCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(decl(
    anyOf(
        varDecl(),
        fieldDecl(),
        cxxMethodDecl(unless(anyOf(
            isOverride(), isConstructor(), isDestructor(),
            isCopyAssignmentOperator(), isMoveAssignmentOperator()
        )))
    ),
    hasParent(cxxRecordDecl(hasDefinition(), hasBases()))).bind("decl"),
    this);
}

bool ClassMemberRedefinedCheck::hasNoMemberWithName(const CXXRecordDecl *Base,
                                                    StringRef MemberName)
{
  Base = Base->getCanonicalDecl();

  for (auto const &D : Base->decls()) {
    auto const ND = dyn_cast<NamedDecl>(D);
    if (!ND)
      continue;

    // Only warn on public and protected decls
    if (ND->getAccess() >= AccessSpecifier::AS_private)
      continue;

    if (!ND->getName().equals(MemberName))
      continue;

    FoundHidden = true;
    HiddenMemberLoc = ND->getBeginLoc();
    HiddenMemberClassName = Base->getName();

    return false;
  }

  if (Base->getNumBases() == 0 && Base->getNumVBases() == 0)
     return true;

  return Base->forallBases([this, MemberName](const CXXRecordDecl *B) {
    return hasNoMemberWithName(B, MemberName);
  });
}

void ClassMemberRedefinedCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *ND = Result.Nodes.getNodeAs<NamedDecl>("decl");
  if (!ND)
    return;

  auto const Loc = ND->getBeginLoc();
  if (Loc.isInvalid())
    return;

  auto const Parent = dyn_cast<CXXRecordDecl>(ND->getDeclContext());
  if (!Parent)
    return;

  auto const Name = ND->getName();

  Parent->forallBases([this, Name](const CXXRecordDecl *B) {
    return hasNoMemberWithName(B, Name);
  });

  if (FoundHidden) {
    diag(Loc, "member '%0' hides member with the same name in base class %1") <<
        Name << HiddenMemberClassName;
    diag(HiddenMemberLoc, "member '%0' of base class %1 found here",
        DiagnosticIDs::Note) << Name << HiddenMemberClassName;

    FoundHidden = false;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
