#### Version 0.6

 - Support for Mathematica 12.0 
 - Support for `NumericArray`
 - Incompatible change: `ByteArray`s are now mapped to `NumericArrayRef` instead of `RawArrayRef`
 - Added `LTemplatePrivateNoCompile.m` as an alternative to `LTemplatePrivate.m`. It is to be used when embedding LTemplate in another package and only _loading support_ is needed (not _compilation support_). It does not load CCompilerDriver, which would be slow on Windows in some Mathematica versions.
 - Minor non-critical bug fixes and performance improvements
 - Documentation fixes

#### Version 0.5.4

 - Bug fix for `SparseArrayRef::iterator`
 - Fix getting integral vectors with `mlstream`
 - Documentation fixes

#### Version 0.5.3

 - Bug fix: in some cases, the row index was not correctly computed when using `SparseArrayRef::iterator`

#### Version 0.5.2

 - Bug fixes
 - Support for ByteArray type specification

#### Version 0.5.1

 - `GenericImageRef` split into `GenericImageRef` / `GenericImage3DRef`
 - Bug fixes
 - Expanded documentation and examples

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
