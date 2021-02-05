### **Ubuntu Linux**
To compile the LLVM on Ubuntu (20.04 or higher) run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF
make -j
```

### **Windows**
To compile the LLVM on Windows run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF -Thost=x64
ninja
```

### **CI Package**
To compile the LLVM on for CI run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=MinSizeRel -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF -DCMAKE_INSTALL_PREFIX=/usr/lib/llvm-bareflank -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_LINK_LLVM_DYLIB=ON
make -j
sudo make install
cp -R /usr/lib/llvm-bareflank .
tar -czvf llvm-bareflank.tar.gz llvm-bareflank
```
