add_executable(pi main.cpp)

# Для души, чтобы можно было писать 'make start_pi` и было красиво
add_custom_target(start_pi COMMAND pi)

target_include_directories(pi PRIVATE
                          "${PROJECT_SOURCE_DIR}/include"
                          )

target_link_libraries(pi PRIVATE arithmetic)
