#include <LTemplate.h>

// Let Armadillo print to Mathematica notebooks by default
#define ARMA_COUT_STREAM mma::mout
#define ARMA_CERR_STREAM mma::mout
#include <armadillo>

// We start by defining conversion functions to/from Armadillo's basic datatypes:
// matrices, column vectors and sparse matrices.

// Armadillo stores matrices in column major order, while Mathematica uses row-major order.
// It is thus fastest to convert an MTensor to/from its transposed Armadillo equivalent.

// Convert from Mathematica matrix to the transposed Armadillo equivalent without copying any data.
template<typename T>
arma::Mat<T> toArmaTransposed(mma::MatrixRef<T> m) {
    // wrap MTensor with arma::mat without copying
    // See the "Advanced Constructors" documentation http://arma.sourceforge.net/docs.html#Mat
    return arma::Mat<T>(m.data(), m.cols(), m.rows(), false /* do not copy */, false /* until resized */);
}

// Convert from Armadillo matrix to transposed Mathematica matrix.
// This time the data must be copied.
template<typename T>
mma::TensorRef<T> fromArmaTransposed(const arma::Mat<T> &m) {
    return mma::makeMatrix<T>(m.n_cols, m.n_rows, m.memptr());
}

// Convert from Mathematica vector to Armadillo column vector without copying.
template<typename T>
arma::Col<T> toArmaVec(mma::TensorRef<T> v) {
    return arma::Col<T>(v.data(), v.size(), false /* do not copy */, false /* until resized */);
}

// Convert from Armadillo column vector to Mathematica vector with copying.
template<typename T>
mma::TensorRef<T> fromArmaVec(const arma::Col<T> &v) {
    return mma::makeVector<T>(v.size(), v.memptr());
}

// Convert from Mathematica sparse matrix to Armadillo sparse matrix.
template<typename T>
arma::SpMat<T> toArmaSparseTransposed(mma::SparseMatrixRef<T> sm) {
    return arma::SpMat<T>(
            // Column indices and row pointers must be explicitly converted
            // because Armadillo uses unsigned integers while Mathematica uses signed ones.
            arma::conv_to<arma::uvec>::from(toArmaVec(sm.columnIndices())) - 1, // convert to 0-based indices; Mathematica uses 1-based ones.
            arma::conv_to<arma::uvec>::from(toArmaVec(sm.rowPointers())),
            toArmaVec(sm.explicitValues()),
            sm.cols(), sm.rows()
           );
}

// Convert from Armadillo sparse matrix to Mathematica SparseArray.
template<typename T>
mma::SparseMatrixRef<T> fromArmaSparse(const arma::SpMat<T> &am) {
    // there are am.n_nonzero explicitly stored elements in am
    auto pos  = mma::makeMatrix<mint>(am.n_nonzero, 2); // positions array
    auto vals = mma::makeVector<double>(am.n_nonzero);  // values array

    // iterate through all explicitly stored elements in the Armadillo sparse matrix and
    // fill out the positions and values arrays to be used in the creation of a Mathematica SparseArray
    mint i = 0;
    for (typename arma::SpMat<T>::const_iterator it = am.begin();
         it != am.end();
         ++it, ++i)
    {
        vals[i] = *it;
        pos(i,0) = it.row() + 1; // convert 0-based index to 1-based
        pos(i,1) = it.col() + 1;
    }

    auto mm = mma::makeSparseMatrix(pos, vals, am.n_rows, am.n_cols);

    pos.free();
    vals.free();

    return mm;
}

class Arma {
public:
    // Matrix inverse
    mma::RealMatrixRef inv(mma::RealMatrixRef mat) {
        arma::mat m = toArmaTransposed(mat);
        return fromArmaTransposed<double>(arma::inv(m));
    }

    // The first k complex eigenvalues of a sparse matrix
    mma::ComplexTensorRef eigs(mma::SparseMatrixRef<double> sm, mint k) {
        return fromArmaVec<mma::complex_t>(arma::eigs_gen(toArmaSparseTransposed(sm), k));
    }

    // Random sparse matrix
    mma::SparseMatrixRef<double> sprandu(mint i, mint j, double dens) {
        return fromArmaSparse(arma::sprandu<arma::sp_mat>(i, j, dens));
    }

    // Solve a linear equation
    mma::RealTensorRef solve(mma::RealMatrixRef mat, mma::RealTensorRef vec) {
        return fromArmaVec<double>(
                    arma::solve(toArmaTransposed(mat).t(), // transpose back to match mat
                    toArmaVec(vec))
                );
    }
        
    // Print an Armadillo matrix
    void print(mma::RealMatrixRef mat) {
        mma::mout << toArmaTransposed(mat).t();
    }

    // Print an Armadillo sparse matrix
    void printSparse(mma::SparseMatrixRef<double> sm) {
        mma::mout << toArmaSparseTransposed(sm);
    }
};
