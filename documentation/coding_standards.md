# Coding Guidelines

We will be using the Qt C++ coding standards for this project. See:

* [http://wiki.qt-project.org/Coding_Style]
* [http://wiki.qt-project.org/Coding_Conventions]

# Documentation

Every class should at the very least contain minimal in-line documentation describing its purpose and giving a sketch of its operation. When possible example code should be given. You're always better off erring on the side of too much documentation in your comments.

We are using the Doxygen standards for code documentation. You should prefer the javadoc-style documentation blocks. [http://www.stack.nl/~dimitri/doxygen/docblocks.html Read the Doxygen documentation for details].

# File Naming Standard

Header files should use the extension “.hh”. Code files should use the extension “.cc”. Headers and code files should be named after the chief exposed class described in the file. For example, the class “Loop2D” would be defined in “Loop2D.hh” and “Loop2D.cc”. Capitalization should be the same as the class name.

# Namespaces

Be generous with namespaces. If you have a group of clearly related classes, many of them invisible outside a section of code, consider placing it in a namespace.
If you have a large number of classes beginning with the same prefix, you should definitely consider a namespace.

# Algorithmic Guidelines

Standard, well-understood algorithms should be used whenever possible. Any algorithmic implementation should include a basic set of unit tests. An emphasis should be placed on speed and robustness. Alternate algorithms that are not selected (especially ones which reduce memory usage) should be noted in the developer and code documentation for possible future implementation.
Preference should be given to parallelizable algorithms.
