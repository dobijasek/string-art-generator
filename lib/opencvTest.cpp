#include <iostream>

void runCudaKernel();

int main() {
    std::cout << "Running CUDA kernel...\n";
    runCudaKernel();
    std::cout << "CUDA kernel finished.\n";
    return 0;
}