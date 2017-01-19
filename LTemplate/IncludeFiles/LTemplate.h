/*
 * Copyright (c) 2017 Szabolcs Horvát.
 *
 * See the file LICENSE.txt for copying permission.
 */

/**
 * \mainpage
 *
 * This is Doxygen-generated documentation for the C++ interface of the LTemplate _Mathematica_ package.
 *
 * For the latest version of the package go to https://github.com/szhorvat/LTemplate
 *
 * See `LTemplateTutorial.nb` for an introduction and additional documentation.
 */

#ifndef LTEMPLATE_H
#define LTEMPLATE_H

/** \file
 * \brief Include this header before classes to be used with the LTemplate _Mathematica_ package.
 *
 */

#include "mathlink.h"
#include "WolframLibrary.h"
#include "WolframSparseLibrary.h"

// mathlink.h defines P. It has a high potential for conflict, so we undefine it.
// It is normally only used with .tm files and it is not needed for LTemplate.
#undef P

#include <string>
#include <ostream>
#include <sstream>
#include <complex>

namespace mma {

/// Global `WolframLibraryData` object for accessing the LibraryLink API.
extern WolframLibraryData libData;

/// Complex number type compatible with LibraryLink
typedef std::complex<double> complex_t;


/// For use in the mma::message() function.
enum MessageType { M_INFO, M_WARNING, M_ERROR, M_ASSERT };


/** \brief Issue a _Mathematica_ message.
 *  \param msg the text of the message
 *  \param type determines the message tag which will be used
 */
void message(const char *msg, MessageType type = M_INFO);

inline void message(std::string msg, MessageType type = M_INFO) { message(msg.c_str(), type); }


/// Call _Mathematica_'s `Print[]`.
inline void print(const char *msg) {
    if (libData->AbortQ())
        return; // trying to use the MathLink connection during an abort appears to break it

    MLINK link = libData->getMathLink(libData);
    MLPutFunction(link, "EvaluatePacket", 1);
        MLPutFunction(link, "Print", 1);
            MLPutString(link, msg);
    libData->processMathLink(link);
    int pkt = MLNextPacket(link);
    if (pkt == RETURNPKT)
        MLNewPacket(link);
}

/// Call _Mathematica_'s `Print[]`, `std::string` argument version.
inline void print(std::string msg) { print(msg.c_str()); }


/// Can be used to output with _Mathematica_'s `Print[]` in a manner similar to `std::cout`. The stream _must_ be flushed (`std::endl` or `std::flush`) to trigger printing.
extern std::ostream mout;


/** \brief Throwing this type returns to _Mathematica_ immediately.
 *  \param s reported in _Mathematica_ as `LTemplate::error`; gets copied.
 *  \param err used as the LibraryFunction exit code.
 */
class LibraryError {
    const std::string msg;
    const bool has_msg;
    const int err_code;

public:
    explicit LibraryError(int err = LIBRARY_FUNCTION_ERROR) : has_msg(false), err_code(err) { }
    LibraryError(std::string s, int err = LIBRARY_FUNCTION_ERROR) : msg(s), has_msg(true), err_code(err) { }

    const std::string &message() const { return msg; }
    bool has_message() const { return has_msg; }
    int error_code() const { return err_code; }

    void report() const {
        if (has_msg)
            mma::message(msg, M_ERROR);
    }
};


#ifdef NDEBUG
#define massert(condition) ((void)0)
#else
/// Replacement for the standard `assert` macro. Instead of aborting the process, it throws a mma::LibraryError
#define massert(condition) (void)(((condition) || mma::detail::massert_impl(#condition, __FILE__, __LINE__)), 0)
#endif

namespace detail { // private
    inline bool massert_impl(const char *cond, const char *file, int line) {
        std::ostringstream msg;
        msg << cond << ", file " << file << ", line " << line;
        message(msg.str(), M_ASSERT);
        throw LibraryError();
    }
} // end namespace detail


/// Check for and honour user aborts.
inline void check_abort() {
    if (libData->AbortQ())
        throw LibraryError();
}


namespace detail { // private
    template<typename T> T * getData(MTensor t);

    template<> inline mint * getData(MTensor t) { return libData->MTensor_getIntegerData(t); }
    template<> inline double * getData(MTensor t) { return libData->MTensor_getRealData(t); }
    template<> inline complex_t * getData(MTensor t) { return reinterpret_cast< complex_t * >( libData->MTensor_getComplexData(t) ); }

    // copy data from column major format to row major format
    template<typename T, typename U>
    inline void transposedCopy(const T *from, U *to, mint nrow, mint ncol) {
        for (mint i=0; i < ncol; ++i)
            for (mint j=0; j < nrow; ++j)
                to[i + j*ncol] = from[j + i*nrow];
    }
} // end namespace detail


/** \brief Wrapper class for `MTensor` pointers
 *  \param T must be `mint`, `double` or `mma::complex_t`.
 *
 * Note that this class only holds a reference to an `MTensor`.  Multiple \ref TensorRef objects
 * may point to the same `MTensor`.
 */
template<typename T>
class TensorRef {
    const MTensor t; // reminder: MTensor is a pointer type
    T * const tensor_data;
    const mint len;

public:
    TensorRef(const MTensor &mt) :
        t(mt),
        tensor_data(detail::getData<T>(t)),
        len(libData->MTensor_getFlattenedLength(t))
    {
        // empty
    }

    /// Returns the referenced \c MTensor
    MTensor tensor() { return t; }

    /// Returns the rank of the tensor, same as \c MTensor_getRank
    mint rank() const { return libData->MTensor_getRank(t); }

    /// Returns the number of elements in the tensor, same as \c MTensor_getFlattenedLength
    mint length() const { return len; }

    /// Returns the number of elements in the tensor, synonym of \ref length()
    mint size() const { return length(); }

    /// Frees the referenced \c MTensor, same as \c MTensor_free
    /**
     * Warning: multiple \ref TensorRef objects may reference the same \c MTensor.
     * Freeing the \c MTensor invalidates all references to it.
     */
    void free() { libData->MTensor_free(t); }

    void disown() { libData->MTensor_disown(t); }   
    void disownAll() { libData->MTensor_disownAll(t); }

    mint shareCount() const { return libData->MTensor_shareCount(t); }

    /// Clones the referenced \c MTensor and returns a new \ref TensorRef object pointing to it
    TensorRef clone() const {
        MTensor c = NULL;
        int err = libData->MTensor_clone(t, &c);
        if (err) throw LibraryError("MTensor_clone() failed", err);
        return c;
    }

    const mint *dimensions() const { return libData->MTensor_getDimensions(t); }

    /// Returns a pointer to the underlying storage of the corresponding \c MTensor
    T *data() { return tensor_data; }

    T & operator [] (mint i) { return tensor_data[i]; }
    const T & operator [] (mint i) const { return tensor_data[i]; }

    T *begin() { return data(); }
    T *end() { return begin() + length(); }
};

typedef TensorRef<mint>      IntTensorRef;
typedef TensorRef<double>    RealTensorRef;
typedef TensorRef<complex_t> ComplexTensorRef;


/// Wrapper class for `MTensor` pointers to rank 2 tensors
/**
 * Remember that \c MTensor stores data in row-major order.
 */
template<typename T>
class MatrixRef : public TensorRef<T> {
    mint nrows, ncols;

public:
    MatrixRef(const TensorRef<T> &tr) : TensorRef<T>(tr)
    {
        if (TensorRef<T>::rank() != 2)
            throw LibraryError("MatrixRef: Matrix expected.");
        const mint *dims = TensorRef<T>::dimensions();
        nrows = dims[0];
        ncols = dims[1];
    }

    /// Number of rows in the matrix
    mint rows() const { return nrows; }

    /// Number of columns in the matrix
    mint cols() const { return ncols; }

    /// Index into a matrix using row and column indices
    T & operator () (mint i, mint j) { return (*this)[ncols*i + j]; }

    /// Index into a constant matrix using row and column indices
    const T & operator () (mint i, mint j) const { return (*this)[ncols*i + j]; }
};

typedef MatrixRef<mint>       IntMatrixRef;
typedef MatrixRef<double>     RealMatrixRef;
typedef MatrixRef<complex_t>  ComplexMatrixRef;


/// Wrapper class for `MTensor` pointers to rank 3 tensors
template<typename T>
class CubeRef : public TensorRef<T> {
    mint nrows, ncols, nslices;

public:
    CubeRef(const TensorRef<T> &tr) : TensorRef<T>(tr)
    {
        if (TensorRef<T>::rank() != 3)
            throw LibraryError("CubeRef: Rank-3 tensor expected.");
        const mint *dims = TensorRef<T>::dimensions();
        nrows = dims[0];
        ncols = dims[1];
        nslices = dims[2];
    }

    /// Number of rows in the cube
    mint rows() const { return nrows; }

    /// Number of columns in the cube
    mint cols() const { return ncols; }

    /// Number of slices in the cube
    mint slices() const { return nslices; }

    /// Index into a cube using row, column and slice indices
    T & operator () (mint i, mint j, mint k) { return (*this)[nslices*ncols*i + nslices*j + k]; }

    /// Index into a constant cube using row, column and slice indices
    const T & operator () (mint i, mint j, mint k) const { return (*this)[nslices*ncols*i + nslices*j + k]; }
};

typedef CubeRef<mint>       IntCubeRef;
typedef CubeRef<double>     RealCubeRef;
typedef CubeRef<complex_t>  ComplexCubeRef;


namespace detail { // private
    template<typename T> int libraryType();

    template<> inline int libraryType<mint>()      { return MType_Integer; }
    template<> inline int libraryType<double>()    { return MType_Real; }
    template<> inline int libraryType<complex_t>() { return MType_Complex; }
} // end namespace detail

/// Creates a rank 3 tensor of the given dimensions
template<typename T>
inline CubeRef<T> makeCube(mint nrow, mint ncol, mint nslice) {
    MTensor t = NULL;
    mint dims[3];
    dims[0] = nrow;
    dims[1] = ncol;
    dims[2] = nslice;
    int err = libData->MTensor_new(detail::libraryType<T>(), 3, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


/// Creates a matrix (rank 2 tensor) of the given dimensions
template<typename T>
inline MatrixRef<T> makeMatrix(mint nrow, mint ncol) {
    MTensor t = NULL;
    mint dims[2];
    dims[0] = nrow;
    dims[1] = ncol;
    int err = libData->MTensor_new(detail::libraryType<T>(), 2, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


/// Creates a matrix (rank 2 tensor) of the given dimensions from a row-major storage C array
template<typename T, typename U>
inline MatrixRef<T> makeMatrix(mint nrow, mint ncol, const U *data) {
    TensorRef<T> t = makeMatrix<T>(nrow, ncol);
    std::copy(data, data + nrow*ncol, t.begin());
    return t;
}


/// Creates a matrix of the given dimensions from a column-major storage C array
template<typename T, typename U>
inline MatrixRef<T> makeMatrixTransposed(mint nrow, mint ncol, const U *data) {
    TensorRef<T> t = makeMatrix<T>(nrow, ncol);
    detail::transposedCopy(data, t.data(), nrow, ncol);
    return t;
}


/// Creates a vector (rank 1 tensor) of the given length
template<typename T>
inline TensorRef<T> makeVector(mint len) {
    MTensor t = NULL;
    mint dims[1];
    dims[0] = len;
    int err = libData->MTensor_new(detail::libraryType<T>(), 1, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


/// Creates a vector of the given length from a C array
template<typename T, typename U>
inline TensorRef<T> makeVector(mint len, const U *data) {
    TensorRef<T> t = makeVector<T>(len);
    std::copy(data, data+len, t.begin());
    return t;
}


/// Wrapper class for `MSparseArray` pointers
template<typename T>
class SparseArrayRef {
    const MSparseArray sa; // reminder: MSparseArray is a pointer type

public:
    SparseArrayRef(const MSparseArray &msa) : sa(msa) { /* empty */ }

    MSparseArray sparseArray() { return sa; }

    mint rank() const { return libData->sparseLibraryFunctions->MSparseArray_getRank(sa); }

    void free() { libData->sparseLibraryFunctions->MSparseArray_free(sa); }
    void disown() { libData->sparseLibraryFunctions->MSparseArray_disown(sa); }
    void disownAll() { libData->sparseLibraryFunctions->MSparseArray_disownAll(sa); }

    mint shareCount() const { return libData->sparseLibraryFunctions->MSparseArray_shareCount(sa); }

    SparseArrayRef clone() const {
        MSparseArray c = NULL;
        int err = libData->sparseLibraryFunctions->MSparseArray_clone(sa, &c);
        if (err) throw LibraryError("MSparseArray_clone() failed", err);
        return c;
    }

    /// Returns the indices of non-default (i.e. explicit) values in the sparse array.
    /// The result `MTensor` is part of the `MSparseArray` data structure and will be destroyed at the same time with it.
    /// Clone it before returning it to the kernel.
    IntTensorRef explicitPositions() const {
        MTensor mt = NULL;
        libData->sparseLibraryFunctions->MSparseArray_getExplicitPositions(sa, &mt);
        return IntTensorRef(mt);
    }

    /// True if the sparse array has explicit values.  Pattern arrays do not have explicit values.
    bool explicitValuesQ() const {
        return libData->sparseLibraryFunctions->MSparseArray_getExplicitValues(sa) != NULL;
    }

    /// Returns the explicit values in the sparse array as an `MTensor`.
    /// The result `MTensor` is part of the `MSparseArray` data structure and will be destroyed at the same time with it.
    /// Clone it before returning it to the kernel.
    TensorRef<T> explicitValues() const {
        MTensor *mt = libData->sparseLibraryFunctions->MSparseArray_getExplicitValues(sa);
        if (*mt == NULL)
            throw LibraryError("SparseArrayRef::explicitValues() called on pattern array");
        return TensorRef<T>(*mt);
    }

    T implicitValue() const {
        MTensor *mt = libData->sparseLibraryFunctions->MSparseArray_getImplicitValue(sa);
        return (TensorRef<T>(*mt).data())[0];
    }
};


/// Convenience function for disowning `const char *` strings.
inline void disownString(const char *str) {
    libData->UTF8String_disown(const_cast<char *>(str));
}


} // end namespace mma

#endif // LTEMPLATE_H

