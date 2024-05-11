add_library(knn-cpp INTERFACE)

target_sources(knn-cpp INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/knn-cpp/include/knncpp.h
)

find_package(Eigen3 REQUIRED)
include_directories(SYSTEM ${EIGEN3_INCLUDE_DIRS})

target_include_directories(knn-cpp INTERFACE
${CMAKE_CURRENT_SOURCE_DIR}/knn-cpp/include
${EIGEN3_INCLUDE_DIR}
)

