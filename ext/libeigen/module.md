# Eigen Linear Algebra Library

Eigen is a C++ template library for linear algebra: matrices, vectors, numerical
solvers, and related algorithms. It is widely used in scientific computing,
computer graphics, machine learning, and robotics due to its efficiency and
ease of use.

https://libeigen.gitlab.io

This modules provides a configuration to use Eigen with or without heap, io, and
debug functionality.

To print any Eigen values, you need to include the `modm:io` module *and* the
`modm:platform:heap` module. Unfortunately Eigen's printing functionality relies
on dynamic memory allocation.

