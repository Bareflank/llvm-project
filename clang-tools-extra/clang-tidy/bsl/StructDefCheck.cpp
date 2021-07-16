//===--- StructDefCheck.cpp - clang-tidy ----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "StructDefCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

#include <iostream>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXRecordDecl, hasBases) {
  return Node.getNumBases() > 0 || Node.getNumVBases() > 0;
}

void StructDefCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      cxxMethodDecl(
          hasParent(cxxRecordDecl(isStruct())), isUserProvided(),
          anyOf(cxxConstructorDecl(unless(anyOf(isDefaulted(), isDeleted()))),
                cxxDestructorDecl(), isCopyAssignmentOperator(),
                isMoveAssignmentOperator()))
          .bind("special"),
      this);
  Finder->addMatcher(
      cxxMethodDecl(
          hasParent(cxxRecordDecl(isStruct())), isUserProvided(), isVirtual(),
          unless(anyOf(cxxConstructorDecl(), cxxDestructorDecl(),
                       isCopyAssignmentOperator(), isMoveAssignmentOperator())))
          .bind("virtual"),
      this);
  Finder->addMatcher(
      cxxRecordDecl(has(accessSpecDecl(anyOf(isPrivate(), isProtected()))),
                    isStruct())
          .bind("access"),
      this);
  Finder->addMatcher(
      cxxRecordDecl(hasDefinition(), isStruct(), hasBases()).bind("base"),
      this);
  Finder->addMatcher(
      cxxRecordDecl(hasDefinition(), isStruct(), unless(anyOf(isFinal(), isImplicit())))
          .bind("final"),
      this);
}

void StructDefCheck::check(const MatchFinder::MatchResult &Result) {
  auto StructDef = Result.Nodes.getNodeAs<CXXMethodDecl>("special");
  if (StructDef) {
    auto StructLoc = StructDef->getLocation();

    if (StructLoc.isInvalid() || StructLoc.isMacroID())
      return;

    diag(StructLoc,
         "struct should not define special member functions or methods");
  }

  auto StructADef = Result.Nodes.getNodeAs<CXXRecordDecl>("access");
  if (StructADef) {
    auto StructLoc = StructADef->getLocation();

    if (StructLoc.isInvalid() || StructLoc.isMacroID())
      return;

    diag(StructLoc, "struct should contain only public data members");
  }

  auto StructBDef = Result.Nodes.getNodeAs<CXXRecordDecl>("base");
  if (StructBDef) {
    auto StructLoc = StructBDef->getLocation();

    if (StructLoc.isInvalid() || StructLoc.isMacroID())
      return;

    diag(StructLoc, "struct should not inherit from another class or struct");
  }

  auto StructFDef = Result.Nodes.getNodeAs<CXXRecordDecl>("final");
  if (StructFDef) {
    auto StructLoc = StructFDef->getLocation();

    // Ignore prototypes
    if (StructFDef->getDefinition() != StructFDef)
      return;

    if (StructLoc.isInvalid() || StructLoc.isMacroID())
      return;

    FullSourceLoc FullLocation = Result.Context->getFullLoc(StructLoc);
    auto const File = FullLocation.getFileEntry();
    if (nullptr == File)
      return;

    // integral_constant cannot be marked final to work
    auto const filename{File->tryGetRealPathName()};
    if (filename.find(".h") != std::string::npos ||
        filename.find("integral_constant.hpp") != std::string::npos) {
      return;
    }

    diag(StructLoc, "struct must be defined as final; cannot be base of "
                    "another struct or class");
  }

  auto StructVDef = Result.Nodes.getNodeAs<CXXMethodDecl>("virtual");
  if (StructVDef) {
    auto StructLoc = StructVDef->getLocation();

    if (StructLoc.isInvalid() || StructLoc.isMacroID())
      return;

    diag(StructLoc, "struct should not have virtual members");
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
