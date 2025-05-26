#include <iostream> 
#include <utility.hpp> 

int main(void) {
    cp::MT19937 rng(92);
    std::cout << rng() << std::endl;
    return 0;
}