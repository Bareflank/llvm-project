====================================================
Extra Clang Tools 11.0.0 (In-Progress) Release Notes
====================================================

.. contents::
   :local:
   :depth: 3

Written by the `LLVM Team <https://llvm.org/>`_

.. warning::

   These are in-progress notes for the upcoming Extra Clang Tools 11 release.
   Release notes for previous releases can be found on
   `the Download Page <https://releases.llvm.org/download.html>`_.

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

Note that if you are reading this file from a Git checkout or the
main Clang web page, this document applies to the *next* release, not
the current one. To see the release notes for a specific release, please
see the `releases page <https://llvm.org/releases/>`_.

What's New in Extra Clang Tools 11.0.0?
=======================================

Some of the major new features and improvements to Extra Clang Tools are listed
here. Generic improvements to Extra Clang Tools as a whole or to its underlying
infrastructure are described first, followed by tool-specific sections.

Major New Features
------------------

...

Improvements to clangd
----------------------

The improvements are...

Improvements to clang-doc
-------------------------

The improvements are...

Improvements to clang-query
---------------------------

The improvements are...

Improvements to clang-rename
----------------------------

The improvements are...

Improvements to clang-tidy
--------------------------

New module
^^^^^^^^^^
- New module `llvmlibc`.

  This module contains checks related to the LLVM-libc coding standards.

New checks
^^^^^^^^^^
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

- New :doc:`bsl-copy-move-access-specifier
  <clang-tidy/checks/bsl-copy-move-access-specifier>` check.

  Checks that copy and move constructors and copy assignment and move 
  assignment operators are declared protected or deleted in base class.

- New :doc:`bsl-decl-forbidden
  <clang-tidy/checks/bsl-decl-forbidden>` check.

  Warns if unions or bitfields are declared.

- New :doc:`bsl-friend-decl
  <clang-tidy/checks/bsl-friend-decl>` check.

  Warns on friend declarations.

- New :doc:`bsl-function-name-use
  <clang-tidy/checks/bsl-function-name-use>` check.

  Warns if a function name is used in an expression other than a call
  or address-of.

- New :doc:`bsl-lambda-implicit-capture
  <clang-tidy/checks/bsl-lambda-implicit-capture>` check.

  Check for implicit captures of variables with automatic storage duration.

- New :doc:`bsl-lambda-param-list
  <clang-tidy/checks/bsl-lambda-param-list>` check.

  Ensures lambda expressions' parameter lists are explicit.

- New :doc:`bsl-literals-digit-separator
  <clang-tidy/checks/bsl-literals-digit-separator>` check.

  Warns on unusual digit separator locations in integer literals.

- New :doc:`bsl-literals-user-defined
  <clang-tidy/checks/bsl-literals-user-defined>` check.

  Warns on usage of user-defined literals.

- New :doc:`bsl-namespace-global
  <clang-tidy/checks/bsl-namespace-global>` check.

  Warns if anything other than main(), operator new/delete, namespaces, or
  extern "C" declarations are present at global scope.

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

  FIXME: add release notes.

- New :doc:`bsl-pure-override
  <clang-tidy/checks/bsl-pure-override>` check.

  Warns if a pure virtual function overrides a non-pure function.

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

- New :doc:`bsl-using-decl-scope
  <clang-tidy/checks/bsl-using-decl-scope>` check.

  Warns if a using declaration in a header file is outside of
  either a class or function.

- New :doc:`bsl-var-braced-init
  <clang-tidy/checks/bsl-var-braced-init>` check.

  Warns whenever direct list initialization is not used for
  non-auto-declared variables. Warns whenever any list initialization
  is used for auto-declared variables.

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

- New :doc:`bsl-literals-uppercase-suffix
  <clang-tidy/checks/bsl-literals-uppercase-suffix>` check.

  Checks that each integer literal has an uppercase integer suffix.

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

- New :doc:`bugprone-spuriously-wake-up-functions
  <clang-tidy/checks/bugprone-spuriously-wake-up-functions>` check.

  Finds ``cnd_wait``, ``cnd_timedwait``, ``wait``, ``wait_for``, or
  ``wait_until`` function calls when the function is not invoked from a loop
  that checks whether a condition predicate holds or the function has a
  condition parameter.

- New :doc:`bugprone-reserved-identifier
  <clang-tidy/checks/bugprone-reserved-identifier>` check.

  Checks for usages of identifiers reserved for use by the implementation.

- New :doc:`bugprone-suspicious-include
  <clang-tidy/checks/bugprone-suspicious-include>` check.

  Finds cases where an include refers to what appears to be an implementation
  file, which often leads to hard-to-track-down ODR violations, and diagnoses
  them.

- New :doc:`cert-oop57-cpp
  <clang-tidy/checks/cert-oop57-cpp>` check.

  Flags use of the `C` standard library functions ``memset``, ``memcpy`` and
  ``memcmp`` and similar derivatives on non-trivial types.

- New :doc:`llvmlibc-restrict-system-libc-headers
  <clang-tidy/checks/llvmlibc-restrict-system-libc-headers>` check.

  Finds includes of system libc headers not provided by the compiler within
  llvm-libc implementations.

- New :doc:`objc-dealloc-in-category
  <clang-tidy/checks/objc-dealloc-in-category>` check.

  Finds implementations of -dealloc in Objective-C categories.

- New :doc:`misc-no-recursion
  <clang-tidy/checks/misc-no-recursion>` check.

  Finds recursive functions and diagnoses them.

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

Changes in existing checks
^^^^^^^^^^^^^^^^^^^^^^^^^^

- Improved :doc:`readability-qualified-auto
  <clang-tidy/checks/readability-qualified-auto>` check now supports a
  `AddConstToQualified` to enable adding ``const`` qualifiers to variables
  typed with ``auto *`` and ``auto &``.

- Improved :doc:`readability-redundant-string-init
  <clang-tidy/checks/readability-redundant-string-init>` check now supports a
  `StringNames` option enabling its application to custom string classes. The
  check now detects in class initializers and constructor initializers which
  are deemed to be redundant.

- Checks supporting the ``HeaderFileExtensions`` flag now support ``;`` as a
  delimiter in addition to ``,``, with the latter being deprecated as of this
  release. This simplifies how one specifies the options on the command line:
  ``--config="{CheckOptions: [{ key: HeaderFileExtensions, value: h;;hpp;hxx }]}"``

Renamed checks
^^^^^^^^^^^^^^

- The 'fuchsia-restrict-system-headers' check was renamed to :doc:`portability-restrict-system-includes
  <clang-tidy/checks/portability-restrict-system-includes>`

Improvements to include-fixer
-----------------------------

The improvements are...

Improvements to clang-include-fixer
-----------------------------------

The improvements are...

Improvements to modularize
--------------------------

The improvements are...

Improvements to pp-trace
------------------------

The improvements are...

Clang-tidy visual studio plugin
-------------------------------

