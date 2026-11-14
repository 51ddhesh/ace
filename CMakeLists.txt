cmake_minimum_required(VERSION 3.25)
project(Ace LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

option(ENABLE_ASAN "Enable Address and UB Sanitizers" OFF)
option(ENABLE_TSAN "Enable Thread Sanitizer" OFF)
option(ENABLE_PGO_GEN "Build with PGO instrumentation generation" OFF)
option(ENABLE_PGO_USE "Build using generated PGO profile data" OFF)
option(BUILD_CUDA_DATALOADER "Build CUDA sm_89 NNUE dataloader extension" ON)

set(ENGINE_SOURCES
    src/main.cpp
)

add_executable(ace ${ENGINE_SOURCES})
target_include_directories(ace PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_compile_options(ace PRIVATE
    -Wall -Wextra -Wpedantic -Werror
    -fno-exceptions -fno-rtti
    -march=native -mtune=native -mbmi2 -mavx2
)

if(CMAKE_BUILD_TYPE STREQUAL "Release")
    include(CheckIPOSupported)
    check_ipo_supported(RESULT lto_supported OUTPUT lto_error)
    if(lto_supported)
        set_property(TARGET ace PROPERTY INTERPROCEDURAL_OPTIMIZATION TRUE)
    endif()
    target_compile_options(ace PRIVATE -O3 -DNDEBUG -funroll-loops)
endif()

if(ENABLE_ASAN)
    target_compile_options(ace PRIVATE -O1 -g -fsanitize=address,undefined -fno-omit-frame-pointer)
    target_link_options(ace PRIVATE -fsanitize=address,undefined)
endif()

if(ENABLE_TSAN)
    target_compile_options(ace PRIVATE -O1 -g -fsanitize=thread)
    target_link_options(ace PRIVATE -fsanitize=thread)
endif()

# Profile-Guided Optimization (PGO)
if(ENABLE_PGO_GEN)
    target_compile_options(ace PRIVATE -fprofile-generate=${CMAKE_BINARY_DIR}/pgo)
    target_link_options(ace PRIVATE -fprofile-generate=${CMAKE_BINARY_DIR}/pgo)
elseif(ENABLE_PGO_USE)
    target_compile_options(ace PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/pgo -fprofile-correction)
    target_link_options(ace PRIVATE -fprofile-use=${CMAKE_BINARY_DIR}/pgo -fprofile-correction)
endif()

if(BUILD_CUDA_DATALOADER)
    include(CheckLanguage)
    check_language(CUDA)
    if(CMAKE_CUDA_COMPILER)
        enable_language(CUDA)
        set(CMAKE_CUDA_STANDARD 20)
        set(CMAKE_CUDA_STANDARD_REQUIRED ON)
        set(CMAKE_CUDA_ARCHITECTURES 89)

        add_library(ace_cuda_loader SHARED train/dataloader/unpack_kernels.cu)
        target_include_directories(ace_cuda_loader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)
        target_compile_options(ace_cuda_loader PRIVATE
            $<$<COMPILE_LANGUAGE:CUDA>:--use_fast_math -O3 -Xcompiler=-Wall,-Wextra,-fno-exceptions>
        )
        message(STATUS "CUDA enabled: targeting sm_89")
    else()
        message(WARNING "CUDA compiler not found. Skipping ace_cuda_loader target.")
    endif()
endif()
