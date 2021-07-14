//===--- IsInATestFile.h - clang-tidy ---------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_ISINATESTFILE_H
#define LLVM_CLANG_TOOLS_EXTRA_CLANG_TIDY_BSL_ISINATESTFILE_H

#include "../ClangTidyCheck.h"

#include "clang/AST/ASTContext.h"
#include "clang/AST/Comment.h"
#include "clang/AST/CommentCommandTraits.h"

namespace clang {
namespace tidy {
namespace bsl {

inline bool isDefinedInATestFile(
  ASTContext const * const Context, SourceLocation const Loc)
{
  if (Loc.isInvalid())
    return false;

  FullSourceLoc FullLocation = Context->getFullLoc(Loc);

  auto const File = FullLocation.getFileEntry();
  if (!File)
    return false;

  std::string filename{File->tryGetRealPathName()};

  const size_t in_clang_folder = filename.find("clang-tidy");
  if (std::string::npos != in_clang_folder)
    return false;

  const size_t test_folder1 = filename.find("test/");
  const size_t test_folder2 = filename.find("test\\");
  const size_t test_folder3 = filename.find("tests/");
  const size_t test_folder4 = filename.find("tests\\");
  if (std::string::npos != test_folder1 ||
      std::string::npos != test_folder2 ||
      std::string::npos != test_folder3 ||
      std::string::npos != test_folder4)
  {
    return true;
  }

  return false;
}

}
}
}

#endif
