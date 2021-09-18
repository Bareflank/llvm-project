//===--- NamePrefixesCheck.cpp - clang-tidy -------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NamePrefixesCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void NamePrefixesCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    declaratorDecl(
      unless(
        anyOf(
          isImplicit(),
          isExpansionInSystemHeader(),
          hasName("dontcare")
        )
      )
    ).bind("decl"),
    this
  );
}

void NamePrefixesCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *DD = Result.Nodes.getNodeAs<DeclaratorDecl>("decl");
  auto const str{DD->getNameAsString()};
  StringRef const name{str};

  if (auto const *VD = dyn_cast<VarDecl>(DD)) {
    // Ignore constexpr variables
    if (VD->isConstexpr()) {
      return;
    }

    if (VD->isStaticLocal() || VD->isStaticDataMember()) {
      if (!name.startswith("s_")) {
        diag(VD->getLocation(), "static local/member variables must start with 's_'");
      }

      return;
    }

    if (!VD->isLocalVarDeclOrParm()) {
      if (!name.startswith("g_")) {
        diag(VD->getLocation(), "global variables must start with 'g_'");
      }

      return;
    }
  }

  if (auto const *FD = dyn_cast<FieldDecl>(DD)) {
    // Ignore structs
    if (auto const *TD = dyn_cast<TagDecl>(FD->getParent())) {
      if (TD->isStruct())
        return;

      if (TD->isUnion())
        return;
    }

    if (!name.startswith("m_")) {
      diag(FD->getLocation(), "non-static member variables must start with 'm_'");
    }

    return;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
