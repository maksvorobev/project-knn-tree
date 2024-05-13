add_library(rapidcsv INTERFACE)

target_sources(rapidcsv INTERFACE
    ${CMAKE_CURRENT_SOURCE_DIR}/rapidcsv/src/rapidcsv.h
)

target_include_directories(rapidcsv INTERFACE
${CMAKE_CURRENT_SOURCE_DIR}/rapidcsv/src
)

