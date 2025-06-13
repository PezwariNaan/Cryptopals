#include <cassert>
#include <cstdint>
#include <random>
#include <sys/types.h>
#include <thread>
#include <utility.hpp> 

struct output {
    uint32_t result;
    std::time_t seed;
};

output get_output(void) {
    output answer;
    std::random_device rd;
    std::cout << "Start Time: " << time(NULL) << '\n';
    int wait_period = rd() % 100;
    std::cout << "Wait Time: " << wait_period << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(wait_period));
    std::time_t seed_time = time(NULL);

    cp::MT19937 rng(time(NULL));
    
    uint32_t random_number = rng();
    answer.result = random_number;
    answer.seed = seed_time;

    return answer;
}

std::time_t guess_seed(uint32_t random_number) {
    std::time_t now = std::time(nullptr);
    for (std::time_t t = now - 100; t <= now; ++t) {
        cp::MT19937 rng(t);
        if (rng() == random_number) {
            return t;
        }
    }

    return 0;
}

int main(void) {
    output random_number = get_output();

    std::time_t seed = guess_seed(random_number.result);

    assert(seed == random_number.seed);
    std::cout << "Seed: " << seed << std::endl;

    return 0;
}
