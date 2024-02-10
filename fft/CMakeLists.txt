add_library(fft fft.cpp)

target_include_directories(fft PRIVATE
                          "${PROJECT_SOURCE_DIR}/fft"
                          "${PROJECT_SOURCE_DIR}/include"
                          # "${PROJECT_SOURCE_DIR}/arithmetic"
                          )

# target_link_libraries(fft PRIVATE arithmetic_32)
