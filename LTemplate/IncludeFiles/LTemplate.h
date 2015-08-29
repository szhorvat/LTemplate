/** \file
 *  Include this header before classes to be used with the LTemplate Mathematica package
 */

#ifndef LTEMPLATE_H
#define LTEMPLATE_H

#include "mathlink.h"
#include "WolframLibrary.h"

#include <map>
#include <vector>
#include <complex>

#ifndef LTEMPLATE_CONTEXT
#define LTEMPLATE_CONTEXT /* empty */
#endif

namespace mma {

/// Global WolframLibraryData object for accessing the LibraryLink API.
extern WolframLibraryData libData;

typedef std::complex<double> complex_t;


/** Throwing this returns to Mathematica immediately.
 *  \param reported in Mathematica as LTemplate::error
 *  \param used as the LibraryFunction exit code.
 */
struct LibraryError {
    const char *message;
    int errcode;

    LibraryError(const char *m = NULL, int err = LIBRARY_FUNCTION_ERROR) : message(m), errcode(err) { }
};


/// Check for and honour user aborts.
inline void check_abort() {
    if (libData->AbortQ())
        throw LibraryError();
}


/// For use in the message() function.
enum MessageType { M_INFO, M_WARNING, M_ERROR, M_ASSERT };


/** Issue a Mathematica message
 * \param msg the text of the message
 * \param type determines the message tag which will be used
 */
inline void message(const char *msg, MessageType type = M_INFO) {
    if (msg == NULL)
        return;

    const char *tag;
    switch (type) {
    case M_ERROR:
        tag = "error";
        break;
    case M_WARNING:
        tag = "warning";
        break;
    case M_ASSERT:
        tag = "assert";
        break;
    case M_INFO:
    default:
        tag = "info";
    }

    MLINK link = libData->getMathLink(libData);
    MLPutFunction(link, "EvaluatePacket", 1);
        MLPutFunction(link, "Message", 2);
            MLPutFunction(link, "MessageName", 2);
                MLPutSymbol(link, LTEMPLATE_CONTEXT "LTemplate");
                MLPutString(link, tag);
            MLPutString(link, msg);
    libData->processMathLink(link);
    int pkt = MLNextPacket(link);
    if (pkt == RETURNPKT)
        MLNewPacket(link);
}


/// Call Mathematica's Print[].
inline void print(const char *msg) {
    MLINK link = libData->getMathLink(libData);
    MLPutFunction(link, "EvaluatePacket", 1);
        MLPutFunction(link, "Print", 1);
            MLPutString(link, msg);
    libData->processMathLink(link);
    int pkt = MLNextPacket(link);
    if (pkt == RETURNPKT)
        MLNewPacket(link);
}


#ifdef NDEBUG
#define massert(condition) ((void)0)
#else
#define massert(condition) (void)(((condition) || mma::_massert_impl(#condition)), 0)
#endif

inline bool _massert_impl(const char *cond) {
    message(cond, M_ASSERT);
    throw LibraryError();
}


template<typename T> T * getData(MTensor t);

template<> inline mint * getData(MTensor t) { return libData->MTensor_getIntegerData(t); }
template<> inline double * getData(MTensor t) { return libData->MTensor_getRealData(t); }
template<> inline complex_t * getData(MTensor t) { return reinterpret_cast< complex_t * >( libData->MTensor_getComplexData(t) ); }


/// Wrapper class for MTensor pointers
template<typename T>
class TensorRef {
    MTensor t; // reminder: MTensor is a pointer type    
    const mint len;
    const std::vector<mint> dims;
    T *tensor_data;

public:
    TensorRef(const MTensor &mt) :
        t(mt),
        tensor_data(getData<T>(t)),
        len(libData->MTensor_getFlattenedLength(t)),
        dims(libData->MTensor_getDimensions(t), libData->MTensor_getDimensions(t) + libData->MTensor_getRank(t))
    {
        // empty
    }

    MTensor tensor() { return t; }

    mint rank() const { return libData->MTensor_getRank(t); }
    mint length() const { return len; }

    void free() { libData->MTensor_free(t); }
    void disown() { libData->MTensor_disown(t); }
    void disownAll() { libData->MTensor_disownAll(t); }

    mint shareCount() const { return libData->MTensor_shareCount(t); }

    TensorRef clone() {
        MTensor c = NULL;
        int err = libData->MTensor_clone(t, &c);
        if (err) throw LibraryError("MTensor_clone() failed", err);
        return c;
    }

    const std::vector<mint> & dimensions() const { return dims; }

    T *data() { return tensor_data; }
    T & operator [] (mint i) { return tensor_data[i]; }
    const T & operator [] (mint i) const { return tensor_data[i]; }

    T *begin() { return data(); }
    T *end() { return begin() + length(); }
};

typedef TensorRef<mint>      IntTensorRef;
typedef TensorRef<double>    RealTensorRef;
typedef TensorRef<complex_t> ComplexTensorRef;


/// Wrapper class for MTensor pointers to rank 2 tensors
template<typename T>
class MatrixRef : public TensorRef<T> {
    mint nrows, ncols;

public:
    MatrixRef(const TensorRef<T> &tr) : TensorRef<T>(tr)
    {
        if (TensorRef<T>::rank() != 2)
            throw LibraryError("MatrixRef: Matrix expected.");
        nrows = TensorRef<T>::dimensions()[0];
        ncols = TensorRef<T>::dimensions()[1];
    }

    mint rows() const { return nrows; }
    mint cols() const { return ncols; }

    T & operator () (mint i, mint j) { return (*this)[nrows*i + j]; }
    const T & operator () (mint i, mint j) const { return (*this)[nrows*i + j]; }
};

typedef MatrixRef<mint>       IntMatrixRef;
typedef MatrixRef<double>     RealMatrixRef;
typedef MatrixRef<complex_t>  ComplexMatrixRef;


/// Wrapper class for MTensor pointers to rank 2 tensors
template<typename T>
class CubeRef : public TensorRef<T> {
    mint nrows, ncols, nslices;

public:
    CubeRef(const TensorRef<T> &tr) : TensorRef<T>(tr)
    {
        if (TensorRef<T>::rank() != 3)
            throw LibraryError("CubeRef: Rank-3 tensor expected.");
        nrows = TensorRef<T>::dimensions()[0];
        ncols = TensorRef<T>::dimensions()[1];
        nslices = TensorRef<T>::dimensions()()[2];
    }

    mint rows() const { return nrows; }
    mint cols() const { return ncols; }
    mint slices() const { return nslices; }

    T & operator () (mint i, mint j, mint k) { return (*this)[nrows*ncols*i + ncols*j + k]; }
    const T & operator () (mint i, mint j, mint k) const { return (*this)[nrows*ncols*i + ncols*j + k]; }
};

typedef CubeRef<mint>       IntCubeRef;
typedef CubeRef<double>     RealCubeRef;
typedef CubeRef<complex_t>  ComplexCubeRef;


template<typename T> int libraryType();

template<> inline int libraryType<mint>()      { return MType_Integer; }
template<> inline int libraryType<double>()    { return MType_Real; }
template<> inline int libraryType<complex_t>() { return MType_Complex; }

/// Creates a rank 3 tensor of the given dimensions
template<typename T>
inline CubeRef<T> makeCube(mint nrow, mint ncol, mint nslice) {
    MTensor t = NULL;
    mint dims[3];
    dims[0] = nrow;
    dims[1] = ncol;
    dims[2] = nslice;
    int err = libData->MTensor_new(libraryType<T>(), 3, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


/// Creates a rank 2 tensor of the given dimensions
template<typename T>
inline MatrixRef<T> makeMatrix(mint nrow, mint ncol) {
    MTensor t = NULL;
    mint dims[2];
    dims[0] = nrow;
    dims[1] = ncol;
    int err = libData->MTensor_new(libraryType<T>(), 2, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


/// Creates a vector (rank 1 tensor) of the given length
template<typename T>
inline TensorRef<T> makeVector(mint len) {
    MTensor t = NULL;
    mint dims[1];
    dims[0] = len;
    int err = libData->MTensor_new(libraryType<T>(), 1, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


/// Convenience function for disowning const char * strings.
inline void disownString(const char *str) {
    libData->UTF8String_disown(const_cast<char *>(str));
}


} // end namespace mma

#endif // LTEMPLATE_H

