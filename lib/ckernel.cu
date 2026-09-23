#include <iostream>
#include <cuda_runtime_api.h>

__global__ void helloFromGPU2() {
    // printf("child: %i %i  %i %i\n",blockIdx.x,threadIdx.x,blockIdx.y,threadIdx.y);
}

__global__ void helloFromGPU() {
    if(threadIdx.x==0) printf("%i\n",blockIdx.x);
}

void runCudaKernel() {
    helloFromGPU<<<10000, 256>>>();
    
    cudaDeviceSynchronize();
}