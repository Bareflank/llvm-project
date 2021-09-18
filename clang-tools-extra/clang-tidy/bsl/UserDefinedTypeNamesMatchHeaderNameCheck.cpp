//===--- UserDefinedTypeNamesMatchHeaderNameCheck.cpp - clang-tidy --------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UserDefinedTypeNamesMatchHeaderNameCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

void UserDefinedTypeNamesMatchHeaderNameCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    namedDecl(
      unless(
        isImplicit()
      )
    ).bind("decl"),
    this
  );
}

void UserDefinedTypeNamesMatchHeaderNameCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *D = Result.Nodes.getNodeAs<NamedDecl>("decl");

  if (!D->getBeginLoc().isValid())
    return;

  FullSourceLoc FullLocation = Result.Context->getFullLoc(D->getBeginLoc());

  auto const File = FullLocation.getFileEntry();
  if (!File)
    return;

  if (isa<NamespaceDecl>(D) ||
      isa<FunctionDecl>(D) ||
      isa<FunctionTemplateDecl>(D) ||
      isa<TypedefDecl>(D) ||
      isa<TypeAliasDecl>(D) ||
      isa<VarDecl>(D) ||
      isa<VarTemplateDecl>(D) ||
      isa<FieldDecl>(D) ||
      isa<EnumConstantDecl>(D) ||
      isa<TemplateTypeParmDecl>(D) ||
      isa<NonTypeTemplateParmDecl>(D) ||
      isa<TemplateTemplateParmDecl>(D) ||
      isa<TypeAliasTemplateDecl>(D) ||
      isa<ClassTemplateDecl>(D) ||
      isa<ClassTemplateSpecializationDecl>(D)) {
    return;
  }

  if (D->getParentFunctionOrMethod())
    return;

  if (auto const *DC = D->getDeclContext()) {
    if (isa<RecordDecl>(DC))
      return;
  }

  if (auto const *RD{dyn_cast<RecordDecl>(D)}) {
    if (RD->getDefinition() != RD)
      return;
  }

  std::string name{D->getNameAsString()};
  std::string filename{File->tryGetRealPathName()};

  const size_t last_slash_idx = filename.find_last_of("\\/");
  if (std::string::npos != last_slash_idx)
  {
      filename.erase(0, last_slash_idx + 1);
  }

  const size_t period_idx = filename.rfind('.');
  if (std::string::npos != period_idx)
  {
      if (filename.substr(period_idx) != ".hpp" &&
          filename.substr(period_idx) != ".h") {
        diag(D->getBeginLoc(), "All declarations must be made in header files that end in .hpp/.h");
        return;
      }

      filename.erase(period_idx);
  }
  else {
    diag(D->getBeginLoc(), "All source files must have an extension");
    return;
  }

  if (filename == "cstdint" ||
      filename == "safe_integral") {
    return;
  }

  if (name == filename)
    return;

  diag(D->getBeginLoc(), "User-defined types must have the same name as the header file they are "
                         "defined in. Either name the %0 '%1', or name the header '%2'")
                         << D->getDeclKindName() << filename << name;
}

} // namespace bsl
} // namespace tidy
} // namespace clang
