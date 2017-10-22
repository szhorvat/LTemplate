
#include <LTemplate.h>
#include <iomanip>

class Sparse {
public:
    // ACCESSING PROPERTIES

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
        return sa.rowPointers().clone();
    }

    mma::IntTensorRef columnIndices(mma::SparseArrayRef<double> sa) {
        return sa.columnIndices().clone();
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


    // DENSE <-> SPARSE CONVERSIONS

    // Convert sparse arrays to dense ones.
    mma::RealTensorRef toDense(mma::SparseArrayRef<double> sa) {
        return sa.toTensor();
    }

    // Convert dense arrays to sparse ones.
    mma::SparseArrayRef<double> toSparse(mma::RealTensorRef t) {
        return t.toSparseArray();
    }


    // ELEMENT ACCESS

    // Iterate through all index-pairs of a matrix and print the corresponding element.
    void printAll(mma::SparseMatrixRef<double> sm) {
        mma::mout << "Rows: " << sm.rows() << ", Cols: " << sm.cols() << std::endl;
        for (int i=0; i < sm.rows(); ++i) {
            for (int j=0; j < sm.cols(); ++j)
                mma::mout << std::setw(9) << sm(i,j);
            mma::mout << '\n';
        }
        mma::mout << std::endl;
    }


    // CREATING SPARSE ARRAYS FROM SCRATCH

    // Create a 5-by-6 sparse array by explicitly specifying the stored elements
    mma::SparseMatrixRef<double> createMatrixDemo() {
        // The positions matrix must me n-by-2 in size
        // Positions indices start at 1, as in Mathematica, not in 0, as in C++
        auto pos = mma::makeMatrix<mint>({{1,1}, {3,4}, {2,3}});

        // The explicit value array
        auto vals = mma::makeVector<double>({1.0, 2.5, 3.6});

        auto sm = mma::makeSparseMatrix(pos, vals, 5, 6);

        // The Tensors we used for constructing the sparse matrix must be explicitly freed
        pos.free();
        vals.free();

        return sm;
    }

    // Create a 3D sparse array, analogously to createMatrixDemo()
    mma::SparseArrayRef<double> createCubeDemo() {
        auto pos  = mma::makeMatrix<mint>({{1,2,1}, {3,2,1}});
        auto vals = mma::makeVector<double>({1.5,3.2});
        auto dims = mma::makeVector<mint>({3,3,3});

        auto sa = mma::makeSparseArray(pos, vals, dims);

        pos.free();
        vals.free();
        dims.free();

        return sa;
    }

    // Create a 3-by-4 all-zero SparseArray
    mma::SparseMatrixRef<double> createZeroMatrixDemo() {
        auto pos  = mma::makeMatrix<mint>(0,2);
        auto vals = mma::makeVector<double>(0);

        auto sa = mma::makeSparseMatrix(pos, vals, 3, 4);

        pos.free();
        vals.free();

        return sa;
    }
};
