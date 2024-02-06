add_executable(test_ test.cpp)

# Для души, чтобы можно было писать 'make start_test` и было красиво
add_custom_target(start_test COMMAND test_)

target_link_libraries(test_ arithmetic GTest::gtest_main)

target_include_directories(test_ PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/test"
)

include(GoogleTest)
gtest_discover_tests(test_)