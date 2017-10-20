
#include <LTemplate.h>

class Sparse {
public:    
    mma::RealTensorRef explicitValues(mma::SparseArrayRef<double> sa) {
        // The Tensor returned by .explicitValues() is part of the SparseArray data structure,
        // and should not be modified. Therefore we .clone() it before returning it to Mathematica.
        return sa.explicitValues().clone();
    }

    mma::IntTensorRef explicitPositions(mma::SparseArrayRef<double> sa) {
        // Unlike .explicitValues(), the .explicitPositions() member function creates a new Tensor.
        // We are responsible for freeing this Tensor when we are done using it.
        // In this case we simply return it to the kernel without cloning.
        return sa.explicitPositions();
    }

    mma::IntTensorRef rowPointers(mma::SparseArrayRef<double> sa) {
        return sa.rowPointers();
    }

    mma::IntTensorRef columnIndices(mma::SparseArrayRef<double> sa) {
        return sa.columnIndices();
    }

    // Find the rank of a sparse array.
    mint depth(mma::SparseArrayRef<double> sa) {
        return sa.rank();
    }

    mma::IntTensorRef dimensions(mma::SparseArrayRef<double> sa) {
        return mma::makeVector<mint>(sa.rank(), sa.dimensions());
    }

    double implicitValue(mma::SparseArrayRef<double> sa) {
        return sa.implicitValue();
    }

    // Convert sparse arrays to dense ones.
    mma::RealTensorRef toDense(mma::SparseArrayRef<double> sa) {
        return sa.toTensor();
    }

    // Convert dense arrays to sparse ones.
    mma::SparseArrayRef<double> toSparse(mma::RealTensorRef t) {
        return t.toSparseArray();
    }

    // Iterate through all index-pairs of a matrix and print the corresponding element.
    void printAll(mma::SparseMatrixRef<double> sm) {
        mma::mout << "Rows: " << sm.rows() << ", Cols: " << sm.cols() << std::endl;
        for (int i=0; i < sm.rows(); ++i)
            for (int j=0; j < sm.cols(); ++j) {
                mma::mout << "(" << i << ", " << j << "): " << sm(i,j) << std::endl;
            }
    }
};
