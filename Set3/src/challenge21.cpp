#include <utility.hpp>

namespace cp {
    #define n 624 // Degree of Recurence
    #define m 397 // Middle Word
    #define w 32 // Word Length
    #define r 31 // Right shift
    #define UMASK (0xffffffffUL << r) // Upper Bit Mask
    #define LMASK (0xffffffffUL >> (w - r)) // Lower Bit Mask 
    #define a 0x9908b0dfUL // Twist Coefficient 
    #define u 11 // Tempering Bit Shift
    #define s 7 // Tempering Bit Shift 
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
}

int main(void) {
    cp::initalise(cp::mt_state, 23);
    for (auto i : cp::mt_state.state_array)
        std::cout << i << '\n';
    return 0;
}    
