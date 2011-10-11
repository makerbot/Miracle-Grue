# Coding Guidelines

We will be using the Qt C++ coding standards for this project. See:
* [http://developer.qt.nokia.com/wiki/Qt_Coding_Style]
* [http://developer.qt.nokia.com/wiki/Coding_Conventions]

# File Naming Standard

Header files should use the extension “.hh”. Code files should use the extension “.cc”. Headers and code files should be named after the chief exposed class described in the file. For example, the class “Loop2D” would be defined in “Loop2D.hh” and “Loop2D.cc”. Capitalization should be the same as the class name.

# Namespaces

Be generous with namespaces. If you have a group of clearly related classes, many of them invisible outside a section of code, consider placing it in a namespace.
If you have a large number of classes beginning with the same prefix, you should definitely consider a namespace.

# Algorithmic Guidelines

Standard, well-understood algorithms should be used whenever possible. Any algorithmic implementation should include a basic set of unit tests. An emphasis should be placed on speed and robustness. Alternate algorithms that are not selected (especially ones which reduce memory usage) should be noted in the developer and code documentation for possible future implementation.
Preference should be given to parallelizable algorithms.
