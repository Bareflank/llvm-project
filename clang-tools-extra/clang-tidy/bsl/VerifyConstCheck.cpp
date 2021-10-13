//===--- VerifyConstCheck.cpp - clang-tidy --------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "VerifyConstCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "BslCheckUtils.h"

#include <iostream>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void VerifyConstCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    varDecl(
      unless(
        anyOf(
          isImplicit(),
          hasName("dontcare")
        )
      )
    ).bind("var-decl"),
  this);

  Finder->addMatcher(
    fieldDecl(
      unless(
        isImplicit()
      )
    ).bind("field-decl"),
  this);
}

StringRef getRawTokenIdentifierStr(SourceLocation Loc,
                         const MatchFinder::MatchResult &Result) {
  if (Loc.isInvalid() || Loc.isMacroID())
    return StringRef();

  auto Mgr = Result.SourceManager;
  auto Ctx = Result.Context;

  Token Tok;
  if (Lexer::getRawToken(Loc, Tok, *Mgr, Ctx->getLangOpts(), false))
    return StringRef();

  return Tok.getRawIdentifier();
}

void VerifyConstCheck::check_var_decl(const MatchFinder::MatchResult &Result) {
  auto const VD = Result.Nodes.getNodeAs<VarDecl>("var-decl");
  if (nullptr == VD)
    return;

  if (VD->isInvalidDecl())
    return;

  if (VD->isConstexpr())
    return;

  auto name = VD->getName();
  if (name.empty())
    return;

  if (VD->isStaticLocal() || VD->isStaticDataMember()) {
    if (name.startswith("s_"))
      name = name.drop_front(2);
  }

  if (!VD->isLocalVarDeclOrParm()) {
    if (name.startswith("g_"))
      name = name.drop_front(2);
  }

  auto const Loc = VD->getLocation();
  if (Loc.isInvalid())
    return;

  /// NOTE:
  /// - There are a couple of different types of variables that we have to
  ///   worry about, and it is important that we are explicit about what is
  ///   const and what is not.
  ///   - int var
  ///   - int *var
  ///
  /// - Like Rust, all variables must be marked const unless they are
  ///   explicitly marked as mutable. To mark mutability, you must name
  ///   your variables with the following suffixes:
  ///   - <none> or cst_
  ///   - mut_
  ///   - udm_
  ///
  /// - To label a variable as const, you don't need to add any suffix. Again,
  ///   by default, all variables must be const, so just mark the variable
  ///   as const using "const" in east const form (on the right side of the
  ///   type) and you are good. If a variable needs to be mutable, add the
  ///   mut_ suffix. If a variable can be either (only needed for template
  ///   types), use the "undefined mutability" suffix of udm_.
  ///
  /// - For the pointer type, we have to worry about who is mutable. LLVM
  ///   tracks the type (the variable itself) and the "pointee". To handle
  ///   the pointee, add a second suffix with a "p" in front for pointee.
  ///
  /// - For global and static variables, the bsl-name-prefix check makes sure
  ///   that all variables start with g_ and s_. In these cases, we remove the
  ///   g_ and s_ and then perform the check so that the prefix check still
  ///   works. So for example, a mutable global variable would be g_mut_var.
  ///
  /// - So, in a nutshell, a variable's name can take on the form of
  ///   <pointee mutability>_<variable mutability>_<variable name>,
  ///   which gives us the following configuration (with none meaning const):
  ///   - <    >_<   >_var
  ///   - <xxxx>_<cst>_var
  ///   - <xxxx>_<mut>_var
  ///   - <xxxx>_<udm>_var
  ///   - <    >_<xxx>_var
  ///   - <pcst>_<xxx>_var
  ///   - <pmut>_<xxx>_var
  ///   - <pudm>_<xxx>_var
  ///
  /// - Of these configurations, we can have the following combinations with
  ///   the allowed defintions in the comment
  ///   -              var: const var
  ///   -        <cst>_var: const var
  ///   -        <mut>_var: var
  ///   -        <udm>_var: const var, var
  ///   -       <pcst>_var: const *const var
  ///   -       <pmut>_var: *const var
  ///   -       <pudm>_var: const *const var, *const var
  ///   - <pcst>_<cst>_var: const *const var
  ///   - <pcst>_<mut>_var: const *var
  ///   - <pcst>_<udm>_var: const *const var, const *var
  ///   - <pmut>_<cst>_var: *const var
  ///   - <pmut>_<mut>_var: *var
  ///   - <pmut>_<udm>_var: *const var, *var
  ///   - <pudm>_<cst>_var: const *const var, *const var
  ///   - <pudm>_<mut>_var: const *var, *var
  ///   - <pudm>_<udm>_var: all types
  ///
  /// - Quick note on the last one. It can be used to accept any type, which
  ///   includes both pointer types and non-pointer types. It is a catch all
  ///   that basically says, I don't care about what you pass me. This should
  ///   only be used by the BSL, or for perfect forwarding.
  ///
  /// - The following tests the non-pointer types:
  ///
  ///   { bool const i{} };             // correct
  ///   { bool const cst_i{} };         // correct
  ///   { bool mut_i{} };               // correct
  ///   { bool udm_i{} };               // correct
  ///   { bool const udm_i{} };         // correct
  ///   { bool pudm_udm_i{} };          // correct
  ///   { bool const pudm_udm_i{} };    // correct
  ///
  ///   { bool i{} };                   // error
  ///   { bool cst_i{} };               // error
  ///   { bool const mut_i{} };         // error
  ///   { bool pcst_cst_i{} };          // error
  ///   { bool const pcst_cst_i{} };    // error
  ///   { bool pcst_mut_i{} };          // error
  ///   { bool const pcst_mut_i{} };    // error
  ///   { bool pcst_udm_i{} };          // error
  ///   { bool const pcst_udm_i{} };    // error
  ///   { bool pmut_cst_i{} };          // error
  ///   { bool const pmut_cst_i{} };    // error
  ///   { bool pmut_mut_i{} };          // error
  ///   { bool const pmut_mut_i{} };    // error
  ///   { bool pmut_udm_i{} };          // error
  ///   { bool const pmut_udm_i{} };    // error
  ///   { bool pudm_cst_i{} };          // error
  ///   { bool const pudm_cst_i{} };    // error
  ///   { bool pudm_mut_i{} };          // error
  ///   { bool const pudm_mut_i{} };    // error
  ///
  /// - The following tests the pointer types:
  ///
  ///   { bool const *const i{}; }             // correct
  ///   { bool const *const cst_i{}; }         // correct
  ///   { bool const *mut_i{}; }               // correct
  ///   { bool const *udm_i{}; }               // correct
  ///   { bool const *const udm_i{}; }         // correct
  ///   { bool const *const pcst_cst_i{}; }    // correct
  ///   { bool const *pcst_mut_i{}; }          // correct
  ///   { bool const *pcst_udm_i{}; }          // correct
  ///   { bool const *const pcst_udm_i{}; }    // correct
  ///   { bool *const pmut_cst_i{}; }          // correct
  ///   { bool *pmut_mut_i{}; }                // correct
  ///   { bool *pmut_udm_i{}; }                // correct
  ///   { bool *const pmut_udm_i{}; }          // correct
  ///   { bool *const pudm_cst_i{}; }          // correct
  ///   { bool const *const pudm_cst_i{}; }    // correct
  ///   { bool *pudm_mut_i{}; }                // correct
  ///   { bool const *pudm_mut_i{}; }          // correct
  ///   { bool *pudm_udm_i{}; }                // correct
  ///   { bool *const pudm_udm_i{}; }          // correct
  ///   { bool const *pudm_udm_i{}; }          // correct
  ///   { bool const *const pudm_udm_i{}; }    // correct
  ///
  ///   { bool *i{}; }                         // error
  ///   { bool *const i{}; }                   // error
  ///   { bool const *i{}; }                   // error
  ///   { bool *cst_i{}; }                     // error
  ///   { bool *const cst_i{}; }               // error
  ///   { bool const *cst_i{}; }               // error
  ///   { bool *mut_i{}; }                     // error
  ///   { bool *const mut_i{}; }               // error
  ///   { bool const *const mut_i{}; }         // error
  ///   { bool *udm_i{}; }                     // error
  ///   { bool *const udm_i{}; }               // error
  ///   { bool *pcst_cst_i{}; }                // error
  ///   { bool *const pcst_cst_i{}; }          // error
  ///   { bool const *pcst_cst_i{}; }          // error
  ///   { bool *pcst_mut_i{}; }                // error
  ///   { bool *const pcst_mut_i{}; }          // error
  ///   { bool const *const pcst_mut_i{}; }    // error
  ///   { bool *pcst_udm_i{}; }                // error
  ///   { bool *const pcst_udm_i{}; }          // error
  ///   { bool *pmut_cst_i{}; }                // error
  ///   { bool const *pmut_cst_i{}; }          // error
  ///   { bool const *const pmut_cst_i{}; }    // error
  ///   { bool *const pmut_mut_i{}; }          // error
  ///   { bool const *pmut_mut_i{}; }          // error
  ///   { bool const *const pmut_mut_i{}; }    // error
  ///   { bool const *pmut_udm_i{}; }          // error
  ///   { bool const *const pmut_udm_i{}; }    // error
  ///   { bool *pudm_cst_i{}; }                // error
  ///   { bool const *pudm_cst_i{}; }          // error
  ///   { bool *const pudm_mut_i{}; }          // error
  ///   { bool const *const pudm_mut_i{}; }    // error
  ///

  auto const QT = VD->getType().getNonReferenceType();
  if (QT->isDependentType())
    return;

  if (QT->isPointerType()) {
    if (getRawTokenIdentifierStr(VD->getBeginLoc(), Result) == "const") {
      diag(Loc, "the const qualifier for the pointee to the pointer type %0 for variable %1 must be on the right of the type, not the left") << QT << VD;
      return;
    }
  }
  else {
    if (getRawTokenIdentifierStr(VD->getBeginLoc(), Result) == "const") {
      diag(Loc, "the const qualifier for variable %0 must be on the right of the type, not the left") << VD;
      return;
    }
  }

  if (QT->isPointerType()) {
    if (QT->getPointeeType().isConstQualified()) {
      if (name.startswith("pmut_")) {
          diag(Loc, "the pointee to the pointer type %0 for variable %1 cannot be marked as const or the variable's name must start with "
                    "cst_, mut_, udm_, pcst_cst_, pcst_mut_, pcst_udm_, pudm_cst_, pudm_mut_ or pudm_udm_") << QT << VD;
        return;
      }
    }
    else {
      if (!name.startswith("pmut_") &&
          !name.startswith("pudm_")) {
        diag(Loc, "the pointee to the pointer type %0 for variable %1 must be marked as const or the variable's name must start with "
                    "pmut_cst_, pmut_mut_, pmut_udm_, pudm_cst_, pudm_mut_ or pudm_udm_") << QT << VD;
        return;
      }
    }

    if (QT.isConstQualified()) {
      if (name.startswith("mut_") ||
          name.startswith("pcst_mut_") ||
          name.startswith("pmut_mut_") ||
          name.startswith("pudm_mut_")) {
        diag(Loc, "the variable %0 of type %1 canont be marked as const or the variable's name must start with "
                    "cst_, udm_, pcst_cst_, pcst_udm_, pmut_cst_, pmut_udm_, pudm_cst_ or pudm_udm_") << VD << QT;
        return;
      }
    }
    else {
      if (!name.startswith("mut_") &&
          !name.startswith("udm_") &&
          !name.startswith("pcst_mut_") &&
          !name.startswith("pcst_udm_") &&
          !name.startswith("pmut_mut_") &&
          !name.startswith("pmut_udm_") &&
          !name.startswith("pudm_mut_") &&
          !name.startswith("pudm_udm_")) {
        diag(Loc, "the variable %0 of type %1 must be marked as const or the variable's name must start with "
                    "mut_, udm_, pcst_mut_, pcst_udm_, pmut_mut_, pmut_udm_, pudm_mut_ or pudm_udm_") << VD << QT;
        return;
      }
    }
  }
  else {
    if (name.startswith("pcst_cst_") ||
        name.startswith("pcst_mut_") ||
        name.startswith("pcst_udm_") ||
        name.startswith("pmut_cst_") ||
        name.startswith("pmut_mut_") ||
        name.startswith("pmut_udm_") ||
        name.startswith("pudm_cst_") ||
        name.startswith("pudm_mut_")) {
      diag(Loc, "the variable %0 has a pointer-only suffix which is not allowed for non-pointer types") << VD;
      return;
    }

    if (QT.isConstQualified()) {
      if (name.startswith("mut_")) {
        diag(Loc, "the variable %0 of type %1 canont be marked as const if it starts with mut_") << VD << QT;
        return;
      }
    }
    else {
      if (!name.startswith("mut_") && !name.startswith("udm_") && !name.startswith("pudm_udm_")) {
        diag(Loc, "the variable %0 of type %1 must be marked as const or start with mut_, udm_ or pudm_udm_") << VD << QT;
        return;
      }
    }
  }
}

void VerifyConstCheck::check_field_decl(const MatchFinder::MatchResult &Result) {
  auto const FD = Result.Nodes.getNodeAs<FieldDecl>("field-decl");
  if (nullptr == FD)
    return;

  if (FD->isInvalidDecl())
    return;

  auto const Loc = FD->getLocation();
  if (Loc.isInvalid())
    return;

  if (FD->getName().empty())
    return;

  auto const QT = FD->getType().getNonReferenceType();
  if (QT.isConstQualified()) {
    if (getRawTokenIdentifierStr(FD->getBeginLoc(), Result) != "const")
      return;

    diag(Loc, "the const qualifier for variable %0 must be on the right of the type, not the left") << FD;
  }
}

void VerifyConstCheck::check(const MatchFinder::MatchResult &Result) {
  check_var_decl(Result);
  check_field_decl(Result);
}

} // namespace bsl
} // namespace tidy
} // namespace clang
