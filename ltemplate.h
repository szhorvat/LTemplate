#ifndef LTEMPLATE_H
#define LTEMPLATE_H

#include "WolframLibrary.h"

#include <map>
#include <complex>


namespace mma {

extern WolframLibraryData libData;

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


class IntTensor {
    MTensor &t;
    mint *tensor_data;

public:
    IntTensor(MTensor &mt) : t(mt) {
        tensor_data = libData->MTensor_getIntegerData(t);
    }

    MTensor &tensor() { return t; }

    mint rank() { return libData->MTensor_getRank(t); }

    mint *data() { return tensor_data; }

    mint & operator [] (mint i) { return tensor_data[i]; }

    mint operator [] (mint i) const { return tensor_data[i]; }

    mint len() { return libData->MTensor_getFlattenedLength(t); }
};


class IntMatrix : public IntTensor {
public:

};


class RealTensor {
    MTensor &t;
    double *tensor_data;

public:
    RealTensor(MTensor &mt) : t(mt) {
        tensor_data = libData->MTensor_getRealData(t);
    }

    MTensor &tensor() { return t; }

    mint rank() { return libData->MTensor_getRank(t); }

    double *data() { return tensor_data; }

    double & operator [] (mint i) { return tensor_data[i]; }

    double operator [] (mint i) const { return tensor_data[i]; }

    mint len() { return libData->MTensor_getFlattenedLength(t); }
};


template<typename T> T getTensor(MArgument marg) { return T(MArgument_getMTensor(marg)); }

template<typename T> void setTensor(MArgument marg, T &val) { MArgument_setMTensor(marg, val.tensor()); }


} // end namespace mma

#endif // LTEMPLATE_H

