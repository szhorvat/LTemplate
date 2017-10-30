
#include <LTemplate.h>

struct Manager {

    // Release a VecExpr
    void releaseVecExpr(mint id) {
        int err = mma::libData->releaseManagedLibraryExpression("VecExpr", id);
        if (err)
            throw mma::LibraryError("Managed library expression does not exist.");
    }
};
