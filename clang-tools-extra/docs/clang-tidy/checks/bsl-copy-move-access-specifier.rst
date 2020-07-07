.. title:: clang-tidy - bsl-copy-move-access-specifier

bsl-copy-move-access-specifier
==============================

Checks that copy and move constructors and copy assignment and move 
assignment operators are declared protected or deleted in base class.

Example:

.. code-block:: c++

  // Abstract base class
  class A {
  public:
    A() = default;
    A(A const &) = default; // Non-compliant
    A(A &&) = default;      // Non-compliant
    virtual ~A() = 0;
    A &operator=(A const &) = default; // Non-compliant
    A &operator=(A &&) = default;      // Non-compliant
  };

  class B : public A {};

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

  class E {
  public:
    E() = default;
    virtual ~E() = 0;
    E(E const &) = delete;            // Compliant
    E(E &&) = delete;                 // Compliant
    E &operator=(E const &) = delete; // Compliant
    E &operator=(E &&) = delete;      // Compliant
  };

  class F : public E {};

  // Non-abstract base class
  class G {
  public:
    G() = default;
    virtual ~G() = default;
    G(G const &) = default;            // Non-compliant
    G(G &&) = default;                 // Non-compliant
    G &operator=(G const &) = default; // Non-compliant
    G &operator=(G &&) = default;      // Non-compliant
  };

  class H : public G {};
