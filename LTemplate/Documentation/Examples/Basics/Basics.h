
#include <LTemplate.h>
#include <complex>

struct Basics {
    // Add two numbers
    double add(double x, double y) {
        return x+y;
    }

    // How many times does z -> z^2 + c need to be iterated before abs(z) > 2?
    mint mandel(mma::complex_t c, mint max_iter) {
        mint iter = 0;
        mma::complex_t z = 0;
        while (std::abs(z) < 2 && iter < max_iter) {
            z = z*z + c;
            iter++;
        }
        return iter;
    }
};
