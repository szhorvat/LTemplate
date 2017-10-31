
#ifndef TENSOR_H
#define TENSOR_H

#include <LTemplate.h>
#include <mlstream.h>

// Class representing a Tensor.
// It is specialized for Real and Integer tensors in RealTensor.h and IntegerTensor.h.
// This is because LTemplate requires a separate header file for each class.
template<typename T>
class Tensor {
    mma::TensorRef<T> *rt = nullptr;

    void free() {
        if (rt) {
            rt->free();
            delete rt;
        }
    }

public:
    ~Tensor() { free(); }

    // Set Tensor value; meant to be used in conjunction with Make[]
    void set(mma::TensorRef<T> t) {
        free();
        rt = new mma::TensorRef<T>(t);
    }

    // Retrieve Tensor value using MathLink
    void value(MLINK ml) {
        if (! rt)
            throw  mma::LibraryError("No tensor set.");
        mlStream link(ml);
        link >> mlCheckArgs(0);
        link.newPacket();
        link << *rt;
    }
};

#endif // TENSOR_H
