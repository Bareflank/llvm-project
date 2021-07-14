//===--- OpLogicalPostfixCheck.cpp - clang-tidy ---------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "OpLogicalPostfixCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/Lex/Lexer.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(UnaryOperator, isPostfixIncrement) {
  return Node.isIncrementOp() && Node.isPostfix();
}

AST_MATCHER(UnaryOperator, isPostfixDecrement) {
  return Node.isDecrementOp() && Node.isPostfix();
}

AST_MATCHER(Expr, isDependentScopeDeclRefExpr) {
  return isa<DependentScopeDeclRefExpr>(Node);
}

AST_MATCHER(Expr, isTypeTraitExpr) {
  return isa<TypeTraitExpr>(Node);
}

void OpLogicalPostfixCheck::registerMatchers(MatchFinder *Finder) {
  auto postfixExpr = ignoringImpCasts(anyOf(
    arraySubscriptExpr(),
    callExpr(unless(anyOf(
      cudaKernelCallExpr(),
      cxxOperatorCallExpr(),
      userDefinedLiteral()
    ))),
    cxxBoolLiteral(),
    cxxNullPtrLiteralExpr(),
    cxxConstCastExpr(),
    cxxDynamicCastExpr(),
    cxxReinterpretCastExpr(),
    cxxStaticCastExpr(),
    cxxThisExpr(),
    cxxNoexceptExpr(),
    unaryExprOrTypeTraitExpr(),
    isTypeTraitExpr(),
    declRefExpr(),
    isDependentScopeDeclRefExpr(),
    memberExpr(),
    parenExpr(),
    unaryOperator(anyOf(isPostfixDecrement(), isPostfixIncrement()))
  ));

  Finder->addMatcher(expr(anyOf(
    binaryOperator(
      hasOperatorName("||"),
      hasEitherOperand(expr(unless(anyOf(
        postfixExpr,
        ignoringImpCasts(binaryOperator(hasOperatorName("||")))
      ))).bind("or-nonpostfix"))
    ),
    binaryOperator(
      hasOperatorName("&&"),
      hasEitherOperand(expr(unless(anyOf(
        postfixExpr,
        ignoringImpCasts(binaryOperator(hasOperatorName("&&")))
      ))).bind("and-nonpostfix"))
    )
  )).bind("op"),
  this);
}

void OpLogicalPostfixCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Op = Result.Nodes.getNodeAs<BinaryOperator>("op");
  auto const Loc = Op->getOperatorLoc();

  if (Loc.isInvalid())
    return;

  auto const Mgr = Result.SourceManager;

  if (Op->getOpcodeStr() == "&&") {
    auto AndOperand = Result.Nodes.getNodeAs<Expr>("and-nonpostfix");

    auto Diag =
        diag(Loc, "operands to '&&' must be postfix or '&&' expressions");
    Diag <<
        FixItHint::CreateInsertion(AndOperand->getBeginLoc(), Twine("(").str());

    auto EndLoc = Lexer::getLocForEndOfToken(
        AndOperand->getEndLoc(), 0, *Mgr, Result.Context->getLangOpts());
    Diag << FixItHint::CreateInsertion(EndLoc, Twine(")").str());

    return;
  }

  if (Op->getOpcodeStr() == "||") {
    auto OrOperand = Result.Nodes.getNodeAs<Expr>("or-nonpostfix");

    auto Diag =
        diag(Loc, "operands to '||' must be postfix or '||' expressions");
    Diag <<
        FixItHint::CreateInsertion(OrOperand->getBeginLoc(), Twine("(").str());

    auto EndLoc = Lexer::getLocForEndOfToken(
        OrOperand->getEndLoc(), 0, *Mgr, Result.Context->getLangOpts());
    Diag << FixItHint::CreateInsertion(EndLoc, Twine(")").str());

    return;
  }

  llvm_unreachable("unexpected opcode str");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
