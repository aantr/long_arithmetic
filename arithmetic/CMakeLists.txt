add_library(arithmetic arithmetic.cpp arithmetic_operators.cpp)

target_include_directories(arithmetic PUBLIC
                          "${PROJECT_SOURCE_DIR}/arithmetic"
                          "${PROJECT_SOURCE_DIR}/fft"
                          "${PROJECT_SOURCE_DIR}/include"
                          )

target_link_libraries(arithmetic PRIVATE fft)
