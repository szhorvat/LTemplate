#### Version 0.5

 - Expanded SparseArray support, added `SparseMatrixRef`.
 - Added experimental RawArray support.
 - Added experimental Image and Image3D support.
 - Uniform type specifications through `LType`, e.g. `LType[List, Real]` or `LType[SparseArray, Integer, 2]`. Added `NormalizeTemplate` which converts to this form. The usual LibraryLink compatible type specifications also work.
 - More functions for creating Tensors.
 - Class instances can now be retrieved by ID; access to the class-collection.
 - Greatly expanded documentation with many examples.
 - Several other small improvements.

**Incompatible changes:**

 - A compiler with C++11 support is now required. GCC 4.8 and MSVC 2015 both work.
 - `CompileTempate` automatically selects C++11 using a compiler flag. If you need a newer standard, such as C++14, you must now select this explicitly using a flag similar to `-std=c++14`, *even if C++14 is the default for your compiler*.
 - `CubeRef` is now indexed as (slices, rows, columns) for consistency with Image. In practice, this means that `operator ()` has not changed, but `slices()`, `rows()` and `cols()` now return the tensor size along the 1st, 2nd and 3rd index, respectively.
