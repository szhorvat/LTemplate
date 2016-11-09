/*
 * Copyright (c) 2016 Szabolcs Horvát.
 *
 * See the file LICENSE.txt for copying permission.
 */

#ifndef LTEMPLATE_HELPERS_H
#define LTEMPLATE_HELPERS_H

#include "LTemplate.h"
#include <algorithm>
#include <vector>
#include <map>

namespace mma {
namespace detail {

// Functions for getting and setting arguments and return values

template<typename T>
inline TensorRef<T> getTensor(MArgument marg) { return TensorRef<T>(MArgument_getMTensor(marg)); }

template<typename T>
inline void setTensor(MArgument marg, TensorRef<T> &val) { MArgument_setMTensor(marg, val.tensor()); }

template<typename T>
inline SparseArrayRef<T> getSparseArray(MArgument marg) { return MArgument_getMSparseArray(marg); }

template<typename T>
inline void setSparseArray(MArgument marg, SparseArrayRef<T> &val) { MArgument_setMSparseArray(marg, val.sparseArray()); }

inline complex_t getComplex(MArgument marg) {
    mcomplex c = MArgument_getComplex(marg);
    return complex_t(c.ri[0], c.ri[1]);
}

inline void setComplex(MArgument marg, complex_t val) {
    mcomplex *c = reinterpret_cast<mcomplex *>(&val);
    MArgument_setComplex(marg, *c);
}


inline const char *getString(MArgument marg) {
    return const_cast<const char *>(MArgument_getUTF8String(marg));
}

inline void setString(MArgument marg, const char *val) {
    MArgument_setUTF8String(marg, const_cast<char *>(val));
}


template<typename Collection>
inline IntTensorRef get_collection(const Collection &collection) {
    IntTensorRef ids = makeVector<mint>(collection.size());

    typename Collection::const_iterator i = collection.begin();
    mint *j = ids.begin();
    for (; i != collection.end(); ++i, ++j)
        *j = i->first;

    return ids;
}


template<typename T>
class getObject {
    std::map<mint, T *> &collection;
public:
    explicit getObject(std::map<mint, T *> &coll) : collection(coll) { }
    T & operator () (MArgument marg) { return *(collection[MArgument_getInteger(marg)]); }
};


// Underlying stream buffer for mma::mout
class MBuffer : public std::streambuf {
    std::vector<char_type> buf;

public:
    MBuffer(std::size_t buf_size = 4096) : buf(buf_size + 1) {
        setp(&buf.front(), &buf.back());
    }

protected:
    int sync();
    int_type overflow(int_type ch);

private:
    MBuffer(const MBuffer &);
    MBuffer & operator = (const MBuffer &);
};


// Used with RAII to ensure that mma::mout is flushed before the exit of any top-level function.
struct MOutFlushGuard {
    ~MOutFlushGuard() { mout.flush(); }
};


// Handles unknown exceptions in top-level functions.
void handleUnknownException(const char *what, const char *funname) {
    std::ostringstream msg;
    msg << "Unknown exception caught in "
        << funname
        << ". The library may be in an inconsistent state. It is recommended that you restart the kernel now to avoid instability.";
    if (what)
        msg << '\n' << what;
    message(msg.str(), M_ERROR);
}


} // namespace detail
} // namespace mma

#endif // LTEMPLATE_HELPERS_H
