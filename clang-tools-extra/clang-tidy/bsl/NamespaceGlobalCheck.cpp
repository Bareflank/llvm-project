//===--- NamespaceGlobalCheck.cpp - clang-tidy ----------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "NamespaceGlobalCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

AST_MATCHER(LinkageSpecDecl, isExternC) {
    return Node.getLanguage() == LinkageSpecDecl::LanguageIDs::lang_c;
}

void NamespaceGlobalCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
      decl(hasParent(translationUnitDecl()),
           unless(anyOf(
               functionDecl(anyOf(
                   hasName("main"),
                   hasOverloadedOperatorName("new"),
                   hasOverloadedOperatorName("delete"),
                   hasName("operator\"\"_u8"),
                   hasName("operator\"\"_u16"),
                   hasName("operator\"\"_u32"),
                   hasName("operator\"\"_u64"),
                   hasName("operator\"\"_umx"),
                   hasName("operator\"\"_i8"),
                   hasName("operator\"\"_i16"),
                   hasName("operator\"\"_i32"),
                   hasName("operator\"\"_i64"),
                   hasName("operator\"\"_imx"),
                   hasName("operator\"\"_idx")
               )),
               namespaceDecl(),
               linkageSpecDecl(isExternC())
           ))
      ).bind("global-decl"),
      this);
}

void NamespaceGlobalCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *GlobalDecl = Result.Nodes.getNodeAs<Decl>("global-decl");

  auto const Loc = GlobalDecl->getLocation();
  if (Loc.isInvalid())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(Loc);
  auto const File = FullLocation.getFileEntry();
  if (nullptr == File)
    return;

  auto const filename{File->tryGetRealPathName()};
  if (filename.find(".h") != std::string::npos)
    return;

  diag(Loc, "only main, operator new/delete, namespaces, and extern \"C\" "
            "declarations are allowed at global scope");
}

} // namespace bsl
} // namespace tidy
} // namespace clang
