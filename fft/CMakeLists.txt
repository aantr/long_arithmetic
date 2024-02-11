add_library(fft fft.cpp)

target_include_directories(fft PRIVATE
                          "${PROJECT_SOURCE_DIR}/fft"
                          "${PROJECT_SOURCE_DIR}/include"
                          )

