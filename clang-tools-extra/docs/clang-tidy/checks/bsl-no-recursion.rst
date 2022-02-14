.. title:: clang-tidy - bsl-no-recursion

bsl-no-recursion
================

Finds strongly connected functions (by analyzing call graph for SCC's
that are loops), diagnoses each function in the cycle,
and displays one example of possible call graph loop (recursion).
Makes exception for constexpr functions, including lambdas
implicitly marked constexpr in C++17 or later. 
Based on misc-no-recursion.

Examples:

.. code-block:: c++

  // We don't have the definition of this function,
  // so we can't tell anything about it..
  void external();

  // This function is obviously not recursive.
  void no_recursion() {
  }

  // Since we don't know what `external()` does,
  // we don't know if this is recursive or not.
  void maybe_no_recursion() {
    external();
  }

  // Function calls itself - obviously a recursion.
  void endless_recursion() {
    endless_recursion();
  }

  bool external_oracle();
  bool another_external_oracle();

  // Function calls itself if some external function said so - recursion.
  void maybe_endless_recursion() {
    if (external_oracle())
      maybe_endless_recursion();
  }

  // Obviously-constrained recursion.
  void recursive_countdown(unsigned x) {
    if (x == 0)
      return;
    recursive_countdown(x - 1);
  }

  void indirect_recursion();
  void conditionally_executed() {
    if (external_oracle())
      indirect_recursion();
  }
  void indirect_recursion() {
    if (external_oracle())
      conditionally_executed();
  }

  void taint();
  void maybe_selfrecursion_with_two_backedges() {
    if (external_oracle())
      maybe_selfrecursion_with_two_backedges();
    taint();
    if (another_external_oracle())
      maybe_selfrecursion_with_two_backedges();
  }

  void indirect_recursion_with_alternatives();
  void conditionally_executed_choice_0() {
    if (external_oracle())
      indirect_recursion_with_alternatives();
  }
  void conditionally_executed_choice_1() {
    if (external_oracle())
      indirect_recursion_with_alternatives();
  }
  void indirect_recursion_with_alternatives() {
    if (external_oracle())
      conditionally_executed_choice_0();
    else
      conditionally_executed_choice_1();
  }

  static void indirect_recursion_with_depth2();
  static void conditionally_executed_depth1() {
    if (external_oracle())
      indirect_recursion_with_depth2();
  }
  static void conditionally_executed_depth0() {
    if (external_oracle())
      conditionally_executed_depth1();
  }
  void indirect_recursion_with_depth2() {
    if (external_oracle())
      conditionally_executed_depth0();
  }

  int boo();
  void foo(int x = boo()) {}
  void bar() {
    foo();
    foo();
  }
  int boo() {
    bar();
    return 0;
  }

  int recursion_through_function_ptr() {
    auto *ptr = &recursion_through_function_ptr;
    if (external_oracle())
      return ptr();
    return 0;
  }

  // Lambda implicitly constexpr in C++17 and later
  int recursion_through_lambda() {
    auto zz = []() {
      if (external_oracle())
        return recursion_through_lambda();
      return 0;
    };
    return zz();
  }

  struct recursion_through_destructor {
    ~recursion_through_destructor() {
      if (external_oracle()) {
        recursion_through_destructor variable;
        // variable goes out of scope, it's destructor runs, and we are back here.
      }
    }
  };

  constexpr int f(int n, int s=0) {
    return n ? f(n-1,s+n) : s;
  }

