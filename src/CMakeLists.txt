add_library(image
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/image_match/image.hpp"
    image.cpp
    )

add_library(hmmd
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/image_match/hmmd.hpp"
    hmmd.cpp
    )

add_library(csd
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/image_match/csd.hpp"
    csd.cpp
    )

target_link_libraries(image
    PUBLIC gsl
    PRIVATE spdlog
    PRIVATE stb
    )

target_link_libraries(hmmd
    PUBLIC image
    PRIVATE spdlog
    )

target_link_libraries(csd
    PUBLIC image
    PRIVATE hmmd
    PRIVATE spdlog
    )

target_include_directories(image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_include_directories(hmmd PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
target_include_directories(csd PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
