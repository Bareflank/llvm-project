//===--- ElseRequiredAfterIfCheck.cpp - clang-tidy ------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ElseRequiredAfterIfCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "BslCheckUtils.h"
#include "IsDefinedInATestFile.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void ElseRequiredAfterIfCheck::registerMatchers(MatchFinder *Finder) {
  auto const InterruptsControlFlow =
    stmt(
      anyOf(
        returnStmt(),
        continueStmt(),
        breakStmt(),
        gotoStmt()
      )
    );

  Finder->addMatcher(
    compoundStmt(
      forEach(
        ifStmt(
          hasThen(
            stmt(
              unless(
                anyOf(
                  InterruptsControlFlow,
                  compoundStmt(has(InterruptsControlFlow))
                )
              )
            )
          ),
          unless(hasElse(stmt())),
          unless(isConstexpr())
        ).bind("if_missing_else")
      )
    ),
    this);

  Finder->addMatcher(
    compoundStmt(
      forEach(
        ifStmt(
          hasThen(
            stmt(
              anyOf(
                InterruptsControlFlow,
                compoundStmt(has(InterruptsControlFlow))
              )
            )
          ),
          hasParent(stmt().bind("parent")),
          unless(hasElse(stmt())),
          unless(isConstexpr())
        ).bind("if_missing_else_next_line")
      )
    ),
    this);

  Finder->addMatcher(
    compoundStmt(
      forEach(
        ifStmt(
          hasThen(
            stmt(
              anyOf(
                InterruptsControlFlow,
                compoundStmt(has(InterruptsControlFlow))
              )
            )
          ),
          hasElse(stmt().bind("unneeded_else")),
          unless(isConstexpr())
        )
      )
    ),
    this
  );
}

void ElseRequiredAfterIfCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *IfMissingElse = Result.Nodes.getNodeAs<IfStmt>("if_missing_else");
  auto const *IfMissingElseNextLine = Result.Nodes.getNodeAs<IfStmt>("if_missing_else_next_line");
  auto const *UnneededElse = Result.Nodes.getNodeAs<Stmt>("unneeded_else");

  if (IfMissingElse) {
    auto const Loc = IfMissingElse->getIfLoc();
    if (Loc.isInvalid())
      return;

    if (isDefinedInATestFile(Result.Context, Loc))
      return;

    if (stmtContainsErrors(IfMissingElse, Result))
      return;

    diag(Loc, "'else' is required after 'if'");
  }

  if (IfMissingElseNextLine) {
    auto const Loc = IfMissingElseNextLine->getIfLoc();
    if (Loc.isInvalid())
      return;

    if (isDefinedInATestFile(Result.Context, Loc))
      return;

    if (stmtContainsErrors(IfMissingElseNextLine, Result))
      return;

    auto *Parent = Result.Nodes.getNodeAs<Stmt>("parent");
    if (stmtContainsErrors(Parent, Result))
      return;

    for (auto child = Parent->child_begin(); child != Parent->child_end(); ++child) {
      auto next = child;
      ++next;
      if (next == Parent->child_end() && *child == IfMissingElseNextLine) {
        diag(Loc, "'else' is required after 'if' or add bsl::touch() after `if`");
      }
    }
  }

  if (UnneededElse) {
    auto const Loc = UnneededElse->getBeginLoc();
    if (Loc.isInvalid())
      return;

    if (isDefinedInATestFile(Result.Context, Loc))
      return;

    if (stmtContainsErrors(UnneededElse, Result))
      return;

    diag(Loc, "do not use 'else' after 'return/continue/break'");
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
