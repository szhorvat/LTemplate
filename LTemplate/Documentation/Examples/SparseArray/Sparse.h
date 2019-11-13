
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

    // Find the dimensions of a sparse array.
    mma::IntTensorRef dimensions(mma::SparseArrayRef<double> sa) {
        return mma::makeVector<mint>(sa.rank(), sa.dimensions());
    }

    // The implicit value of a sparse array is the value assumed for the
    // elements that were not explicitly stored.
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
    void printMatrix(mma::SparseMatrixRef<double> sm) {
        mma::mout << "Rows: " << sm.rows() << ", Cols: " << sm.cols() << std::endl;
        for (int i=0; i < sm.rows(); ++i) {
            for (int j=0; j < sm.cols(); ++j)
                mma::mout << std::setw(9) << sm(i,j);
            mma::mout << '\n';
        }
    }

    // Iterate through all explicitly stored elements of a sparse matrix.
    void printExplicit(mma::SparseMatrixRef<double> sm) {
        mma::mout << "Rows: " << sm.rows() << ", Cols: " << sm.cols() << std::endl;
        for (auto it = sm.begin(); it != sm.end(); ++it)
            mma::mout << "(" << it.row() << ", " << it.col() << ") " << *it << '\n';
    }

    // Find the smallest and largest elements of a sparse array.
    mma::RealTensorRef minmax(mma::SparseArrayRef<double> sa) {
        auto mm = std::minmax_element(sa.explicitValues().begin(), sa.explicitValues().end());
        auto res = std::minmax({*mm.first, *mm.second, sa.implicitValue()});
        return mma::makeVector<double>({res.first, res.second});
    }


    // CREATING SPARSE ARRAYS FROM SCRATCH

    // Create a 5-by-6 sparse array by explicitly specifying the stored elements.
    mma::SparseMatrixRef<double> createMatrixDemo() {
        // The positions matrix must me n-by-2 in size.
        // Positions indices start at 1, as in Mathematica, not in 0, as in C++.
        auto pos = mma::makeMatrix<mint>({{1,1}, {3,4}, {2,3}});

        // The explicit value array.
        auto vals = mma::makeVector<double>({1.0, 2.5, 3.6});

        auto sm = mma::makeSparseMatrix(pos, vals, 5, 6);

        // The Tensors we used for constructing the sparse matrix must be explicitly freed.
        pos.free();
        vals.free();

        return sm;
    }

    // Create a 3D sparse array, analogously to createMatrixDemo() above.
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

    // Create a 3-by-4 all-zero SparseArray.
    mma::SparseMatrixRef<double> createZeroMatrixDemo() {
        auto pos  = mma::makeMatrix<mint>(0,2);
        auto vals = mma::makeVector<double>(0);

        auto sa = mma::makeSparseMatrix(pos, vals, 3, 4);

        pos.free();
        vals.free();

        return sa;
    }


    // RECOMPUTE STRUCTURE

    // Recompute the sparse structure to eliminate unnecessary explicit values.
    mma::SparseArrayRef<double> resetImplicitValue(mma::SparseArrayRef<double> sa) {
        return sa.resetImplicitValue();
    }

    // Recompute the sparse structure based on a user-specified implicit value.
    mma::SparseArrayRef<double> newImplicitValue(mma::SparseArrayRef<double> sa, double iv) {
        return sa.resetImplicitValue(iv);
    }


    // MODIFY SPARSE ARRAY

    mma::SparseArrayRef<double> modify(mma::SparseArrayRef<double> sa) {
        auto fun = [] (double x) { return x*x + 1; }; // x -> x*x+1
        for (auto &el : sa.explicitValues())
            el = fun(el);
        sa.implicitValue() = fun(sa.implicitValue());

        /* Optionally, use
         *
         *     return sa.resetImplicitValue();
         *
         * to eliminate any redundant explicit values. This is unnecessary with the
         * transformation x -> x*x+1 as it leaves no real value unchanged.
         */

        return sa;
    }
};
