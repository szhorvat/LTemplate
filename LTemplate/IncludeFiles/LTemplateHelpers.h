#ifndef LTEMPLATE_HELPERS_H
#define LTEMPLATE_HELPERS_H

namespace mma {

// Functions for getting and setting arguments and return values

template<typename T>
inline TensorRef<T> getTensor(MArgument marg) { return TensorRef<T>(MArgument_getMTensor(marg)); }

template<typename T>
inline void setTensor(MArgument marg, TensorRef<T> &val) { MArgument_setMTensor(marg, val.tensor()); }


inline complex_t getComplex(MArgument marg) {
    mcomplex c = MArgument_getComplex(marg);
    return complex_t(c.ri[0], c.ri[1]);
}

inline void setComplex(MArgument marg, complex_t val) {
    mcomplex *c = reinterpret_cast<mcomplex *>(&val);
    MArgument_setComplex(marg, *c);
}

} // namespace mma

#endif // LTEMPLATE_HELPERS_H
