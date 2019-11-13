
#include <LTemplate.h>

#include <random>
#include <cmath>

// Auxiliary class for holding the spin state.
template<typename T>
class Matrix {
    int nrows, ncols;
    T *data;
    
public:
    Matrix(int r, int c) : nrows(r), ncols(c) {
        data = new T[nrows*ncols];    
    }
    
    ~Matrix() { delete [] data; }
    
    T & operator () (int i, int j) { return data[i*ncols + j]; }
    const T & operator () (int i, int j) const { return data[i*ncols + j]; }

    T & operator [] (int i) { return data[i]; }
    const T & operator [] (int i) const { return data[i]; }
    
    int rows() const { return nrows; }
    int cols() const { return ncols; }
    int size() const { return nrows*ncols; }
    
    T *begin() { return data; }
    T *end() { return data + size(); }
};

typedef Matrix<bool> StateMatrix;

// Convert bool value to -1 or +1.
inline int spin(bool b) { return 2*b-1; }


// LTemplate class for the Ising model simulation.
class Ising {
    std::mt19937 engine;

    /* LTemplate does not currently support constructor arguments, thus Ising
     * objects must be created with an "empty" state. We indicate this using nullptr,
     * which is safe to delete in the destructor.
     *
     * There is a separate function, setState(), for assigning an actual state.
     */
    StateMatrix *state;
    
public:    
    Ising() : engine{ std::random_device()() }, state(nullptr) { }
    ~Ising() { delete state; }
    
    // Seed the random number generator.
    void seed(mint s) { engine.seed(s); }
    
    // Set the Ising state.
    void setState(mma::IntMatrixRef mat) {
        if (mat.rows() < 2 || mat.cols() < 2)
            throw mma::LibraryError("State matrix must be at least of size 2 by 2.");
        delete state;
        state = new StateMatrix(mat.rows(), mat.cols());
        std::copy(mat.begin(), mat.end(), state->begin());
    }
    
    // Retrieve the Ising state.
    mma::IntMatrixRef getState() const {
        if (! state)
            throw mma::LibraryError("State not set.");
        return mma::makeMatrix<mint>(state->rows(), state->cols(), state->begin());        
    }
    
    // Compute the magnetization value.
    mint magnetization() const {
        if (! state)
            throw mma::LibraryError("State not set.");
        int total = 0;
        for (const auto &el : *state)
            total += el;
        return 2*total - state->size();
    }
    
    // Compute the energy value.
    mint energy() const {
        if (! state)
            throw mma::LibraryError("State not set.");
        int E = 0;
        for (int i=1; i < state->rows(); ++i)
            for (int j=0; j < state->cols(); ++j)
                E += spin((*state)(i,j)) * spin((*state)(i-1,j));
        for (int i=0; i < state->rows(); ++i)
            for (int j=1; j < state->cols(); ++j)                     
                E += spin((*state)(i,j)) * spin((*state)(i,j-1));
        return -E;
    }
    
    // Simulate the Ising system for the given number of steps and the given temperature.
    void simulate(mint steps, double temp) {
        if (! state)
            throw mma::LibraryError("State not set.");
        if (temp <= 0)
            throw mma::LibraryError("Temperature must be strictly positive.");

        std::uniform_int_distribution<> randrow(0, state->rows()-1);
        std::uniform_int_distribution<> randcol(0, state->cols()-1);
        std::uniform_real_distribution<> rrand;

        for (mint i=0; i < steps; ++i) {
            // Allow for aborting the simulation every 1000 steps.
            if (i % 1000 == 0)
                mma::check_abort();

            int ri = randrow(engine);
            int rj = randcol(engine);

            int s = 0;
            if (ri > 0)               s += spin((*state)(ri-1, rj));
            if (ri < state->rows()-1) s += spin((*state)(ri+1, rj));
            if (rj > 0)               s += spin((*state)(ri, rj-1));
            if (rj < state->cols()-1) s += spin((*state)(ri, rj+1));

            int x = spin((*state)(ri,rj));

            int deltaE = 2*s*x;
            
            if (rrand(engine) < std::exp( -deltaE/temp )) {
                (*state)(ri,rj) = ! (*state)(ri,rj); // do flip
            }
        }
    }
};
