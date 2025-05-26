#include <cstdint>
#include <iostream>

namespace cp {
    class MT19937 {
    private:
        static constexpr int n = 624; // Degree of Recurence (Size of initalised array)
        static constexpr int m = 397; // Middle Word
        static constexpr int w = 32;  // Word Length
        static constexpr int r = 31;  // Left shift
        static constexpr unsigned long a  = 0x9908b0dfUL; // Twist Coefficient 
        static constexpr uint32_t u  = 11;                // Tempering Bit Shift
        static constexpr uint32_t s  = 7;                 // Tempering Bit Shift 
        static constexpr int t = 15;                      // Tempering Bit Shift
        static constexpr int l = 18;                      // Tempering Bit Shift
        static constexpr int b = 0x9d2c5680UL;            // Tempering Bit Mask
        static constexpr int c = 0xefc60000UL;            // Tempering Bit Mask
        static constexpr unsigned long f = 1812433253UL;  // Initialisation Value

        static constexpr unsigned long UMASK = (0xffffffffUL << r);       // Upper Bit Mask
        static constexpr uint32_t LMASK = (0xffffffffUL >> (w - r)); // Lower Bit Mask 

        uint32_t state_array[n];
        int state_index = n + 1;
        uint32_t seed_value = 1234U;
        
        void initalise( uint32_t seed) {
            state_array[0] = seed;

            for (int i = 1; i < n; ++i) {
                seed = f * (seed ^ (seed >> (w - 2))) + i;
                state_array[i] = seed;
            }
            state_index = 0;
        }

        void twist() {
            for (int i = 0; i < n; ++i) {
                int j = (i + 1) % n;
                uint32_t x = (state_array[i] & UMASK )| (state_array[j] & LMASK);
                uint32_t xA = x >> 1;
                if (x & 0x1) xA ^= a;
                int k = (i + m) % n;
                state_array[i] = state_array[k] ^ xA;
            }
            state_index = 0;
        }
    
    public:
        MT19937(uint32_t seed = 1234U) {
            initalise(seed);
        }

        uint32_t operator() () {
            if (state_index >= n) twist();

            uint32_t x = state_array[state_index++];
            x ^= (x >> u);
            x ^= (x << s) & b;
            x ^= (x << t) & c;
            x ^= (x >> l);
            return x;
        }
    };
}

int main(void) {
    cp::MT19937 rng(456);

    for (int i = 0; i < 10; ++i) {
        std::cout << rng() << std::endl;
    }
    
    return 0;
}    
