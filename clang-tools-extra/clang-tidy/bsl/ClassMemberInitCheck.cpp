//===--- ClassMemberInitCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ClassMemberInitCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(CXXCtorInitializer, isWrittenMemberInitializer) {
  return Node.isWritten() && Node.isMemberInitializer();
}

AST_MATCHER(CXXConstructorDecl, isUserDefinedConcrete) {
  bool IsUserProvided = Node.isUserProvided();
  bool IsDefinition = Node.isThisDeclarationADefinition();

  const CXXRecordDecl *Parent = Node.getParent();
  bool IsTemplateClass = Parent->getDescribedClassTemplate() != nullptr;
  bool IsConcrete = !IsTemplateClass || Node.isTemplateInstantiation();

  return IsUserProvided && IsDefinition && IsConcrete;
}

void ClassMemberInitCheck::checkCtorWithInit(const CXXConstructorDecl *Ctor)
{
  auto const Loc = Ctor->getBeginLoc();
  auto const Parent = Ctor->getParent()->getCanonicalDecl();
  auto NumFields = 0;

  if (nullptr == Parent)
    return;

  for (auto const Field : Parent->fields()) {
    ++NumFields;

    if (!Field->hasInClassInitializer())
      continue;

    diag(Loc, "must use either in-class initializers for all fields"
        " or constructor initializers for all fields");

    auto const Init = Field->getInClassInitializer();
    if (nullptr != Init) {
      diag(Init->getBeginLoc(), "found in-class initializer here",
          DiagnosticIDs::Note);
    }

    return;
  }

  auto NumMemberInits = 0;

  for (auto const I : Ctor->inits()) {
    if (I->isMemberInitializer())
      ++NumMemberInits;
  }

  if (NumMemberInits != NumFields)
    diag(Loc, "member initializer list does not initialize each field");
}

void ClassMemberInitCheck::checkCtorWithoutInit(const CXXConstructorDecl *Ctor)
{
  auto const Parent = Ctor->getParent()->getCanonicalDecl();

  for (auto const Field : Parent->fields()) {
    if (Field->hasInClassInitializer())
      continue;

    diag(Ctor->getBeginLoc(), "must use either in-class initializers for"
        " all fields or constructor initializers for all fields");
  }
}

void ClassMemberInitCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(cxxConstructorDecl(
    isUserDefinedConcrete(),
    hasAnyConstructorInitializer(isWrittenMemberInitializer()),
    unless(anyOf(isCopyConstructor(), isMoveConstructor()))
  ).bind("ctor-init"), this);

  Finder->addMatcher(cxxConstructorDecl(
    isUserDefinedConcrete(),
    unless(anyOf(
      isCopyConstructor(), isMoveConstructor(), isDelegatingConstructor(),
      hasAnyConstructorInitializer(isWrittenMemberInitializer())
    ))
  ).bind("ctor-noinit"), this);
}

void ClassMemberInitCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Init = Result.Nodes.getNodeAs<CXXConstructorDecl>("ctor-init");
  if (Init) {
    checkCtorWithInit(Init);
    return;
  }

  auto const *NoInit = Result.Nodes.getNodeAs<CXXConstructorDecl>("ctor-noinit");
  if (NoInit) {
    checkCtorWithoutInit(NoInit);
    return;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
