include(FetchContent)

set(BUILD_GMOCK OFF CACHE BOOL "" FORCE)
set(INSTALL_GTEST OFF CACHE BOOL "" FORCE)

FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB TESTS_FILES "*.cpp")
add_executable(tests ${TESTS_FILES})
target_link_libraries(tests core GTest::gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
