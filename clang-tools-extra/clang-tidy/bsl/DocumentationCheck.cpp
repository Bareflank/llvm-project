//===--- DocumentationCheck.cpp - clang-tidy ------------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "IsDefinedInATestFile.h"

#include "DocumentationCheck.h"
#include "clang/AST/ASTContext.h"
#include "clang/ASTMatchers/ASTMatchFinder.h"
#include "clang/AST/Comment.h"
#include "clang/AST/CommentCommandTraits.h"

using namespace clang::ast_matchers;

namespace clang {
namespace tidy {
namespace bsl {

std::string getCommandName(
  unsigned CommandID)
{
  if (auto const * const Info = clang::comments::CommandTraits::getBuiltinCommandInfo(CommandID))
    return Info->Name;

  return {};
}

bool isParamNameInFullComment(
  clang::comments::FullComment const * const FC, std::string const &name)
{
  for (auto const *BlockComment : FC->getBlocks()) {
    if (auto const *PCC = dyn_cast<clang::comments::ParamCommandComment>(BlockComment)) {
      if (PCC->getParamNameAsWritten() == name) {
        return true;
      }
    }

    if (auto const *TPCC = dyn_cast<clang::comments::TParamCommandComment>(BlockComment)) {
      if (TPCC->getParamNameAsWritten() == name) {
        return true;
      }
    }
  }

  return false;
}

bool hasABrief(
  ASTContext const * const Context, Decl const * const D)
{
  if (auto const *FC = Context->getCommentForDecl(D, nullptr)) {
    for (auto const *BlockComment : FC->getBlocks()) {
      if (auto const *BCC = dyn_cast<clang::comments::BlockCommandComment>(BlockComment)) {
        if (getCommandName(BCC->getCommandID()) == "brief") {
          return true;
        }
      }
    }
  }

  return false;
}

bool hasAReturn(
  ASTContext const * const Context, Decl const * const D)
{
  if (auto const *FC = Context->getCommentForDecl(D, nullptr)) {
    for (auto const *BlockComment : FC->getBlocks()) {
      if (auto const *BCC = dyn_cast<clang::comments::BlockCommandComment>(BlockComment)) {
        if (getCommandName(BCC->getCommandID()) == "return") {
          return true;
        }
      }
    }
  }

  return false;
}

clang::comments::ParamCommandComment const * hasExtraParamCommandComment(
  clang::comments::FullComment const * const FC)
{
  for (auto const *BlockComment : FC->getBlocks()) {
    if (auto const *PCC = dyn_cast<clang::comments::ParamCommandComment>(BlockComment)) {
      if (!PCC->isParamIndexValid()) {
        return PCC;
      }
    }
  }

  return nullptr;
}

clang::comments::TParamCommandComment const * hasExtraTemplateParamCommandComment(
  clang::comments::FullComment const * const FC)
{
  for (auto const *BlockComment : FC->getBlocks()) {
    if (auto const *TPCC = dyn_cast<clang::comments::TParamCommandComment>(BlockComment)) {
      if (!TPCC->isPositionValid()) {
        return TPCC;
      }
    }
  }

  return nullptr;
}

void DocumentationCheck::registerMatchers(MatchFinder *Finder) {
  Finder->addMatcher(
    functionDecl(
      unless(
        isImplicit()
      )
    ).bind("func-decl"),
    this
  );

  Finder->addMatcher(
    cxxRecordDecl(
      unless(
        isImplicit()
      )
    ).bind("class-decl"),
    this
  );

  Finder->addMatcher(
    varDecl(
      unless(
        isImplicit()
      )
    ).bind("var-decl"),
    this
  );

  Finder->addMatcher(
    fieldDecl(
      unless(
        isImplicit()
      )
    ).bind("field-decl"),
    this
  );

  Finder->addMatcher(
    typeAliasDecl(
      unless(
        isImplicit()
      )
    ).bind("alias-decl"),
    this
  );

  Finder->addMatcher(
    enumDecl(
      unless(
        isImplicit()
      )
    ).bind("enum-decl"),
    this
  );
}

void DocumentationCheck::check(const MatchFinder::MatchResult &Result) {
  auto const *Context = Result.Context;

  if (auto const *FD = Result.Nodes.getNodeAs<FunctionDecl>("func-decl")) {
    if (isDefinedInATestFile(Context, FD->getBeginLoc()))
      return;

    if (FD->isInvalidDecl())
      return;

    if (FD->getMemberSpecializationInfo())
      return;

    if (FD->getTemplateSpecializationInfo())
      return;

    if (auto const *CXXMD = dyn_cast<CXXMethodDecl>(FD)) {
      if (CXXMD->getParent()->isLambda())
        return;
    }

    if (!hasABrief(Context, FD)) {
      diag(FD->getLocation(), "Function %0 is missing documentation. Are you missing the '@brief' command?") << FD;
    }

    if (isa<CXXDeductionGuideDecl>(FD))
      return;

    if (!FD->getReturnType()->isVoidType() && !hasAReturn(Context, FD)) {
      diag(FD->getLocation(), "Function %0 is missing return documentation. Are you missing the '@return' command?") << FD;
    }

    if (auto const *FC = Context->getCommentForDecl(FD, nullptr)) {
      for (auto const *param : FD->parameters()) {
        if (!isParamNameInFullComment(FC, param->getNameAsString())) {
          diag(param->getLocation(), "Function parameter %0 is missing documentation") << param;
        }
      }

      if (auto const *PCC = hasExtraParamCommandComment(FC)) {
        diag(PCC->getBeginLoc(), "Parameter comment does not have an associated parameter");
      }

      if (auto const *FTD = FD->getDescribedFunctionTemplate()) {
        if (auto const *paramList = FTD->getTemplateParameters()) {
          for (auto const *param : *paramList) {
            if (param->getNameAsString() == "")
              continue;

            if (!isParamNameInFullComment(FC, param->getNameAsString())) {
              diag(param->getLocation(), "Template parameter %0 is missing documentation") << param;
            }
          }
        }

        if (auto const *TPCC = hasExtraTemplateParamCommandComment(FC)) {
          diag(TPCC->getBeginLoc(), "Template parameter comment does not have an associated template parameter");
        }
      }
    }

    return;
  }

  if (auto const *CXXRD = Result.Nodes.getNodeAs<CXXRecordDecl>("class-decl")) {
    if (isDefinedInATestFile(Context, CXXRD->getBeginLoc()))
      return;

    if (CXXRD->isInvalidDecl())
      return;

    if (isa<ClassTemplateSpecializationDecl>(CXXRD))
      return;

    if (CXXRD->isLambda())
      return;

    if (CXXRD->getDefinition() != CXXRD)
      return;

    if (!hasABrief(Context, CXXRD)) {
      if (CXXRD->isClass())
        diag(CXXRD->getLocation(), "Class %0 is missing documentation. Are you missing the '@brief' command?") << CXXRD;
      if (CXXRD->isStruct())
        diag(CXXRD->getLocation(), "Struct %0 is missing documentation. Are you missing the '@brief' command?") << CXXRD;
      if (CXXRD->isUnion())
        diag(CXXRD->getLocation(), "Union %0 is missing documentation. Are you missing the '@brief' command?") << CXXRD;
    }

    if (auto const *FC = Context->getCommentForDecl(CXXRD, nullptr)) {
      if (auto const *CTD = CXXRD->getDescribedClassTemplate()) {
        if (auto const *paramList = CTD->getTemplateParameters()) {
          for (auto const *param : *paramList) {
            if (param->getNameAsString() == "")
              continue;

            if (!isParamNameInFullComment(FC, param->getNameAsString())) {
              diag(param->getLocation(), "Template parameter %0 is missing documentation") << param;
            }
          }
        }

        if (auto const *TPCC = hasExtraTemplateParamCommandComment(FC)) {
          diag(TPCC->getBeginLoc(), "Template parameter comment does not have an associated template parameter");
        }
      }
    }

    return;
  }

  if (auto const *VD = Result.Nodes.getNodeAs<VarDecl>("var-decl")) {
    if (isDefinedInATestFile(Context, VD->getBeginLoc()))
      return;

    if (VD->isInvalidDecl())
      return;

    if (VD->getParentFunctionOrMethod())
      return;

    if (VD->getNameAsString() == "")
      return;

    if (VD->hasExternalFormalLinkage())
      return;

    if (!hasABrief(Context, VD)) {
      diag(VD->getLocation(), "Variable %0 is missing documentation. Are you missing the '@brief' command?") << VD;
    }

    return;
  }

  if (auto const *FD = Result.Nodes.getNodeAs<FieldDecl>("field-decl")) {
    if (isDefinedInATestFile(Context, FD->getBeginLoc()))
      return;

    if (FD->isInvalidDecl())
      return;

    if (!hasABrief(Context, FD)) {
      diag(FD->getLocation(), "Member %0 is missing documentation. Are you missing the '@brief' command?") << FD;
    }

    return;
  }

  if (auto const *TAD = Result.Nodes.getNodeAs<TypeAliasDecl>("alias-decl")) {
    if (isDefinedInATestFile(Context, TAD->getBeginLoc()))
      return;

    if (TAD->isInvalidDecl())
      return;

    if (TAD->getParentFunctionOrMethod())
      return;

    if (!hasABrief(Context, TAD)) {
      diag(TAD->getLocation(), "Alias %0 is missing documentation. Are you missing the '@brief' command?") << TAD;
    }

    if (auto const *FC = Context->getCommentForDecl(TAD, nullptr)) {
      if (auto const *TATD = TAD->getDescribedAliasTemplate()) {
        if (auto const *paramList = TATD->getTemplateParameters()) {
          for (auto const *param : *paramList) {
            if (param->getNameAsString() == "")
              continue;

            if (!isParamNameInFullComment(FC, param->getNameAsString())) {
              diag(param->getLocation(), "Template parameter %0 is missing documentation") << param;
            }
          }
        }
      }
    }

    return;
  }

  if (auto const *ED = Result.Nodes.getNodeAs<EnumDecl>("enum-decl")) {
    if (isDefinedInATestFile(Context, ED->getBeginLoc()))
      return;

    if (ED->isInvalidDecl())
      return;

    if (!hasABrief(Context, ED)) {
      diag(ED->getLocation(), "Enum %0 is missing documentation. Are you missing the '@brief' command?") << ED;
    }

    return;
  }
}

} // namespace bsl
} // namespace tidy
} // namespace clang
