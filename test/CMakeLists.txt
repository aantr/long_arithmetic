add_executable(test test.cpp)

# Для души, чтобы можно было писать 'make start_test` и было красиво
add_custom_target(start_test COMMAND test)

target_link_libraries(test PRIVATE arithmetic)

target_include_directories(test PRIVATE
        "${PROJECT_SOURCE_DIR}/include"
        "${PROJECT_SOURCE_DIR}/test"
)
