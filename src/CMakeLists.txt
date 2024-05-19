add_library(source_code INTERFACE)

# out data files
configure_file(${CMAKE_SOURCE_DIR}/KNNAlgorithmDataset.csv data_file COPYONLY)

target_include_directories(
    source_code INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)

find_package(MPI) 

include_directories(SYSTEM ${MPI_INCLUDE_PATH})

target_sources(
    source_code INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(a.out)

target_link_libraries(a.out source_code knn-cpp rapidcsv ${MPI_C_LIBRARIES})