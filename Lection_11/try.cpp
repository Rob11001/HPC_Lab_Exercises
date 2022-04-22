#include <CL/cl.h>
#include <CL/sycl.hpp>
#include <iostream>
using namespace sycl;

int main() {
    // Create queue on whatever default device that the implementation
    // chooses. Implicit use of the default_selector. 
    
    queue Q { default_selector() };
    std::cout << "Selected device: " <<
    Q.get_device().get_info<info::device::name>() << "\n";

    return 0;
}
