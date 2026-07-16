cmake_minimum_required(VERSION 3.14)
project(streaming_platform)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

# Check if sanitizers are supported by the compiler/linker before enabling them
include(CheckCXXCompilerFlag)
if(CMAKE_BUILD_TYPE MATCHES Debug)
    # Check compiler support
    check_cxx_compiler_flag("-fsanitize=address,undefined" HAS_SANITIZER)
    if(HAS_SANITIZER)
        # Check linker support by trying to compile and link a dummy target with these options
        set(CMAKE_REQUIRED_FLAGS "-fsanitize=address,undefined")
        check_cxx_compiler_flag("-fsanitize=address,undefined" HAS_SANITIZER_LINK)
        unset(CMAKE_REQUIRED_FLAGS)
        if(HAS_SANITIZER_LINK)
            add_compile_options(-fsanitize=address,undefined -fno-omit-frame-pointer)
            add_link_options(-fsanitize=address,undefined)
            message(STATUS "Address and Undefined sanitizers are ENABLED")
        else()
            message(WARNING "Sanitizer compilation succeeded but linking failed. Disabling sanitizers.")
        endif()
    else()
        message(WARNING "Sanitizers are not supported by the current compiler. Disabling sanitizers.")
    endif()
endif()

include_directories(src)
add_executable(meu_sistema src/main.cpp)

# Q4 - Catch2 Configuration via FetchContent
include(FetchContent)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG         v3.5.2
)
FetchContent_MakeAvailable(Catch2)

add_executable(testes tests/test_tp2.cpp)
target_include_directories(testes PRIVATE src)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain)

include(CTest)
include(Catch)
catch_discover_tests(testes)