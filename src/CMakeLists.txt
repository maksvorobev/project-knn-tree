add_library(source_code INTERFACE)

target_include_directories(
    source_code INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
)


target_sources(
    source_code INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
)

add_executable(a.out)

target_link_libraries(a.out source_code knn-cpp)