enable_testing()

add_executable(test_ test.cpp)

target_link_libraries(test_ arithmetic arithmetic_32 GTest::gtest_main)

target_include_directories(test_ PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/test"
        "${PROJECT_SOURCE_DIR}/arithmetic"
)

include(GoogleTest)
gtest_discover_tests(test_)