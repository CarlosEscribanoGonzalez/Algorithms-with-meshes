
## Description

This is the documentation for the code given as initial helpers for the
FMFIG triangle meshes homework. This documentation is not intended to be
read in a determinate order, but used as help and fast guide of available
[classes](hierarchy.html) and [members](functions.html).

This library implement the storage and some usual operations for generic triangle meshes.

It is [C++ a header-only library][1]. You do not need to precompile or link to anything,
just include headers (e.g. `#include <SimpleMesh.hpp>`) and run.
Storage is based in STL [vectors][2] as containers of coordinates and indexes of triangles.

Some exercises use the [Eigen](http://eigen.tuxfamily.org) library for easy matrix
storage and operation. Eigen provides a good support to sparse matrix operations
(as product or solving) that are automatically parallelized in modern computers.

[1]: https://stackoverflow.com/questions/12671383/benefits-of-header-only-libraries#12673877
[2]: http://www.cplusplus.com/reference/vector/vector/
