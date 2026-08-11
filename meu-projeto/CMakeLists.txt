cmake_minimum_required(VERSION 3.14)
project(streaming_platform)

# Set standard to C++20 as required by TP3
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra)

# Sanitizer configuration
include(CheckCXXCompilerFlag)
option(USE_TSAN "Enable ThreadSanitizer (disables Address/Undefined Sanitizer)" OFF)

if(CMAKE_BUILD_TYPE MATCHES Debug)
    if(USE_TSAN)
        check_cxx_compiler_flag("-fsanitize=thread" HAS_TSAN)
        if(HAS_TSAN)
            add_compile_options(-fsanitize=thread)
            add_link_options(-fsanitize=thread)
            message(STATUS "ThreadSanitizer is ENABLED (-fsanitize=thread)")
        else()
            message(WARNING "ThreadSanitizer is not supported by the compiler.")
        endif()
    else()
        check_cxx_compiler_flag("-fsanitize=address,undefined" HAS_SANITIZER)
        if(HAS_SANITIZER)
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
endif()

# Fetch dependencies
include(FetchContent)

# 1. nlohmann/json
FetchContent_Declare(
  json
  GIT_REPOSITORY https://github.com/nlohmann/json.git
  GIT_TAG         v3.11.3
)
FetchContent_MakeAvailable(json)

# 2. Catch2 (v3)
FetchContent_Declare(
  Catch2
  GIT_REPOSITORY https://github.com/catchorg/Catch2.git
  GIT_TAG         v3.5.2
)
FetchContent_MakeAvailable(Catch2)

include_directories(src)

# Main Executable
add_executable(meu_sistema src/main.cpp)
target_link_libraries(meu_sistema PRIVATE nlohmann_json::nlohmann_json)

# Catch2 Tests Executable
add_executable(testes tests/test_tp3.cpp)
target_include_directories(testes PRIVATE src)
target_link_libraries(testes PRIVATE Catch2::Catch2WithMain nlohmann_json::nlohmann_json)

include(CTest)
include(Catch)
catch_discover_tests(testes)

# Qt6 Configuration (GUI)
find_package(Qt6 COMPONENTS Widgets QUIET)
if(Qt6_FOUND)
    message(STATUS "Qt6 found! Enabling GUI target 'gui'.")
    qt_standard_project_setup()
    qt_add_executable(gui src/gui_main.cpp src/janela.cpp src/janela.hpp)
    target_include_directories(gui PRIVATE src)
    target_link_libraries(gui PRIVATE Qt6::Widgets nlohmann_json::nlohmann_json)
else()
    message(WARNING "Qt6 not found. GUI target will be disabled.")
endif()