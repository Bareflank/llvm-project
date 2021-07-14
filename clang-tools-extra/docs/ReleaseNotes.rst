======================================
Extra Clang Tools 11.0.0 Release Notes
======================================

.. contents::
   :local:
   :depth: 3

Written by the `LLVM Team <https://llvm.org/>`_

Introduction
============

This document contains the release notes for the Extra Clang Tools, part of the
Clang release 11.0.0. Here we describe the status of the Extra Clang Tools in
some detail, including major improvements from the previous release and new
feature work. All LLVM releases may be downloaded from the `LLVM releases web
site <https://llvm.org/releases/>`_.

For more information about Clang or LLVM, including information about
the latest release, please see the `Clang Web Site <https://clang.llvm.org>`_ or
the `LLVM Web Site <https://llvm.org>`_.

What's New in Extra Clang Tools 11.0.0?
=======================================

Some of the major new features and improvements to Extra Clang Tools are listed
here. Generic improvements to Extra Clang Tools as a whole or to its underlying
infrastructure are described first, followed by tool-specific sections.

Improvements to clangd
----------------------

Performance
^^^^^^^^^^^

- Eliminated long delays after adding/removing includes ("async preambles")

- Faster indexing

- Less memory used to index headers used by open files ("dynamic index")

- Many requests are implicitly cancelled rather than queued when the file is
  edited, preventing a backlog

- Background indexing can be selectively disabled per-path through config

Selecting and targeting
^^^^^^^^^^^^^^^^^^^^^^^

- Improved understanding and selection around broken code ("recovery AST")

- Operations like "go-to-definition" will target things on the left of the
  cursor, if there is nothing eligible on the right.

- Arguments to ``assert()``-like macros can be properly selected.

Diagnostics
^^^^^^^^^^^

- When a header is saved, diagnostics for files that use it are updated.

- Calls ``std::make_unique`` produce diagnostics for the constructor call.
  (Template functions *in general* are not expanded for performance reasons).

- Diagnostics update more quickly for files that build quickly (no 500ms delay)

- Automatic fixes are offered even when they affect macro arguments.

- Warnings from included headers are not shown (but errors still are).

- A handful of high-quality clang-tidy checks are enabled by default:

  - readability-misleading-indentation,

  - readability-deleted-default,

  - bugprone-integer-division,

  - bugprone-sizeof-expression,

  - bugprone-suspicious-missing-comma,

  - bugprone-unused-raii,

  - bugprone-unused-return-value,

  - misc-unused-using-decls,

  - misc-unused-alias-decls,

  - misc-definitions-in-headers

Refactorings
^^^^^^^^^^^^

- Rename applies across the project, using the index.

- Accuracy of rename improved in many places.

- New refactoring: add using declaration for qualified name.

- New refactoring: move function definition out-of-line.

Code completion
^^^^^^^^^^^^^^^

- Function call parentheses are not inserted if they already exist.

- Completion of ``#include`` filenames triggers earlier (after ``<``, ``"``, and
  ``/``) and is less aggressive about replacing existing text.

- Documentation is reflowed in the same way as on hover.

Go-to-definition
^^^^^^^^^^^^^^^^

- Dependent names in templates may be heuristically resolved

- Identifiers in comments may be resolved using other occurrences in the file
  or in the index.

- Go-to-definition on an ``override`` or ``final`` specifier jumps to the
  overridden method.

Hover
^^^^^

- Expressions passed as function arguments show parameter name, conversions etc.

- Members now include the access specifier in the displayed declaration.

- Classes and fields show memory layout information (size and offset).

- Somewhat improved understanding of formatting in documentation comments.

- Trivial inline getters/setters are implicitly documented as such.

Highlighting
^^^^^^^^^^^^

- The ``semanticTokens`` protocol from LSP 3.16 is supported.
  (Only token types are exposed, no modifiers yet).

- The non-standard ``textDocument/semanticHighlighting`` notification is
  deprecated and will be removed in clangd 12.

- Placing the cursor on a control flow keyword highlights related flow
  (e.g. ``break`` -> ``for``).

Language support
^^^^^^^^^^^^^^^^

- clangd features now work inside templates on windows.
  (MSVC-compatible delayed-template-parsing is no longer used).

- Objective-C properties can be targeted and cross-references are indexed.

- Field names in designated initializers (C++20) can be targeted, and code
  completion works in many cases.

- ``goto`` labels: go-to-defintion, cross-references, and rename all work.

- Concepts (C++20): go-to-definition on concept names, and some limited code
  completion support for concept members.

System integration
^^^^^^^^^^^^^^^^^^

- The project index is now written to ``$PROJECT/.cache/clangd/index``.
  ``$PROJECT/.clangd`` is now expected to be a configuration file.

  Old ``$PROJECT/.clangd`` directories can safely be deleted.

  We recommend including both ``.cache/`` and ``.clangd/`` (with trailing slash)
  in ``.gitignore``, for backward-compatibility with earlier releases of clangd.

- For non-project files (those without a compilation database), the index
  location better reflects OS conventions:

  - ``%LocalAppData%\clangd\index`` on Windows

  - ``$(getconf DARWIN_USER_CACHE_DIR)/clangd/index`` on Mac

  - ``$XDG_CACHE_HOME/clangd/index`` or ``~/.cache/clangd/index`` on others

  Old ``~/.clangd/index`` directories can safely be deleted.

- clangd now reads configuration from ``.clangd`` files inside your project,
  and from a user configuration file in an OS-specific location:

  - ``%LocalAppData%\clangd\config.yaml`` on Windows

  - ``~/Library/Preferences/clangd/config.yaml`` on Mac

  - ``$XDG_CONFIG_HOME/clangd/config.yaml`` or ``~/.config/clangd/config.yaml``
    on others

  See `clangd configuration format <https://clangd.llvm.org/config.html>`_.

- clangd will search for compilation databases (``compile_commands.json``) in
  a ``build/`` subdirectory, as well as in the project root.
  This follows CMake conventions, avoiding the need for a symlink in many cases.

- Compile flags can be selectively modified per-path, using configuration.

- Improved filtering of unhelpful compile flags (such as those relating to
  pre-compiled headers).

- Improved detection of standard library headers location.

Miscellaneous
^^^^^^^^^^^^^

- Background indexing status is reported using LSP 3.15 progress events
  (``window/workDoneProgress/create``).

- Infrastructure for gathering internal metrics.
  (Off by default, set ``$CLANGD_METRICS`` to generate a named CSV file).

- Document versions are now tracked, version is reported along with diagnostics.

- Too many stability and correctness fixes to mention.

Improvements to clang-tidy
--------------------------

New module
^^^^^^^^^^
- New module `llvmlibc`.

  This module contains checks related to the LLVM-libc coding standards.

New checks
^^^^^^^^^^
- New :doc:`bsl-assign-op-decl-ref-qualifier
  <clang-tidy/checks/bsl-assign-op-decl-ref-qualifier>` check.

  Checks that assignment operators are declared with the ref-qualifier &.

- New :doc:`bsl-auto-type-usage
  <clang-tidy/checks/bsl-auto-type-usage>` check.

  Checks that the auto specifier is only used
    1. to declare that a variable has the same type as return type of a function call
    2. to declare that a variable has the same type as initializer of non-fundamental
      (non-literal) type
    3. to declare parameters of a generic lambda expression
    4. to declare a function (template) using trailing return type syntax.

- New :doc:`bsl-boolean-operators-forbidden
  <clang-tidy/checks/bsl-boolean-operators-forbidden>` check.

  Warns if you use && or ||.

- New :doc:`bsl-class-base
  <clang-tidy/checks/bsl-class-base>` check.

  Warns if a class derives from more than one non-interface base class.

- New :doc:`bsl-class-final-function
  <clang-tidy/checks/bsl-class-final-function>` check.

  Warns if a non-overridden virtual function is declared in final class and
  warns if an overridden function is not marked final in a final class.

- New :doc:`bsl-class-member-init
  <clang-tidy/checks/bsl-class-member-init>` check.

  Warns in any of the following cases:
    1. class has fields that use a mixture of in-class initializers
      and constructor initializers
    2. constructor doesn't init each field with member initialization list
      (unless they are initialized in-class)

- New :doc:`bsl-class-member-redefined
  <clang-tidy/checks/bsl-class-member-redefined>` check.

  Warns if a derived class defines a data member or method that hides
  a member with the same name in one of its base classes.

- New :doc:`bsl-class-virtual-base
  <clang-tidy/checks/bsl-class-virtual-base>` check.

  Warns if a class has any virtual bases.

- New :doc:`bsl-comparison-operators-forbidden
  <clang-tidy/checks/bsl-comparison-operators-forbidden>` check.

  Warns if you use >= or <=

- New :doc:`bsl-copy-move-access-specifier
  <clang-tidy/checks/bsl-copy-move-access-specifier>` check.

  Checks that copy and move constructors and copy assignment and move
  assignment operators are declared protected or deleted in base class.

- New :doc:`bsl-const-obj-std-move
  <clang-tidy/checks/bsl-const-obj-std-move>` check.

  Warns if std::move (or bsl::move) is used on objects declared const or const&.

- New :doc:`bsl-decl-forbidden
  <clang-tidy/checks/bsl-decl-forbidden>` check.

  Warns if unions or bitfields are declared.

- New :doc:`bsl-destructor-access-specifier
  <clang-tidy/checks/bsl-destructor-access-specifier>` check.

  Warns if destructor of base class is not public virtual, public override,
  or protected non-virtual, unless public destructor is non-virtual in final class.

- New :doc:`bsl-documentation
  <clang-tidy/checks/bsl-documentation>` check.

  Warns when documentation is missing

- New :doc:`bsl-else-required-after-if
  <clang-tidy/checks/bsl-else-required-after-if>` check.

  Warns if else/bsl::touch is missing in certain situations. This will also
  warn if else is used when it shouldn't be, which would prevent the above
  checks from working correctly.

- New :doc:`bsl-empty-if-else
  <clang-tidy/checks/bsl-empty-if-else>` check.

  Warns if an if, else-if or else statement is empty.

- New :doc:`bsl-explicit-constructor
  <clang-tidy/checks/bsl-explicit-constructor>` check.

  Checks that constructors with a single argument of builtin type are marked
  explicit to avoid unwanted conversions.

- New :doc:`bsl-forward-reference-overloaded
  <clang-tidy/checks/bsl-forward-reference-overloaded>` check.

  Checks that a function that containing a “forwarding reference” as its argument
  is not overloaded unless the overload has a different number of parameters

- New :doc:`bsl-function-name-use
  <clang-tidy/checks/bsl-function-name-use>` check.

  Warns if a function name is used in an expression other than a call
  or address-of.

- New :doc:`bsl-function-noexcept
  <clang-tidy/checks/bsl-function-noexcept>` check.

  Checks that every function is marked as noexcept or noexcept(false).

- New :doc:`bsl-identifier-typographically-unambiguous
  <clang-tidy/checks/bsl-identifier-typographically-unambiguous>` check.

  Checks that different identifiers are typographically unambiguous.

- New :doc:`bsl-implicit-conversions-forbidden
  <clang-tidy/checks/bsl-implicit-conversions-forbidden>` check.

  Warns of any implicit conversion

- New :doc:`bsl-integral-literals-in-constexpr
  <clang-tidy/checks/bsl-integral-literals-in-constexpr>` check.

  FIXME: add release notes.

- New :doc:`bsl-lambda-implicit-capture
  <clang-tidy/checks/bsl-lambda-implicit-capture>` check.

  Check for implicit captures of variables with automatic storage duration.

- New :doc:`bsl-lambda-param-list
  <clang-tidy/checks/bsl-lambda-param-list>` check.

  Ensures lambda expressions' parameter lists are explicit.

- New :doc:`bsl-left-side-constants
  <clang-tidy/checks/bsl-left-side-constants>` check.

  FIXME: add release notes.

- New :doc:`bsl-literals-digit-separator
  <clang-tidy/checks/bsl-literals-digit-separator>` check.

  Warns on unusual digit separator locations in integer literals.

- New :doc:`bsl-literals-user-defined
  <clang-tidy/checks/bsl-literals-user-defined>` check.

  Warns on usage of user-defined literals.

- New :doc:`bsl-name-case
  <clang-tidy/checks/bsl-name-case>` check.

  Warns when a named declaration is in the wrong case

- New :doc:`bsl-name-prefixes
  <clang-tidy/checks/bsl-name-prefixes>` check.

  Warns if a variable does not have the correct prefix in it's name

- New :doc:`bsl-namespace-global
  <clang-tidy/checks/bsl-namespace-global>` check.

  Warns if anything other than main(), operator new/delete, namespaces, or
  extern "C" declarations are present at global scope.

- New :doc:`bsl-nodiscard
  <clang-tidy/checks/bsl-nodiscard>` check.

  Warns if [[nodiscard]] or [[maybe_unused]] are missing

- New :doc:`bsl-non-pod-classdef
  <clang-tidy/checks/bsl-non-pod-classdef>` check.

  Warns when member data in non-POD class type is not private and on non-POD types
  not defined as class

- New :doc:`bsl-non-safe-integral-types-are-forbidden
  <clang-tidy/checks/bsl-non-safe-integral-types-are-forbidden>` check.

  Warns if you try to use an integral type instead of a safe_integral

- New :doc:`bsl-op-bitwise-operands
  <clang-tidy/checks/bsl-op-bitwise-operands>` check.

  Warns if operands to binary bitwise operations have different types.

- New :doc:`bsl-op-conditional-subexpr
  <clang-tidy/checks/bsl-op-conditional-subexpr>` check.

  Warns whenever a conditional operator is used as a subexpression

- New :doc:`bsl-op-equality-virt-memfn-nullptr
  <clang-tidy/checks/bsl-op-equality-virt-memfn-nullptr>` check.

  Warns if a pointer to virtual member function is compared for equality
  to an expression that is not 'nullptr'.

- New :doc:`bsl-op-forbidden-overload
  <clang-tidy/checks/bsl-op-forbidden-overload>` check.

  Warns if binary-logical, comma, subscript, or unary-ampresand operators
  are overloaded.

- New :doc:`bsl-op-logical-postfix
  <clang-tidy/checks/bsl-op-logical-postfix>` check.

  Warns if either operand to '&&' or '||' expressions are either
  not postfix or are not '&&' or '||', respectively.

- New :doc:`bsl-op-mixed-increment-decrement
  <clang-tidy/checks/bsl-op-mixed-increment-decrement>` check.

  Warns if "++" or "--" operators are mixed with other operations.

- New :doc:`bsl-op-relational-return-bool
  <clang-tidy/checks/bsl-op-relational-return-bool>` check.

  Warns if a comparison operator does not return a bool

- New :doc:`bsl-pass-by-reference
  <clang-tidy/checks/bsl-pass-by-reference>` check.

  FIXME: add release notes.

- New :doc:`bsl-pure-override
  <clang-tidy/checks/bsl-pure-override>` check.

  Warns if a pure virtual function overrides a non-pure function.

- New :doc:`bsl-special-member-functions
  <clang-tidy/checks/bsl-special-member-functions>` check.

  Checks for classes where some, but not all, of the special member functions
  are defined.

- New :doc:`bsl-stmt-forbidden
  <clang-tidy/checks/bsl-stmt-forbidden>` check.

  Warns on any goto or do statement.

- New :doc:`bsl-stmt-switch-case-parent
  <clang-tidy/checks/bsl-stmt-switch-case-parent>` check.

  Warns whenever a 'case' or 'default' label of a switch statement
  does not have either another 'case' label or the body of the switch
  as its parent statement.

- New :doc:`bsl-stmt-switch-default-break
  <clang-tidy/checks/bsl-stmt-switch-default-break>` check.

  Warns if the default case of switch statements do no end with
  either break or throw.

- New :doc:`bsl-stmt-switch-default-last
  <clang-tidy/checks/bsl-stmt-switch-default-last>` check.

  Warns if the default label is not last in a switch statement.

- New :doc:`bsl-struct-def
  <clang-tidy/checks/bsl-struct-def>` check.

  Checks that a struct only contains public data members, does not provide any
  special member functions or methods, and is not a base or inherits from
  another struct or class

- New :doc:`bsl-template-generic-param
  <clang-tidy/checks/bsl-template-generic-param>` check.

  Checks that a copy constructor/assignment operator is declared when there is a
  template constructor/assignment operator (respectively) with a single parameter
  that is a generic parameter.

- New :doc:`bsl-ternary-operator-forbidden
  <clang-tidy/checks/bsl-ternary-operator-forbidden>` check.

  Warns if you use the ternary operator

- New :doc:`bsl-types-fixed-width-ints-arithmetic-check
  <clang-tidy/checks/bsl-types-fixed-width-ints-arithmetic-check>` check.

  FIXME: add release notes.

- New :doc:`bsl-user-defined-type-names-match-header-name
  <clang-tidy/checks/bsl-user-defined-type-names-match-header-name>` check.

  Warns if a user defined type does not have the same name as the header
  it is defined in.

- New :doc:`bsl-using-decl-scope
  <clang-tidy/checks/bsl-using-decl-scope>` check.

  Warns if a using declaration in a header file is outside of
  either a class or function.

- New :doc:`bsl-using-ident-unique-namespace
  <clang-tidy/checks/bsl-using-ident-unique-namespace>` check.

  Warns if a user defined type does not have a unique namespace

- New :doc:`bsl-using-namespace-forbidden
  <clang-tidy/checks/bsl-using-namespace-forbidden>` check.

  Warns if you include a "using namespace blah" in your code.

- New :doc:`bsl-var-braced-init
  <clang-tidy/checks/bsl-var-braced-init>` check.

  Warns whenever direct list initialization is not used for
  non-auto-declared variables. Warns whenever any list initialization
  is used for auto-declared variables.

- New :doc:`bsl-for-loop-counter
  <clang-tidy/checks/bsl-for-loop-counter>` check.

  Warns if for loop does not contain single loop-counter and uses floating point type.

- New :doc:`bsl-verify-const
  <clang-tidy/checks/bsl-verify-const>` check.

  FIXME: add release notes.

- New :doc:`cppcoreguidelines-avoid-non-const-global-variables
  <clang-tidy/checks/cppcoreguidelines-avoid-non-const-global-variables>` check.
  Finds non-const global variables as described in check I.2 of C++ Core
  Guidelines.

- New :doc:`bsl-enum-explicit
  <clang-tidy/checks/bsl-enum-explicit>` check.

  Check all enum declarations explicitly state the underlying type.

- New :doc:`bsl-enum-init
  <clang-tidy/checks/bsl-enum-init>` check.

  Check that either none or all or the first only enum constants
  are initialized.

- New :doc:`bsl-literals-ascii-only
  <clang-tidy/checks/bsl-literals-ascii-only>` check.

  Ensures only ASCII-encoded string literals are used.

- New :doc:`bsl-literals-no-octal
  <clang-tidy/checks/bsl-literals-no-octal>` check.

  Checks that no octal integer literals nor octal escape sequences
  exist.

- New :doc:`bsl-enum-scoped
  <clang-tidy/checks/bsl-enum-scoped>` check.

  Check all enums are scoped.

- New :doc:`bsl-non-pod-static
  <clang-tidy/checks/bsl-non-pod-static>` check.

  Checks that static storage duration is not used for non-POD variables.

- New :doc:`bsl-types-fixed-width-ints
  <clang-tidy/checks/bsl-types-fixed-width-ints>` check.

  Checks that fixed-width integer types are used.

- New :doc:`bsl-types-no-wide-char
  <clang-tidy/checks/bsl-types-no-wide-char>` check.

  Check that no wide-character types are used.

- New :doc:`bsl-unused-return-value
  <clang-tidy/checks/bsl-unused-return-value>` check.

  Checks for unused non-void function return values.

- New :doc:`bugprone-misplaced-pointer-arithmetic-in-alloc
  <clang-tidy/checks/bugprone-misplaced-pointer-arithmetic-in-alloc>` check.

  Finds cases where an integer expression is added to or subtracted from the
  result of a memory allocation function (``malloc()``, ``calloc()``,
  ``realloc()``, ``alloca()``) instead of its argument.

- New :doc:`bugprone-no-escape
  <clang-tidy/checks/bugprone-no-escape>` check.

  Finds pointers with the ``noescape`` attribute that are captured by an
  asynchronously-executed block.

- New :doc:`bugprone-reserved-identifier
  <clang-tidy/checks/bugprone-reserved-identifier>` check.

  Checks for usages of identifiers reserved for use by the implementation.

- New :doc:`bugprone-spuriously-wake-up-functions
  <clang-tidy/checks/bugprone-spuriously-wake-up-functions>` check.

  Finds ``cnd_wait``, ``cnd_timedwait``, ``wait``, ``wait_for``, or
  ``wait_until`` function calls when the function is not invoked from a loop
  that checks whether a condition predicate holds or the function has a
  condition parameter.

- New :doc:`bugprone-suspicious-include
  <clang-tidy/checks/bugprone-suspicious-include>` check.

  Finds cases where an include refers to what appears to be an implementation
  file, which often leads to hard-to-track-down ODR violations, and diagnoses
  them.

- New :doc:`cert-oop57-cpp
  <clang-tidy/checks/cert-oop57-cpp>` check.

  Flags use of the `C` standard library functions ``memset``, ``memcpy`` and
  ``memcmp`` and similar derivatives on non-trivial types.

- New :doc:`cppcoreguidelines-avoid-non-const-global-variables
  <clang-tidy/checks/cppcoreguidelines-avoid-non-const-global-variables>` check.

  Finds non-const global variables as described in check I.2 of C++ Core
  Guidelines.

- New :doc:`llvmlibc-callee-namespace
  <clang-tidy/checks/llvmlibc-callee-namespace>` check.

  Checks all calls resolve to functions within ``__llvm_libc`` namespace.

- New :doc:`llvmlibc-implementation-in-namespace
  <clang-tidy/checks/llvmlibc-implementation-in-namespace>` check.

  Checks all llvm-libc implementation is within the correct namespace.

- New :doc:`llvmlibc-restrict-system-libc-headers
  <clang-tidy/checks/llvmlibc-restrict-system-libc-headers>` check.

  Finds includes of system libc headers not provided by the compiler within
  llvm-libc implementations.

- New :doc:`misc-no-recursion
  <clang-tidy/checks/misc-no-recursion>` check.

  Finds recursive functions and diagnoses them.

- New :doc:`modernize-replace-disallow-copy-and-assign-macro
  <clang-tidy/checks/modernize-replace-disallow-copy-and-assign-macro>` check.

  Finds macro expansions of ``DISALLOW_COPY_AND_ASSIGN`` and replaces them with
  a deleted copy constructor and a deleted assignment operator.

- New :doc:`objc-dealloc-in-category
  <clang-tidy/checks/objc-dealloc-in-category>` check.

  Finds implementations of -dealloc in Objective-C categories.

- New :doc:`objc-nsinvocation-argument-lifetime
  <clang-tidy/checks/objc-nsinvocation-argument-lifetime>` check.

  Finds calls to ``NSInvocation`` methods under ARC that don't have proper
  argument object lifetimes.

- New :doc:`readability-use-anyofallof
  <clang-tidy/checks/readability-use-anyofallof>` check.

  Finds range-based for loops that can be replaced by a call to ``std::any_of``
  or ``std::all_of``.

New check aliases
^^^^^^^^^^^^^^^^^

- New alias :doc:`cert-con36-c
  <clang-tidy/checks/cert-con36-c>` to
  :doc:`bugprone-spuriously-wake-up-functions
  <clang-tidy/checks/bugprone-spuriously-wake-up-functions>` was added.

- New alias :doc:`cert-con54-cpp
  <clang-tidy/checks/cert-con54-cpp>` to
  :doc:`bugprone-spuriously-wake-up-functions
  <clang-tidy/checks/bugprone-spuriously-wake-up-functions>` was added.

- New alias :doc:`cert-dcl37-c
  <clang-tidy/checks/cert-dcl37-c>` to
  :doc:`bugprone-reserved-identifier
  <clang-tidy/checks/bugprone-reserved-identifier>` was added.

- New alias :doc:`cert-dcl51-cpp
  <clang-tidy/checks/cert-dcl51-cpp>` to
  :doc:`bugprone-reserved-identifier
  <clang-tidy/checks/bugprone-reserved-identifier>` was added.

- New alias :doc:`cert-str34-c
  <clang-tidy/checks/cert-str34-c>` to
  :doc:`bugprone-signed-char-misuse
  <clang-tidy/checks/bugprone-signed-char-misuse>` was added.

- New alias :doc:`llvm-else-after-return
  <clang-tidy/checks/llvm-else-after-return>` to
  :doc:`readability-else-after-return
  <clang-tidy/checks/readability-else-after-return>` was added.

Changes in existing checks
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Improved :doc:`performance-faster-string-find
  <clang-tidy/checks/performance-faster-string-find>` check.

  Now checks ``std::basic_string_view`` by default.

- Improved :doc:`readability-else-after-return
  <clang-tidy/checks/readability-else-after-return>` check now supports a
  `WarnOnConditionVariables` option to control whether to refactor condition
  variables where possible.

- Improved :doc:`readability-identifier-naming
  <clang-tidy/checks/readability-identifier-naming>` check.

  Now able to rename member references in class template definitions with
  explicit access.

- Improved :doc:`readability-redundant-string-init
  <clang-tidy/checks/readability-redundant-string-init>` check now supports a
  `StringNames` option enabling its application to custom string classes. The
  check now detects in class initializers and constructor initializers which
  are deemed to be redundant.

- Checks supporting the ``HeaderFileExtensions`` flag now support ``;`` as a
  delimiter in addition to ``,``, with the latter being deprecated as of this
  release. This simplifies how one specifies the options on the command line:
  ``--config="{CheckOptions: [{ key: HeaderFileExtensions, value: h;;hpp;hxx }]}"``

- Improved :doc:`readability-qualified-auto
  <clang-tidy/checks/readability-qualified-auto>` check now supports a
  `AddConstToQualified` to enable adding ``const`` qualifiers to variables
  typed with ``auto *`` and ``auto &``.

Renamed checks
^^^^^^^^^^^^^^

- The 'fuchsia-restrict-system-headers' check was renamed to :doc:`portability-restrict-system-includes
  <clang-tidy/checks/portability-restrict-system-includes>`

Other improvements
^^^^^^^^^^^^^^^^^^

- For `run-clang-tidy.py` add option to use alpha checkers from
  `clang-analyzer`.
