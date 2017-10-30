
#include <LTemplate.h>
#include <vector>
#include <numeric>
#include <algorithm>

// A class for arbitrary length real vectors
class VecExpr {
    std::vector<double> vec;

public:
    // Set the value of the vector
    void set(mma::RealTensorRef values) {
        vec.assign(values.begin(), values.end());
    }

    // Retrieve the value of the vector
    mma::RealTensorRef value() const {
        return mma::makeVector<double>(vec.size(), vec.data());
    }

    // Inner product with another vector
    // The integer library expression ID passed to the function is
    // automatically translated to an object reference (VecExpr &)
    double inner(const VecExpr &v) const {
        if (vec.size() != v.vec.size())
            throw mma::LibraryError("VecExprs are of inconsistent sizes.");
        return std::inner_product(vec.begin(), vec.end(), v.vec.begin(), 0);
    }

    // Set this vector to the sum of an arbitrary number of other vectors
    // Currently there is no feature in LTemplate to expose multiple IDs as object references.
    // Instead, IDs can be translated manually using mma::getInstance().
    void setToSum(mma::IntTensorRef ids) {
        if (ids.size() == 0)
            throw mma::LibraryError("There must be at least one VecExpr to sum.");

        // Get the size of the first vector, and resize this vector
        const size_t len = mma::getInstance<VecExpr>(ids[0]).vec.size();
        vec.clear();
        vec.resize(len, 0.0);

        // Iterate through all library expression IDs
        for (const auto &id : ids) {
            VecExpr &ve = mma::getInstance<VecExpr>(id);
            if (ve.vec.size() != len)
                throw mma::LibraryError("Mismatch between VecExpr sizes");
            for (auto it1 = vec.begin(), it2 = ve.vec.begin();
                 it1 != vec.end();
                 ++it1, ++it2)
            {
                *it1 += *it2;
            }
        }
    }
};
