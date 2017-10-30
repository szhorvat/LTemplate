
#include <LTemplate.h>
#include <complex>
#include <algorithm>


struct Img {

    /* Invert an Image like ColorNegate
     *
     * Demonstrated features:
     *
     *  - Iterating through all channels except the alpha channel (nonAlphaChannels)
     *  - Iterating through all pixels of a single channel linearly (pixelBegin)
     *
     */
    mma::GenericImageRef invert(mma::ImageRef<double> im) {
        // loop through all channels except the alpha channel
        for (int ch=0; ch < im.nonAlphaChannels(); ++ch)
            // loop through the pixels of the given channel
            // instead of auto it, we could use ImageRef<double>::pixel_iterator it
            for (auto it = im.pixelBegin(ch); it != im.pixelEnd(ch); ++it)
                *it = 1 - *it; // invert pixel value
        return im;
    }

    /* Blur an image up to radius 1 by averaging it with its four direct neighbours.
     * For simplicity, this implementation does not handle boundary pixels.
     *
     * Demonstrated features:
     *
     *  - Iterating through all channels except the alpha channel (if one exists)
     *  - Indexing into an image by pixel position
     *
     */
    mma::GenericImageRef blur1(mma::ImageRef<double> im) {
        // loop through all channels except the alpha channel
        for (int ch=0; ch < im.nonAlphaChannels(); ++ch)
            // loop through all non-boundary pixels using 2D indexing
            for (int i=1; i < im.rows()-1; ++i)
                for (int j=1; j < im.cols()-1; ++j)
                    // average each pixel with its four neighbours
                    im(i,j,ch) = (im(i-1, j, ch) + im(i+1, j, ch) + im(i, j-1, ch) + im(i, j+1, ch) + im(i, j, ch)) / 5.0;
        return im;
    }

    /* Retrieve image dimensions.
     *
     * Demonstrated features:
     *
     *  - GenericImageRef can refer an image with any pixel type, but it cannot index into them.
     *    Using it allows us to prevent copying the image.
     *
     */
    mma::IntTensorRef imageDimensions(mma::GenericImageRef im) {
        return mma::makeVector<mint>({im.cols(), im.rows()});
    }

    /* Creates a gradient image.
     *
     * Demonstrated features:
     *
     * - Creating new images
     * - Indexing into an image by pixel position
     *
     */
    mma::GenericImageRef gradient(mint cols, mint rows) {
        auto im = mma::makeImage<mma::im_byte_t>(cols, rows);
        for (int i=0; i < rows; ++i)
            for (int j=0; j < cols; ++j)
                im(i,j) = 255.0*j/cols;
        return im;
    }

    /* Creates a Mandelbrot set image.
     *
     * Demonstrated features:
     *
     * - Creating new images
     * - Indexing into an image by pixel position
     *
     */
    mma::GenericImageRef mandelbrot(mma::complex_t lower, mma::complex_t upper, mint width, mint iter) {
        if (! (upper.real() > lower.real() && upper.imag() > lower.imag()) )
            throw mma::LibraryError("Real and imaginary parts of 'upper' must be greater than that of 'lower'.");
        if (iter > 255) {
            iter = 255;
            mma::message("The maximum number of iterations is 255. No more than this will be tried.");
        }
        double rat = (upper.real() - lower.real()) / width;
        mint height = (upper.imag() - lower.imag()) / rat;
        auto im = mma::makeImage<mma::im_byte_t>(width, height);

        for (int i=0; i < height; ++i)
            for (int j=0; j < width; ++j) {
                mma::complex_t z = 0,  c = lower + mma::complex_t(j,i)*rat;
                int k=0;
                while (std::abs(z) < 2 && k < iter) {
                    z = z*z+c;
                    k++;
                }
                im(height - i - 1, j) = k == iter ? 0 : k+1;
            }

        return im;
    }

    /* Adjusts each channel of an image so that the values span the whole available range (defined through top()).
     *
     * Demonstrated features:
     *
     *  - Dispatching to the correct function based on the type of the image
     *  - Casting GenericImageRef to a type-specialized ImageRef
     *
     */
    mma::GenericImageRef adjust(mma::GenericImageRef im) {
        // Dispatch to the correct function based on the type of the image
        switch (im.type()) {
        case MImage_Type_Bit:
            break;
        case MImage_Type_Bit8:
            iadjust(mma::ImageRef<mma::im_byte_t>(im));
            break;
        case MImage_Type_Bit16:
            iadjust(mma::ImageRef<mma::im_bit16_t>(im));
            break;
        case MImage_Type_Real32:
            iadjust(mma::ImageRef<mma::im_real32_t>(im));
            break;
        case MImage_Type_Real:
            iadjust(mma::ImageRef<mma::im_real_t>(im));
            break;
        case MImage_Type_Undef:
            throw mma::LibraryError("Invalid image.");
        }
        return im;
    }

private:

    // Specialized version of adjust() for each image type
    template<typename T> void iadjust(mma::ImageRef<T> im) {
        for (int ch=0; ch < im.nonAlphaChannels(); ++ch) {
            auto minmax = std::minmax_element(im.pixelBegin(ch), im.pixelEnd(ch));
            T lo = *minmax.first, hi = *minmax.second;
            T diff = hi - lo;
            if (diff == 0)
                diff = 1;
            for (auto it = im.pixelBegin(ch); it != im.pixelEnd(ch); ++it)
                *it = mma::imageMax<T>() * double(*it - lo) / diff;
        }
    }
};
