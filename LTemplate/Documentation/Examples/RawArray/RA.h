
#include <LTemplate.h>

class RA {
public:
    // Store the data from a Real Tensor in a Byte RawArray and shuffle the byte storage order
    mma::RawArrayRef<uint8_t> shuffle(mma::RealTensorRef t) {
        const uint8_t *data = reinterpret_cast<uint8_t *>(t.data());
        auto ra = mma::makeRawVector<uint8_t>(t.size() * sizeof(double));
        for (int i=0; i < sizeof(double); ++i)
            for (int j=0; j < t.size(); ++j)
                ra[i*t.size() + j] = data[j*sizeof(double) + i];
        return ra;
    }

    // Reverse the transformation done by shuffle()
    mma::RealTensorRef deshuffle(mma::RawArrayRef<uint8_t> ra) {
        if (ra.size() % sizeof(double) != 0)
            throw mma::LibraryError("Input size must be a multiple of 8.");
        auto t = mma::makeVector<double>(ra.size() / sizeof(double));
        uint8_t *data = reinterpret_cast<uint8_t *>(t.data());
        for (int i=0; i < sizeof(double); ++i)
            for (int j=0; j < t.size(); ++j)
                data[j*sizeof(double) + i] = ra[i*t.size() + j];
        return t;
    }
};
