
#include <LTemplate.h>

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
        mma::IntTensorRef dims = mma::makeVector<mint>(2);
        dims[0] = im.rows();
        dims[1] = im.cols();
        return dims;
    }
};
