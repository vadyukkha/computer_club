cmake_minimum_required(VERSION 3.16)
project(computer_club LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(POLICY CMP0135)
    cmake_policy(SET CMP0135 NEW)
endif()

# library (core logic)
file(GLOB SRC_FILES ${PROJECT_SOURCE_DIR}/src/*.cpp)
list(REMOVE_ITEM SRC_FILES ${PROJECT_SOURCE_DIR}/src/main.cpp)
add_library(core ${SRC_FILES})
target_include_directories(core PUBLIC ${PROJECT_SOURCE_DIR}/include)

# main executable
add_executable(main ${PROJECT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(main PRIVATE core)

# tests
option(BUILD_TESTS "Build tests" ON)
if (BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif()
