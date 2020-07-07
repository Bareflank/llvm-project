// RUN: %check_clang_tidy %s bsl-copy-move-access-specifier %t

// Abstract base class
class A {
public:
  A() = default;
  A(A const &) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
  A(A &&) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
  virtual ~A() = 0;
  A &operator=(A const &) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
  A &operator=(A &&) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
};

class B : public A {};
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
// CHECK-MESSAGES: :[[@LINE-2]]:7: warning: Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]

// Abstract base class
class C {
public:
  C() = default;
  virtual ~C() = 0;

protected: // All compliant
  C(C const &) = default;
  C(C &&) = default;
  C &operator=(C const &) = default;
  C &operator=(C &&) = default;
};

class D : public C {};
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
// CHECK-MESSAGES: :[[@LINE-2]]:7: warning: Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]

class E {
public:
  E() = default;
  virtual ~E() = 0;
  E(E const &) = delete;
  E(E &&) = delete;
  E &operator=(E const &) = delete;
  E &operator=(E &&) = delete;
};

class F : public E {};

// Non-abstract base class
class G {
public:
  G() = default;
  virtual ~G() = default;
  G(G const &) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
  G(G &&) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:3: warning: Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
  G &operator=(G const &) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
  G &operator=(G &&) = default;
  // CHECK-MESSAGES: :[[@LINE-1]]:6: warning: Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
};

class H : public G {};
// CHECK-MESSAGES: :[[@LINE-1]]:7: warning: Copy and move assignment operators shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
// CHECK-MESSAGES: :[[@LINE-2]]:7: warning: Copy and move constructors shall be declared protected or defined “=delete” in base class; otherwise declare non-base class as final [bsl-copy-move-access-specifier]
