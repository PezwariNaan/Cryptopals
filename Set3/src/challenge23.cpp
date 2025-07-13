#include <cstdint>
#include <utility.hpp>
#include <vector>


// **************
// MT19937 NUMBERS 
// **************

// static constexpr int n = 624; // Degree of Recurence (Size of initalised array)
// static constexpr int m = 397; // Middle Word
// static constexpr int w = 32;  // Word Length
// static constexpr int r = 31;  // Left shift
// static constexpr unsigned long a  = 0x9908b0dfUL; // Twist Coefficient 
static constexpr uint32_t u  = 11;                // Tempering Bit Shift
static constexpr uint32_t s  = 7;                 // Tempering Bit Shift 
static constexpr int t = 15;                      // Tempering Bit Shift
static constexpr int l = 18;                      // Tempering Bit Shift
static constexpr uint32_t b = 0x9d2c5680UL;            // Tempering Bit Mask
static constexpr uint32_t c = 0xefc60000UL;            // Tempering Bit Mask
// static constexpr unsigned long f = 1812433253UL;  // Initialisation Value

// static constexpr unsigned long UMASK = (0xffffffffUL << r);       // Upper Bit Mask
// static constexpr uint32_t LMASK = (0xffffffffUL >> (w - r)); // Lower Bit Mask 


uint32_t unshift_right_xor(uint32_t y, uint32_t shift) {
    uint32_t result = 0;
    for (int i = 31; i >= 0; i--) {
        uint32_t shifted = (result >> shift);
        uint32_t bit = (y ^ shifted) & (1U << i);
        result |= bit;

    }
    
    return result;
}

uint32_t unshift_left_xor_mask(uint32_t y, uint32_t shift, uint32_t mask) {
    uint32_t result = 0;
    for (int i = 0; i < 32; i++) {
        uint32_t shifted = (result << shift) & mask;
        uint32_t bit = (y ^ shifted) & (1U << i);
        result |= bit;
    }

    return result;
}

uint32_t untemper(uint32_t y) {
    uint32_t x;

    x = unshift_right_xor(y, l);
    x = unshift_left_xor_mask(x, t, c);
    x = unshift_left_xor_mask(x, s, b);
    x = unshift_right_xor(x, u);

    return x;
}

int main(void) {
    //Get 624 Outputs
    cp::MT19937 rng(456);
    std::vector<uint32_t> outputs;

    for (int i = 1; i < 625; i++) {
        uint32_t tempered = rng();
        outputs.push_back(untemper(tempered));
    }

    cp::MT19937 clone(345353453);
    clone.set_state(outputs);

    for (int i = 0; i < 1000; i++) {
        std::cout<< rng() << " : " << clone() << "\n";
    }


    return 0;
}
