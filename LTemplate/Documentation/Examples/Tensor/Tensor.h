
#include <LTemplate.h>
#include <random>
#include <numeric>
#include <iomanip>

class Tensor {
public:
    // A trivial function that returns a real array as-is
    mma::RealTensorRef identity(mma::RealTensorRef t) { return t; }

    // Return the depth of a real array
    mint depth(mma::RealTensorRef t) {
        return t.rank();
    }

    // Return the dimensions of a real array. Demonstrates creating new arrays.
    mma::IntTensorRef dimensions(mma::RealTensorRef t) {
        return mma::makeVector<mint>(t.rank(), t.dimensions());
    }

    // Sum the elements of an arbitrary dimensional real array.
    // Demonstrates linear iteration through array elements.
    double sum(mma::RealTensorRef t) {
        double s = 0.0;
        for (const auto &el : t)
            s += el;
        return s;
    }

    // Print the elements of a vector along with their indices.
    // Demonstrates direct indexing.
    void printVector(mma::RealTensorRef v) {
        for (int i=0; i < v.size(); ++i)
            mma::mout << std::setw(4) << i+1 << ": " << std::setw(9) << v[i] << '\n';
    }

    // Print the elements of a matrix.
    // Demonstrates matrix indexing.
    void printMatrix(mma::RealMatrixRef m) {
        for (int i=0; i < m.rows(); ++i) {
            for (int j=0; j < m.cols(); ++j)
                mma::mout << std::setw(9) << m(i,j);
            mma::mout << '\n';
        }
    }

    // Direct vector creation using initializer list.
    mma::RealTensorRef createVector() {
        return mma::makeVector<double>({1,2,3});
    }

    // Direct matrix creatin using initialier list.
    mma::RealMatrixRef createMatrix() {
        return mma::makeMatrix<double>({{1,2},{3,4}});
    }

    // Direct cube (rank-3 Tensor) creation using initializer list.
    mma::RealCubeRef createCube() {
        return mma::makeCube<double>({ {{1,2},{3,4}}, {{5,6},{7,8}} });
    }

    // Create a high-dimensional Tensor, specifying its dimensions directly.
    mma::RealTensorRef ones2345() {
        auto t = mma::makeTensor<double>({2,3,4,5});
        std::fill(t.begin(), t.end(), 1);
        return t;
    }

    // Create an arbitrary dimensional array of zeros.
    mma::RealTensorRef zeros(mma::IntTensorRef dims) {
        auto t = mma::makeTensor<double>(dims.length(), dims.data());
        std::fill(t.begin(), t.end(), 0);
        return t;
    }

    // Create an integer range.
    mma::IntTensorRef range(mint i, mint j) {
        if (j < i)
            return mma::makeVector<mint>(0);
        else {
            auto vec = mma::makeVector<mint>(j-i+1);
            std::iota(vec.begin(), vec.end(), i);
            return vec;
        }
    }

    // Create n random numbers between lower and upper.
    mma::RealTensorRef randomReal(mint n, double lower, double upper) {
        std::random_device r;
        std::default_random_engine eng(r());
        std::uniform_real_distribution<double> dist(lower, upper);

        auto vec = mma::makeVector<double>(n);
        for (auto &el : vec)
            el = dist(eng);

        return vec;
    }
};
