### **Ubuntu Linux**
To compile the LLVM on Ubuntu (20.04 or higher) run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF
```

### **Windows**
To compile the LLVM on Windows run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF -Thost=x64
```
