
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
     * to read the arguments, check for errors, and return a result.
     * It uses a streams-like interface.
     */
    void reverse2(MLINK link) {
        // A "context string" may optionally be passed to mlStream ("reverse2").
        // This will be prepended to any automatically generated error messages.
        mlStream ml(link, "reverse2");
        ml >> mlCheckArgs(1); // expecting a single argument

        std::string str;
        ml >> str; // read a string
        std::reverse(str.begin(), str.end()); // reverse it

        ml.newPacket(); // must call newPacket() before returning results; same as MLNewPacket()
        ml << str; // resturn a single result
    }

    void addTwo(MLINK link) {
        mlStream ml(link); // the context string may be omitted
        ml >> mlCheckArgs(2); // two arguments expected

        mint a, b;
        ml >> a >> b; // read two integers

        ml.newPacket();
        ml << a+b; // return their sum
    }

    // Compute the product and sum of the elements of a list
    void prodSum(MLINK link) {
        mlStream ml(link, "prodSum");
        ml >> mlCheckArgs(1);

        // vectors can be read directly form mlStream
        std::vector<double> vec;
        ml >> vec;

        double prod = 1.0;
        double sum = 0.0;
        for (const auto &el : vec) {
            prod *= el;
            sum += el;            
        }

        ml.newPacket();

        // To return multiple results, they must be explicitly placed into a List
        ml << mlHead("List", 2) // a list of two elements
           << prod << sum; // now put the correct number of list elements
    }

    // Compute the square root of the elements of a list.
    void sqrtList(MLINK link) {
        mlStream ml(link, "sqrtList");
        ml >> mlCheckArgs(1);

        std::vector<double> vec;
        ml >> vec;

        for (auto &el : vec)
            el = std::sqrt(el);

        ml.newPacket();
        ml << vec; // vectors can be returned directly
    }

    // Concatenate an arbitrary number of strings
    void strcat(MLINK link) {
        mlStream ml(link, "strcat");

        // We do not check for the number of arguments;
        // instead, we read all arguments into a string vector
        std::vector<std::string> vec;
        ml >> vec;

        // Concatenate the strings
        std::string result;
        for (const auto &el : vec)
            result += el;

        ml.newPacket();
        ml << result;
    }
};
