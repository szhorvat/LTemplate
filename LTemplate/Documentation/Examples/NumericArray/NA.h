
#include <LTemplate.h>

class NA {
public:
    // Store the data from a Real Tensor in a Byte NumericArray and shuffle the byte storage order
    mma::NumericArrayRef<uint8_t> shuffle(mma::RealTensorRef t) {
        const uint8_t *data = reinterpret_cast<uint8_t *>(t.data());
        auto na = mma::makeNumericVector<uint8_t>(t.size() * sizeof(double));
        for (int i=0; i < sizeof(double); ++i)
            for (int j=0; j < t.size(); ++j)
                na[i*t.size() + j] = data[j*sizeof(double) + i];
        return na;
    }

    // Reverse the transformation done by shuffle()
    mma::RealTensorRef deshuffle(mma::NumericArrayRef<uint8_t> na) {
        if (na.size() % sizeof(double) != 0)
            throw mma::LibraryError("Input size must be a multiple of 8.");
        auto t = mma::makeVector<double>(na.size() / sizeof(double));
        uint8_t *data = reinterpret_cast<uint8_t *>(t.data());
        for (int i=0; i < sizeof(double); ++i)
            for (int j=0; j < t.size(); ++j)
                data[j*sizeof(double) + i] = na[i*t.size() + j];
        return t;
    }

    // Convert an arbitrary NumericArray into a Byte NumericArray. Use the default conversion method,
    // which is to clip and round values not representable as the target type.
    mma::NumericArrayRef<uint8_t> clipToBytes(mma::GenericNumericArrayRef na) {
        return na.convertTo<uint8_t>();
    }

    // Convert an arbitrary type NumericArray into a Byte NumericArray. Use the Check conversion method,
    // which will verify that all values fit in the target type, and throw a LibraryError exception if they don't.
    mma::NumericArrayRef<uint8_t> safeConvertToBytes(mma::GenericNumericArrayRef na) {
        return na.convertTo<uint8_t>(mma::GenericNumericArrayRef::Check);
    }

    // Convert an arbitrary type NumericArray into a Real32 NumericArray
    // using the chosen conversion method and tolerance.
    mma::NumericArrayRef<float> convertToFloat(mma::GenericNumericArrayRef na, mint method, double tolerance) {
        return na.convertTo<float>(mma::GenericNumericArrayRef::ConversionMethod(method), tolerance);
    }

    // Convert an arbitrary type NumericArray into a Integer16 NumericArray
    // using the chosen conversion method and tolerance.
    mma::NumericArrayRef<int16_t> convertToInteger16(mma::GenericNumericArrayRef na, mint method, double tolerance) {
        return na.convertTo<int16_t>(mma::GenericNumericArrayRef::ConversionMethod(method), tolerance);
    }
};
