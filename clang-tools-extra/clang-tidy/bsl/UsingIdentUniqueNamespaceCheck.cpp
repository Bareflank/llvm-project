//===--- UsingIdentUniqueNamespaceCheck.cpp - clang-tidy ------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "UsingIdentUniqueNamespaceCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

static std::string getNestedNameSpecifierAsString(NamedDecl const *D) {
  std::string name;
  llvm::raw_string_ostream OStream{name};

  D->printNestedNameSpecifier(OStream);
  OStream.flush();

  return name;
}

void UsingIdentUniqueNamespaceCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    namedDecl(
      unless(
        isImplicit()
      )
    ).bind("decl"),
    this
  );
}

void UsingIdentUniqueNamespaceCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *D = Result.Nodes.getNodeAs<NamedDecl>("decl");

  // These are all decl types that we do not need to track. For the template
  // decls, there is a non-template decl as a child that we are watching and
  // the template decl itself is actually a duplicate in the AST.
  if (isa<NamespaceDecl>(D) ||
      isa<VarDecl>(D) ||
      isa<FieldDecl>(D) ||
      isa<CXXConstructorDecl>(D) ||
      isa<CXXDestructorDecl>(D) ||
      isa<VarTemplateDecl>(D) ||
      isa<TemplateTypeParmDecl>(D) ||
      isa<NonTypeTemplateParmDecl>(D) ||
      isa<TemplateTemplateParmDecl>(D) ||
      isa<TypeAliasTemplateDecl>(D) ||
      isa<ClassTemplateDecl>(D) ||
      isa<ClassTemplateSpecializationDecl>(D)) {
    return;
  }

  // Ignore operator overloads
  if (auto const *FD{dyn_cast<FunctionDecl>(D)}) {
    if (FD->isOverloadedOperator())
      return;
  }

  // Ignore all constructors, including generic (i.e., template) constructors
  if (auto const *FTD{dyn_cast<FunctionTemplateDecl>(D)}) {
    if (isa<CXXConstructorDecl>(FTD->getTemplatedDecl()))
      return;
  }

  if (D->getLocation().isInvalid())
    return;

  // If the decl is "bsl::array", the name is "array" and the specifier is
  // "bsl::". Sadly, LLVM does not give us a function for the spec, so we
  // hacked one together using the print function.
  auto name{D->getNameAsString()};
  auto spec{getNestedNameSpecifierAsString(D)};

  // Ignore empty/reserved names
  if (name.empty() || name[0] == '_')
    return;

  const size_t is_nullptr = name.find("nullptr_t");
  if (std::string::npos != is_nullptr)
    return;

  const size_t is_max_align = name.find("max_align_t");
  if (std::string::npos != is_max_align)
    return;

  auto iter{m_ids.find(name)};
  if (iter != m_ids.end()) {
    auto &recordList{iter->second};
    for (auto const &record : recordList) {

      // If the specifiers are the same for both, we can ignore this case as
      // the compiler will make sure the names are overloads. Otherwise, you
      // would get a compile error. We only care about the case where the
      // specifiers are different, but a subset of each other. For example,
      // "bsl::array" and "bsl::details::array".
      if (spec == record.spec) {
        continue;
      }

      if (spec.find(record.spec) != std::string::npos) {
        diag(D->getLocation(), "A user-defined type name shall be a unique identifier within a namespace");
        diag(record.D->getLocation(), "previous user-defined with the same name found here", DiagnosticIDs::Note);
        return;
      }

      if (record.spec.find(spec) != std::string::npos) {
        diag(D->getLocation(), "A user-defined type name shall be a unique identifier within a namespace");
        diag(record.D->getLocation(), "previous user-defined with the same name found here", DiagnosticIDs::Note);
        return;
      }
    }
  }

  m_ids[name].push_back({spec, D});
}

} // namespace bsl
} // namespace tidy
} // namespace clang
