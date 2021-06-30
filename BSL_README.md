### **Ubuntu Linux**
To compile the LLVM on Ubuntu (20.04 or higher) run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;compiler-rt" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF -DCMAKE_CXX_COMPILER="clang++" -DCMAKE_C_COMPILER="clang"
make -j<number of cores>
```

### **Windows**
To compile the LLVM on Windows run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=Release -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;compiler-rt" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF -Thost=x64
ninja
```

### **CI Package**
To compile the LLVM on for CI run the following:
```bash
git clone https://github.com/bareflank/llvm-project
mkdir llvm-project-build && cd llvm-project-build
cmake ../llvm-project/llvm -DCMAKE_BUILD_TYPE=MinSizeRel -DLLVM_TARGETS_TO_BUILD="X86" -DLLVM_ENABLE_PROJECTS="clang;clang-tools-extra;lld;compiler-rt" -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_DOCS=OFF -DCMAKE_INSTALL_PREFIX=/usr/lib/llvm-bareflank -DLLVM_INSTALL_TOOLCHAIN_ONLY=ON -DLLVM_BUILD_LLVM_DYLIB=ON -DLLVM_LINK_LLVM_DYLIB=ON
make -j<number of cores>
```

Once built, use the following to create a package that can be installed by
someone else. This is done by actually installing onto the system, and
packaging up what was installed as follows:
```
sudo rm -Rf /usr/lib/llvm-bareflank
sudo make install
sudo rm -R /usr/lib/llvm-bareflank
cp -R /usr/lib/llvm-bareflank .
tar -czvf llvm-bareflank.tar.gz llvm-bareflank
cp llvm-bareflank.tar.gz ../bsl/utils/linux/llvm-bareflank.tar.gz
cp llvm-bareflank.tar.gz ../hypervisor/utils/linux/llvm-bareflank.tar.gz
```

Then copy the resulting tarball to the <repo>/utils/linux folder. This will get
used by the CI system for testing against Clang Tidy, and can be used by
others if they do not want to compile LLVM manually. Make sure that each repo
gets a fresh copy as they maintain their own versions to prevent people from
being required to download all of the repos just to get CI tools. For example:
```
cp -Rf llvm-bareflank.tar.gz ../bsl/utils/linux/
cp -Rf llvm-bareflank.tar.gz ../hypervisor/utils/linux/
cp -Rf llvm-bareflank.tar.gz ../microv/utils/linux/
```

### **Write Your Own Checks**

To create a check
```
cd ~/working/llvm-project/clang-tools-extra/clang-tidy
./add_new_check.py bsl <name-of-check>
```

To test your check
```
cd ~/llvm-project/clang-tools-extra/test/clang-tidy
./check_clang_tidy.py checkers/<name>.cpp <name> bsl-tmp
```

To dump the ast
```
clang-check -ast-dump ../bsl/tests/aligned_storage/overview.cpp --extra-arg="-fno-color-diagnostics" -I../bsl/include/ -- > dump.txt
```
