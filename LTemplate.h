#ifndef LTEMPLATE_H
#define LTEMPLATE_H

#include "mathlink.h"
#include "WolframLibrary.h"

#include <map>
#include <vector>
#include <complex>


namespace mma {

extern WolframLibraryData libData;

typedef std::complex<double> complex_t;


struct LibraryError {
    const char *message;
    int errcode;

    LibraryError(const char *m = NULL, int err = LIBRARY_FUNCTION_ERROR) : message(m), errcode(err) { }
};


void check_abort() {
    if (libData->AbortQ())
        throw LibraryError();
}


enum MessageType { INFO, WARNING, ERROR, ASSERT };


void message(const char *msg, MessageType type = INFO) {
    if (msg == NULL)
        return;

    const char *tag;
    switch (type) {
    case ERROR:
        tag = "error";
        break;
    case WARNING:
        tag = "warning";
        break;
    case ASSERT:
        tag = "assert";
        break;
    case INFO:
    default:
        tag = "info";
    }

    MLINK link = libData->getMathLink(libData);
    MLPutFunction(link, "EvaluatePacket", 1);
        MLPutFunction(link, "Message", 2);
            MLPutFunction(link, "MessageName", 2);
                MLPutSymbol(link, "LTemplate");
                MLPutString(link, tag);
            MLPutString(link, msg);
    libData->processMathLink(link);
    int pkt = MLNextPacket(link);
    if (pkt == RETURNPKT)
        MLNewPacket(link);
}


void print(const char *msg) {
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

bool _massert_impl(const char *cond) {
    message(cond, ASSERT);
    throw LibraryError();
}


template<typename T> T * getData(MTensor t);

template<> mint * getData(MTensor t) { return libData->MTensor_getIntegerData(t); }
template<> double * getData(MTensor t) { return libData->MTensor_getRealData(t); }
template<> complex_t * getData(MTensor t) { return reinterpret_cast< complex_t * >( libData->MTensor_getComplexData(t) ); }


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


template<typename T>
CubeRef<T> makeCube(mint nrow, mint ncol, mint nslice) {
    MTensor t = NULL;
    mint dims[3];
    dims[0] = nrow;
    dims[1] = ncol;
    dims[2] = nslice;
    int err = libData->MTensor_new(MType_Integer, 3, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


template<typename T>
MatrixRef<T> makeMatrix(mint nrow, mint ncol) {
    MTensor t = NULL;
    mint dims[2];
    dims[0] = nrow;
    dims[1] = ncol;
    int err = libData->MTensor_new(MType_Integer, 2, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


template<typename T>
MatrixRef<T> makeVector(mint len) {
    MTensor t = NULL;
    mint dims[1];
    dims[0] = len;
    int err = libData->MTensor_new(MType_Integer, 1, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return TensorRef<T>(t);
}


// Functions for getting and setting arguments and return values

template<typename T>
TensorRef<T> getTensor(MArgument marg) { return TensorRef<T>(MArgument_getMTensor(marg)); }

template<typename T>
void setTensor(MArgument marg, TensorRef<T> &val) { MArgument_setMTensor(marg, val.tensor()); }


complex_t getComplex(MArgument marg) {
    mcomplex c = MArgument_getComplex(marg);
    return complex_t(c.ri[0], c.ri[1]);
}

void setComplex(MArgument marg, complex_t val) {
    mcomplex *c = reinterpret_cast<mcomplex *>(&val);
    MArgument_setComplex(marg, *c);
}


} // end namespace mma

#endif // LTEMPLATE_H

