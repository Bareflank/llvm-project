//===--- UsingDeclScopeCheck.cpp - clang-tidy -----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UsingDeclScopeCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

UsingDeclScopeCheck::UsingDeclScopeCheck(StringRef Name,
                                         ClangTidyContext *Context)
    : ClangTidyCheck(Name, Context),
      RawStringHeaderFileExtensions(Options.getLocalOrGlobal(
          "HeaderFileExtensions", utils::defaultHeaderFileExtensions())) {
  if (!utils::parseFileExtensions(RawStringHeaderFileExtensions,
                                  HeaderFileExtensions,
                                  utils::defaultFileExtensionDelimiters())) {
    llvm::errs() << "Invalid header file extension: "
                 << RawStringHeaderFileExtensions << "\n";
  }
}

void UsingDeclScopeCheck::storeOptions(
    ClangTidyOptions::OptionMap &Opts) {
  Options.store(Opts, "HeaderFileExtensions", RawStringHeaderFileExtensions);
}

// This match is slightly different than google-global-names-in-headers
// because it only matches using declarations (using directives are forbidden
// by google-build-using-namespace), and it matches them in any scope that is
// not either class or function.
void UsingDeclScopeCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(usingDecl(unless(anyOf(
                        hasDeclContext(functionDecl()),
                        hasDeclContext(cxxRecordDecl())))).bind("decl"),
                     this);
}

void UsingDeclScopeCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Using = Result.Nodes.getNodeAs<UsingDecl>("decl");

  auto const Loc = Using->getBeginLoc();
  if (Loc.isInvalid())
    return;

  auto const Mgr = Result.SourceManager;
  if (!utils::isExpansionLocInHeaderFile(Loc, *Mgr, HeaderFileExtensions)) {
    return;
  }

  diag(Loc, "using declaration in header file is not in class or function");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
