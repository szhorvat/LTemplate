
#include <LTemplate.h>
#include <sstream>
#include <vector>

class Printing {
    mint positiveValue = 1;

public:
    /* Throw an mma::LibraryError exception in case of errors.
     * The associated message will be printed directly in the notebook.
     */
    void set(mint x) {
        if (x <= 0)
            throw mma::LibraryError("The value must be positive.");
        positiveValue = x;
    }

    // Print a C string or an std::string
    void hello() const {
        mma::print("Hello world!");
    }

    /* The most convenient way to show a complex message is to use the
     * stream interface mma::out.
     *
     * Note that the output is only printed to the notebook when the
     * mma::mout buffer is flushed. This is most easily accomplished
     * by inserting an std::endl into the stream. Flushing effectively
     * starts a new notebook cells.
     *
     * Due to how Mathematica's Print[] works, a newline is always
     * inserted at the end of the output, whether or not it was explicitly
     * sent to mma::mout.
     *
     * The buffer is always flushed when the library function exits,
     * this the std::endl below is not strictly required.
     */
    void printValue() const {
        mma::mout << "My value is " << positiveValue << "!" << std::endl;
    }

    // Issue a message in Mathematica as LTemplate::info
    void message(const char *msg) const {
        mma::message(msg);
        mma::disownString(msg); // release the string sent by the kernel
    }

    // Issue an error message in Mathematica as LTemplate::error
    void errorMessage(const char *msg) const {
        mma::message(msg, mma::M_ERROR);
        mma::disownString(msg); // release the string sent by the kernel
    }

    // Use std::ostringstream to build a more complex message
    void messageValue() const {
        std::ostringstream msg;
        msg << "The value is " << positiveValue << ".";
        mma::message(msg.str());
    }

    /* The massert macro can be used in place of the standard C assert macro.
     * Instead of aborting the entire kernel process, it will simply return
     * from the library, and print the assertion failure message in the notebook.
     */
    void assertDemo() const {
        massert(42 == 137);
    }

    // This function demonstrates catching arbitrary exceptions
    void exception() const {
        std::vector<int> vec = {1,2,3};
        vec.at(4); // out-of-bounds error, throws std::out_of_range exception
    }
};
