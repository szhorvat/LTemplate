
#include <LTemplate.h>

// The mlstream.h header makes it easier to handle argument passing through MathLink
#include <mlstream.h>

#include <string>
#include <vector>
#include <cmath>

struct LinkDemo {
    /* When using LinkObject based passing, the function must follow the same conventions
     * which are described in the LibraryLink tutorial:
     *
     *  - a single MLINK argument
     *  - void return type
     *
     * The arguments are passed in a list, which must be explicitly read off the link.
     * The usual MathLink functions (mathlink.h) may be used to do this.
     */
    void reverse(MLINK link) {
        int args = 1;
        if (!MLTestHeadWithArgCount(link, "List", &args))
            throw mma::LibraryError("reverse: one argument expected.");
        const char *str;
        if (!MLGetString(link, &str))
            throw mma::LibraryError("reverse: string expected.");
        std::string s = str;
        MLReleaseString(link, str);
        std::reverse(s.begin(), s.end());

        MLNewPacket(link);
        MLPutString(link, s.c_str());
    }

    /* LTemplate comes with the mlstream.h auxiliary header, which makes it easier
     * to read the argument, check for errors, and return a result.
     * It uses a streams-like interface.
     */
    void reverse2(MLINK ml) {
        // A "context string" may optionally be passed to mlStream ("reverse2").
        // This will be prepended to any automatically generated error messages.
        mlStream link(ml, "reverse2");
        link >> mlCheckArgs(1); // expecting a single argument

        std::string str;
        link >> str; // read a string
        std::reverse(str.begin(), str.end()); // reverse it

        link.newPacket(); // must call newPacket() before returning results
        link << str; // resturn a single result
    }

    void addTwo(MLINK ml) {
        mlStream link(ml); // the context string may be omitted
        link >> mlCheckArgs(2); // two arguments expected

        mint a, b;
        link >> a >> b; // read two integers

        link.newPacket();

        link << a+b;
    }

    // Compute the sum and produce of the elements of a list
    void prodSum(MLINK ml) {
        mlStream link(ml, "prodSum");
        link >> mlCheckArgs(1);

        // vectors can be read directly form mlStream
        std::vector<double> vec;
        link >> vec;

        double sum = 0.0;
        double prod = 1.0;
        for (const auto &el : vec) {
            sum += el;
            prod *= el;
        }

        link.newPacket();
        // Returning multiple results:
        link << mlHead("List", 2) /* a list of two elements */
             << sum << prod; /* now put the correct number of list elements */
    }

    // Compute the square root of the elements of a list.
    void sqrtList(MLINK ml) {
        mlStream link(ml, "sqrtList");
        link >> mlCheckArgs(1);

        std::vector<double> vec;
        link >> vec;

        for (auto &el : vec)
            el = std::sqrt(el);

        link.newPacket();
        link << vec; // vectors can be returned directly
    }
};
