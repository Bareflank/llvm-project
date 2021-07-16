//===--- IdentifierTypographicallyUnambiguousCheck.cpp - clang-tidy -------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IdentifierTypographicallyUnambiguousCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include <string>

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

static std::string getNestedNameSpecifierAsString(
  NamedDecl const *D)
{
  std::string name;
  llvm::raw_string_ostream OStream{name};

  if (D == nullptr) {
    return {};
  }

  D->printNestedNameSpecifier(OStream);
  OStream.flush();

  return name;
}

std::string removeAmbiguity(
  std::string const &in)
{
  std::string out;
  out.reserve(in.length());

  for (size_t i{}; i < in.length(); ++i) {
    if (in[i] == '_')
      continue;

    switch(in[i]) {
      case '0':
        out += 'o';
        continue;

      case '1':
        out += 'i';
        continue;

      case 'l':
        out += 'i';
        continue;

      case 'L':
        out += 'i';
        continue;

      case '5':
        out += 's';
        continue;

      case '2':
        out += 'z';
        continue;

      case 'h':
        out += 'n';
        continue;

      case 'H':
        out += 'n';
        continue;

      case '8':
        out += 'b';
        continue;

      default:
        break;
    }

    if (i < in.length() - 1) {
      if (in[i] == 'r' && in[i + 1] == 'n') {
        out += 'm';
        ++i;
        continue;
      }

      if (in[i] == 'R' && in[i + 1] == 'n') {
        out += 'm';
        ++i;
        continue;
      }

      if (in[i] == 'r' && in[i + 1] == 'N') {
        out += 'm';
        ++i;
        continue;
      }

      if (in[i] == 'R' && in[i + 1] == 'N') {
        out += 'm';
        ++i;
        continue;
      }
    }

    out += in[i];
  }

  std::transform(out.begin(), out.end(), out.begin(), ::tolower);
  return out;
}

template<typename T>
bool bothAre(
  NamedDecl const * const arg1, NamedDecl const * const arg2)
{
  if (isa<T>(arg1)) {
    if (isa<T>(arg2)) {
      return true;
    }
  }

  return false;
}

template<typename T1, typename T2>
bool bothAre(
  NamedDecl const * const arg1, NamedDecl const * const arg2)
{
  if (isa<T1>(arg1) && isa<T2>(arg2)) {
    return true;
  }

  if (isa<T2>(arg1) && isa<T1>(arg2)) {
    return true;
  }

  return false;
}

bool hasDifferentNamespaces(
  NamedDecl const * const arg1, NamedDecl const * const arg2)
{
  return getNestedNameSpecifierAsString(arg1) != getNestedNameSpecifierAsString(arg2);
}

bool hasSameQualifiedName(
  NamedDecl const * const arg1, NamedDecl const * const arg2)
{
  return arg1->getQualifiedNameAsString() == arg2->getQualifiedNameAsString();
}

bool areContainedInDifferentNamespaces(
  NamedDecl const * const arg1, NamedDecl const * const arg2)
{
  auto const NS1{getNestedNameSpecifierAsString(arg1)};
  auto const NS2{getNestedNameSpecifierAsString(arg2)};

  if (NS1.empty() || NS2.empty())
    return false;

  return NS1 != NS2;
}

Decl const *findParentDecl(
  NamedDecl const * const arg)
{
  if (auto const * DC{arg->getDeclContext()}) {
    if (auto const *CXXRD{dyn_cast<CXXRecordDecl>(DC)})
      return dyn_cast<Decl>(CXXRD);

    if (auto const *CXXMD{dyn_cast<CXXMethodDecl>(DC)})
      return dyn_cast<Decl>(CXXMD->getParent());
  }

  return nullptr;
}

bool areContainedInDifferentBlocks(
  NamedDecl const * const arg1, NamedDecl const * const arg2)
{
  auto const * const PoM1{arg1->getParentFunctionOrMethod()};
  auto const * const PoM2{arg2->getParentFunctionOrMethod()};

  if (PoM1 != nullptr && PoM2 != nullptr) {
    return PoM1 != PoM2;
  }

  Decl const *D1{findParentDecl(arg1)};
  Decl const *D2{findParentDecl(arg2)};

  if (D1 == nullptr && D2 == nullptr) {
    if (PoM1 != nullptr && PoM2 == nullptr)
      return areContainedInDifferentNamespaces(dyn_cast<NamedDecl>(PoM1), arg2);

    if (PoM1 == nullptr && PoM2 != nullptr)
      return areContainedInDifferentNamespaces(arg1, dyn_cast<NamedDecl>(PoM2));

    if (PoM1 == nullptr && PoM2 == nullptr)
      return areContainedInDifferentNamespaces(arg1, arg2);
  }

  if (D1 != nullptr && D2 != nullptr) {
    return D1 != D2;
  }

  if (D1 == nullptr && D2 != nullptr) {
    if (PoM1 == nullptr)
      return hasDifferentNamespaces(arg1, dyn_cast<NamedDecl>(D2));

    if (PoM1 != nullptr)
      return true;
  }

  if (D1 != nullptr && D2 == nullptr) {
    if (PoM2 == nullptr) {
      return hasDifferentNamespaces(dyn_cast<NamedDecl>(D1), arg2);
    }

    if (PoM2 != nullptr)
      return true;
  }

  // Should not be reachable.
  return false;
}

bool areTheSameVarDecls(
  NamedDecl const * const arg1, NamedDecl const * const arg2)
{
  auto const * const VD1{dyn_cast<VarDecl>(arg1)};
  auto const * const VD2{dyn_cast<VarDecl>(arg2)};

  if (VD1 == nullptr || VD2 == nullptr) {
    return false;
  }

  auto const * const PoM1{VD1->getParentFunctionOrMethod()};
  auto const * const PoM2{VD2->getParentFunctionOrMethod()};

  if (PoM1 == PoM2) {
    return hasSameQualifiedName(arg1, arg2);
  }

  return false;
}

void IdentifierTypographicallyUnambiguousCheck::registerMatchers(MatchFinder *Finder)
{
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

void IdentifierTypographicallyUnambiguousCheck::check(const MatchFinder::MatchResult &Result)
{
  auto const *D = Result.Nodes.getNodeAs<NamedDecl>("decl");

  if (isa<FieldDecl>(D) ||
      isa<NamespaceDecl>(D) ||
      isa<UsingDirectiveDecl>(D) ||
      isa<CXXConstructorDecl>(D) ||
      isa<CXXDestructorDecl>(D) ||
      isa<CXXDeductionGuideDecl>(D) ||
      isa<VarTemplateDecl>(D) ||
      isa<FunctionTemplateDecl>(D) ||
      isa<TypeAliasTemplateDecl>(D) ||
      isa<ClassTemplateDecl>(D) ||
      isa<ClassTemplateSpecializationDecl>(D)) {
    return;
  }

  if (auto const *DC{D->getDeclContext()}) {
    // Ignore deduction guides (and parents of)
    if (isa<CXXDeductionGuideDecl>(DC))
      return;

    // Ignore specializations (and parents of)
    if (isa<ClassTemplateSpecializationDecl>(DC))
      return;
  }

  if (auto const *FD{dyn_cast<FunctionDecl>(D)}) {
    // Ignore operator overloads
    if (FD->isOverloadedOperator())
      return;

    // Ignore prototypes
    if (FD->getDefinition() != FD)
      return;

    // Ignore Specializations
    if (FD->getMemberSpecializationInfo())
      return;

    // Ignore Specializations
    if (FD->getTemplateSpecializationInfo())
      return;
  }

  if (auto const *CXXRD{dyn_cast<CXXRecordDecl>(D)}) {
    // Ignore member classes
    if (CXXRD->getInstantiatedFromMemberClass())
      return;

    // Ignore prototypes
    if (CXXRD->getDefinition() != CXXRD)
      return;
  }

  if (auto const *RD{dyn_cast<RecordDecl>(D)}) {
    // Ignore prototypes
    if (RD->getDefinition() != RD)
      return;
  }

  // Ignore template types that do not have a parent.
  if (isa<TemplateTypeParmDecl>(D) ||
      isa<NonTypeTemplateParmDecl>(D) ||
      isa<TemplateTemplateParmDecl>(D)) {
    if (auto const * const DC{D->getDeclContext()}) {
      if (isa<NamespaceDecl>(DC) ||
          isa<TranslationUnitDecl>(DC))
        return;
    }
  }

  if (auto const *VD{dyn_cast<VarDecl>(D)}) {
    if (VD->isStaticDataMember() && VD == VD->getDefinition())
      return;

    if (VD->hasExternalFormalLinkage())
      return;
  }

  // These are all decl types that we do not need to track. For the template
  // decls, there is a non-template decl as a child that we are watching and
  // the template decl itself is actually a duplicate in the AST.
  if (D->getLocation().isInvalid())
    return;

  auto unmodifiedName{D->getNameAsString()};

  // Ignore empty/reserved names
  if (unmodifiedName.empty() || unmodifiedName[0] == '_')
    return;

  // Get the version of the name with ambiguity removed.
  auto name{removeAmbiguity(unmodifiedName)};

  auto iter{m_ids.find(name)};
  if (iter != m_ids.end()) {
    auto &recordList{iter->second};
    for (auto const &r : recordList) {

      // Ignore function overloads
      if (bothAre<FunctionDecl>(D, r.D)) {
        if (hasSameQualifiedName(D, r.D))
          continue;
      }

      // Ignore variables that are actually the same variable
      if (bothAre<VarDecl>(D, r.D)) {
        if (areTheSameVarDecls(D, r.D))
          continue;
      }

      // Ignore names in different blocks
      if (areContainedInDifferentBlocks(D, r.D))
        continue;

      if (isa<ParmVarDecl>(D)) {
        if (isa<FieldDecl>(r.D))
          continue;
      }

      if (isa<ParmVarDecl>(r.D)) {
        if (isa<FieldDecl>(D))
          continue;
      }

      diag(D->getLocation(), "Different identifiers shall be typographically unambiguous");
      diag(r.D->getLocation(), "previous identifier found here", DiagnosticIDs::Note);

      // diag(r.D->getLocation(), " - name: %0", DiagnosticIDs::Note) << D->getQualifiedNameAsString();
      // diag(r.D->getLocation(), " - name: %0", DiagnosticIDs::Note) << r.D->getQualifiedNameAsString();
      // diag(r.D->getLocation(), " - specifier: %0", DiagnosticIDs::Note) <<getNestedNameSpecifierAsString(D);
      // diag(r.D->getLocation(), " - specifier: %0", DiagnosticIDs::Note) <<getNestedNameSpecifierAsString(r.D);
      // if (D->getParentFunctionOrMethod())
      //   diag(r.D->getLocation(), " - parent f/m: %0", DiagnosticIDs::Note) << D->getParentFunctionOrMethod();
      // else
      //   diag(r.D->getLocation(), " - parent f/m: %0", DiagnosticIDs::Note) << "null";
      // if (r.D->getParentFunctionOrMethod())
      //   diag(r.D->getLocation(), " - parent f/m: %0", DiagnosticIDs::Note) << r.D->getParentFunctionOrMethod();
      // else
      //   diag(r.D->getLocation(), " - parent f/m: %0", DiagnosticIDs::Note) << "null";
      // diag(r.D->getLocation(), " - parent context: %0", DiagnosticIDs::Note) << D->getDeclContext()->getDeclKindName();
      // diag(r.D->getLocation(), " - parent context: %0", DiagnosticIDs::Note) << r.D->getDeclContext()->getDeclKindName();

      // if (auto const * PD{findParentDecl(D)})
      //   diag(r.D->getLocation(), " - parent decl: %0", DiagnosticIDs::Note) << PD->getDeclKindName();
      // else
      //   diag(r.D->getLocation(), " - parent decl: %0", DiagnosticIDs::Note) << "null";

      // if (auto const * PD{findParentDecl(r.D)})
      //   diag(r.D->getLocation(), " - parent decl: %0", DiagnosticIDs::Note) << PD->getDeclKindName();
      // else
      //   diag(r.D->getLocation(), " - parent decl: %0", DiagnosticIDs::Note) << "null";

      // diag(r.D->getLocation(), "", DiagnosticIDs::Note);
      // diag(r.D->getLocation(), "", DiagnosticIDs::Note);

      return;
    }
  }

  m_ids[name].push_back({D});
}

} // namespace bsl
} // namespace tidy
} // namespace clang
