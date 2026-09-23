cmake_minimum_required(VERSION 3.18)

project(LINEDRAWER LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(USE_CUDA OFF)

if (MSVC)
    enable_language(CUDA)
    set(CUDA_TOOLKIT_ROOT_DIR "C:/Program Files/NVIDIA GPU Computing Toolkit/CUDA/v12.6")
    set(CUDA_INCLUDE_DIRS "${CUDA_TOOLKIT_ROOT_DIR}/include")
    set(CUDA_LIB_DIRS "${CUDA_TOOLKIT_ROOT_DIR}/lib/x64")

    set(CMAKE_CUDA_STANDARD 11)
    set(CMAKE_CUDA_STANDARD_REQUIRED ON)
    set(CMAKE_CUDA_ARCHITECTURES "native")

    message(STATUS "Found CUDA at ${CUDA_TOOLKIT_ROOT_DIR}")

    set(USE_CUDA ON)

    list(APPEND CUDA_LIBS cudart cublas curand)
    list(APPEND CUDA_PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
    set(CUDA_SOURCES lib/ckernel.cu)

    add_definitions(-DUSE_CUDA)
endif()

find_package(OpenCV REQUIRED)
include_directories(${OpenCV_INCLUDE_DIRS})

message(STATUS "OpenCV_INCLUDE_DIRS: ${OpenCV_INCLUDE_DIRS}")
message(STATUS "compiler: C<${CMAKE_C_COMPILER_ID}> CXX<${CMAKE_CXX_COMPILER_ID}> CUDA<${CMAKE_CUDA_COMPILER_ID}>")

include_directories(include)

set(SOURCE_FILES
    lib/nn.cpp
    lib/param.cpp
    lib/canvas.cpp
    lib/wire.cpp
    lib/weight.cpp
    lib/line.cpp
    lib/pair.cpp
    lib/nail.cpp
    lib/pool.cpp
    lib/drawLines.cpp
    lib/drawLines2.cpp
    lib/core.cpp
    lib/gui.cpp
    lib/progress_bar.cpp
    lib/find.cpp
    lib/best.cpp
    lib/score.cpp
    lib/score2.cpp
    lib/layer.cpp
    lib/graph.cpp
    lib/composite.cpp
    lib/CIEDE2000.cpp
    lib/Instrumentor.cpp
    lib/lineAA.cpp
    lib/histogram.cpp
    lib/testQueue.cpp
    lib/paerson.cpp
    lib/paerson2.cpp
)

add_executable(RunLineDraw lib/RunLineDraw.cpp ${SOURCE_FILES} ${CUDA_SOURCES})
if (USE_CUDA)
    set_target_properties(RunLineDraw PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()
target_link_libraries(RunLineDraw ${OpenCV_LIBS} ${CUDA_LIBS})

add_executable(opencvTest lib/opencvTest.cpp ${CUDA_SOURCES})
if (USE_CUDA)
    set_target_properties(opencvTest PROPERTIES CUDA_SEPARABLE_COMPILATION ON)
endif()
target_link_libraries(opencvTest ${OpenCV_LIBS} ${CUDA_LIBS})

add_executable(lineAA lib/lineAA_test.cpp lib/lineAA.cpp lib/pool.cpp)
target_link_libraries(lineAA ${OpenCV_LIBS})

add_executable(layerTest lib/layer_test.cpp lib/layer.cpp)

message(STATUS "CMAKE_CXX_COMPILER_ID: ${CMAKE_CXX_COMPILER_ID}")

# --- Nowoczesna i bezpieczna konfiguracja flag ---
foreach(target RunLineDraw opencvTest lineAA layerTest)
    if (CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        target_compile_options(${target} PRIVATE
            -w
            -O3
            -mavx
            -mavx2
            -mfma
        )
    elseif(MSVC)
        target_compile_options(${target} PRIVATE /w /arch:AVX)
    endif()
endforeach()
