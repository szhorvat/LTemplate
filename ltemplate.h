#ifndef LTEMPLATE_H
#define LTEMPLATE_H

#include "mathlink.h"
#include "WolframLibrary.h"

#include <map>
#include <vector>
#include <complex>


namespace mma {

extern WolframLibraryData libData;

struct LibraryError {
    const char *message;
    int errcode;

    LibraryError(const char *m = NULL, int err = LIBRARY_FUNCTION_ERROR) : message(m), errcode(err) { }
};


void check_abort() {
    if (libData->AbortQ())
        throw LibraryError();
}


enum MessageType { INFO, WARNING, ERROR };


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


class TensorRef {
    MTensor t; // reminder: MTensor is a pointer type
    const std::vector<mint> dims;

public:
    TensorRef(const MTensor &mt) :
        t(mt),
        dims(libData->MTensor_getDimensions(t), libData->MTensor_getDimensions(t) + libData->MTensor_getRank(t))
    {
        // empty
    }

    MTensor tensor() { return t; }

    mint rank() const { return libData->MTensor_getRank(t); }
    mint length() const { return libData->MTensor_getFlattenedLength(t); }

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
};


class IntTensorRef : public TensorRef {
    mint *tensor_data;

public:
    IntTensorRef(const TensorRef &t) :
        TensorRef(t),
        tensor_data(libData->MTensor_getIntegerData(tensor()))
    {
        // empty
    }

    mint *data() { return tensor_data; }
    mint & operator [] (mint i) { return tensor_data[i]; }
    const mint & operator [] (mint i) const { return tensor_data[i]; }

    mint *begin() { return data(); }
    mint *end() { return begin() + length(); }
};


class IntMatrixRef : public IntTensorRef {
    mint nrows, ncols;
public:
    IntMatrixRef(const IntTensorRef &t) : IntTensorRef(t) {
        if (rank() != 2)
            throw LibraryError("IntMatrixRef: Matrix expected.");
        nrows = dimensions()[0];
        ncols = dimensions()[1];
    }

    mint rows() const { return nrows; }
    mint cols() const { return ncols; }

    mint & operator () (mint i, mint j) { return (*this)[nrows*i + j]; }
    const mint & operator () (mint i, mint j) const { return (*this)[nrows*i + j]; }
};


IntMatrixRef makeIntMatrix(mint nrow, mint ncol) {
    MTensor t = NULL;
    mint dims[2];
    dims[0] = nrow;
    dims[1] = ncol;
    int err = libData->MTensor_new(MType_Integer, 2, dims, &t);
    if (err)
        throw LibraryError("MTensor_new() failed.", err);
    return IntTensorRef(t);
}


class RealTensorRef : public TensorRef {
    double *tensor_data;

public:
    RealTensorRef(const TensorRef &t) :
        TensorRef(t),
        tensor_data(libData->MTensor_getRealData(tensor()))
    {
        // empty
    }

    double *data() { return tensor_data; }
    double & operator [] (mint i) { return tensor_data[i]; }
    const double & operator [] (mint i) const { return tensor_data[i]; }

    double *begin() { return data(); }
    double *end() { return begin() + length(); }
};


class RealMatrixRef : public RealTensorRef {
    mint nrows, ncols;
public:
    RealMatrixRef(const RealTensorRef &t) : RealTensorRef(t) {
        if (rank() != 2)
            throw LibraryError("RealMatrixRef: Matrix expected.");
        nrows = dimensions()[0];
        ncols = dimensions()[1];
    }

    mint rows() const { return nrows; }
    mint cols() const { return ncols; }

    double & operator () (mint i, mint j) { return (*this)[nrows*i + j]; }
    const double & operator () (mint i, mint j) const { return (*this)[nrows*i + j]; }
};



// Functions for getting and setting arguments and return values

TensorRef getTensor(MArgument marg) { return TensorRef(MArgument_getMTensor(marg)); }

void setTensor(MArgument marg, TensorRef &val) { MArgument_setMTensor(marg, val.tensor()); }

std::complex<double> getComplex(MArgument marg) {
    mcomplex c = MArgument_getComplex(marg);
    return std::complex<double>(c.ri[0], c.ri[1]);
}

void setComplex(MArgument marg, std::complex<double> val) {
    mcomplex c;
    c.ri[0] = val.real();
    c.ri[1] = val.imag();
    MArgument_setComplex(marg, c);
}


} // end namespace mma

#endif // LTEMPLATE_H

