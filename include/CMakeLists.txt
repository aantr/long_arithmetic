add_library(defines defines.cpp)

target_include_directories(defines PRIVATE
                          "${PROJECT_SOURCE_DIR}/include"
                          "${PROJECT_SOURCE_DIR}/arithmetic"
                          )