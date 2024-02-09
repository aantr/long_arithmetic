add_executable(pi main.cpp)

# Для души, чтобы можно было писать 'make start_pi` и было красиво
add_custom_target(start_pi COMMAND pi)

target_include_directories(pi PRIVATE
                          "${PROJECT_SOURCE_DIR}/include"
                          )

target_link_libraries(pi PRIVATE arithmetic)


add_executable(pi_32 main_32.cpp)

add_custom_target(start_pi_32 COMMAND pi_32)

target_include_directories(pi_32 PRIVATE
                          "${PROJECT_SOURCE_DIR}/include"
                          )

target_link_libraries(pi_32 PRIVATE arithmetic_32)
