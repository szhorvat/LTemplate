#include <LTemplate.h>

// Let Armadillo print to Mathematica notebooks by default
#define ARMA_COUT_STREAM mma::mout
#define ARMA_CERR_STREAM mma::mout
#include <armadillo>

// Armadillo stores matrices in column major order, while Mathematica uses row-major order.
// It is thus fastest to convert an MTensor to its transposed Armadillo equivalent.

// Convert from Mathematica matrix to the Armadillo matrix without copying any data.
template<typename T>
arma::Mat<T> toArmaTransposed(mma::MatrixRef<T> m) {
    // wrap MTensor with arma::mat without copying
    // See the "Advanced Constructors" documentation http://arma.sourceforge.net/docs.html#Mat
    return arma::Mat<T>(m.data(), m.cols(), m.rows(), false /* do not copy */, false /* until resized */);
}

// Convert from Armadillo matrix to Mathematica matrix.
// This time the data must be copied.
template<typename T>
mma::TensorRef<T> fromArmaTransposed(arma::Mat<T> m) {
    return mma::makeMatrix<T>(m.n_cols, m.n_rows, m.memptr());
}

// Convert from Mathematica vector to Armadillo column vector without copying
template<typename T>
arma::Col<T> toArmaVec(mma::TensorRef<T> v) {
    return arma::Col<T>(v.data(), v.size(), false /* do not copy */, false /* until resized */);
}

// Convert from Armadillo column vector to Mathematica vector with copying
template<typename T>
mma::TensorRef<T> fromArmaVec(arma::Col<T> v) {
    return mma::makeVector<T>(v.size(), v.memptr());
}

// Convert from Mathematica sparse matrix to Armadillo sparse matrix
template<typename T>
arma::SpMat<T> toArmaSparseTransposed(mma::SparseMatrixRef<T> sm) {
    return arma::SpMat<T>(
            // column indices and row pointers must be explicitly converted
            // because Armadillo uses unsigned integers while Mathematica uses signed ones
            arma::conv_to<arma::uvec>::from(toArmaVec(sm.columnIndices())) - 1, // convert to 0-based indices; Mathematica uses 1-based ones.
            arma::conv_to<arma::uvec>::from(toArmaVec(sm.rowPointers())),
            toArmaVec(sm.explicitValues()), sm.cols(), sm.rows()
           );
}

class Arma {
public:
    mma::RealMatrixRef inv(mma::RealMatrixRef mat) {
        arma::mat m = toArmaTransposed(mat);
        return fromArmaTransposed<double>(arma::inv(m));
    }

    mma::ComplexTensorRef eigs(mma::SparseMatrixRef<double> sm, mint k) {
        return fromArmaVec<mma::complex_t>(arma::eigs_gen(toArmaSparseTransposed(sm), k));
    }
        
    void print(mma::RealMatrixRef mat) {
        mma::mout << toArmaTransposed(mat).t();
    }

    void printSparse(mma::SparseMatrixRef<double> sm) {
        mma::mout << toArmaSparseTransposed(sm);
    }
};
