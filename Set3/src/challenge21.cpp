#include <utility.hpp>
#include <chrono>

namespace cp {
    #define n 624 // Degree of Recurence (Size of initalised array)
    #define m 397 // Middle Word
    #define w 32  // Word Length
    #define r 31  // Left shift
    #define UMASK (0xffffffffUL << r) // Upper Bit Mask
    #define LMASK (0xffffffffUL >> (w - r)) // Lower Bit Mask 
    #define a 0x9908b0dfUL // Twist Coefficient 
    #define u 11 // Tempering Bit Shift
    #define s 7  // Tempering Bit Shift 
    #define t 15 // Tempering Bit Shift
    #define l 18 // Tempering Bit Shift
    #define b 0x9d2c5680UL // Tempering Bit Mask
    #define c 0xefc60000UL // Tempering Bit Mask
    #define f 1812433253UL // Initialisation Value

    struct state {
        uint32_t state_array[n];
        int state_index;
    } mt_state;

    void initalise(state &mt_state, uint32_t seed) {
        uint32_t *state_array = &(mt_state.state_array[0]);
        state_array[0] = seed;

        for (int i = 1; i < n; i++) {
            seed = f * (seed ^ (seed >> (w-2))) + i;
            state_array[i] = seed;
        }
        mt_state.state_index = 0;
    }
    
    uint32_t mt19937(state &mt_state) {
        uint32_t *state_array = &(mt_state.state_array[0]);
        
        int k = mt_state.state_index;

        int j = k - (n - 1);

        if (j < 0) j += n;
        uint32_t x = (state_array[k] & UMASK) | (state_array[j] & LMASK);
        
        uint32_t xA = x >> 1;
        if (x & 0x00000001UL) xA ^= a;

        j = k - (n - m);
        if (j < 0) j += n;

        x = state_array[j] ^ xA;
        state_array[k++] = x;

        if (k >= n) k = 0;
        mt_state.state_index = k;

        uint32_t y = x ^ (x >> u);
                 y = y ^ ((y << s) & b);
                 y = y ^ ((y << t) & c);
        uint32_t z = y ^ (y >> l);

        return z;
    }
}

int main(void) {
    cp::initalise(cp::mt_state, std::time(0));
    uint32_t random_int = cp::mt19937(cp::mt_state);
    std::cout << random_int << std::endl;
    return 0;
}    
