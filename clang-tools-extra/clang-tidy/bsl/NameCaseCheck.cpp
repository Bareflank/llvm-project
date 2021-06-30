//===--- NameCaseCheck.cpp - clang-tidy -----------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NameCaseCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

bool isLowerCase(std::string const &str)
{
  std::string tmp{str};
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::tolower);

  return str == tmp;
}

bool isUpperCase(std::string const &str)
{
  std::string tmp{str};
  std::transform(tmp.begin(), tmp.end(), tmp.begin(), ::toupper);

  return str == tmp;
}

void NameCaseCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    namedDecl(
      unless(
        anyOf(
          isImplicit(),
          isExpansionInSystemHeader()
        )
      )
    ).bind("decl"),
    this
  );
}

void NameCaseCheck::check(const MatchFinder::MatchResult &Result) {
  const auto *ND = Result.Nodes.getNodeAs<NamedDecl>("decl");
  if (nullptr == ND) {
    return;
  }

  auto const name{ND->getNameAsString()};
  if (name.empty()) {
    return;
  }

  const auto Loc = ND->getLocation();
  if (Loc.isInvalid())
    return;

  if (isa<FunctionTemplateDecl>(ND) ||
      isa<CXXConstructorDecl>(ND) ||
      isa<CXXDestructorDecl>(ND) ||
      isa<ClassTemplateDecl>(ND) ||
      isa<ClassTemplateSpecializationDecl>(ND)) {
    return;
  }

  if (isa<TemplateTypeParmDecl>(ND) ||
      isa<NonTypeTemplateParmDecl>(ND) ||
      isa<TemplateTemplateParmDecl>(ND)) {
    if (!isUpperCase(name)) {
      diag(Loc, "name of template variable is not in upper case");
    }

    return;
  }

  if (auto const * VD = dyn_cast<VarDecl>(ND)) {
    if (VD->hasGlobalStorage() && VD->isConstexpr()) {
      if (!VD->isStaticLocal() && !VD->isStaticDataMember()) {
        if (!isUpperCase(name)) {
          diag(Loc, "name of global constexpr is not in upper case");
        }

        return;
      }
    }
  }

  if (!isLowerCase(name)) {
    diag(Loc, "name of variable is not in lower case");
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
