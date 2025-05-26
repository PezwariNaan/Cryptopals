#include <random>
#include <thread>
#include <utility.hpp> 

int main(void) {
    std::random_device rd;
    std::cout << "Start Time: " << time(NULL) << '\n';
    int wait_period = rd() % 5;
    std::cout << "Wait Time: " << wait_period << '\n';

    std::this_thread::sleep_for(std::chrono::seconds(wait_period));
    std::cout << "Seed Time: " << time(NULL) << '\n';

    cp::MT19937 rng(time(NULL));
    std::cout << "RNG: " << rng()  << '\n';

    return 0;
}