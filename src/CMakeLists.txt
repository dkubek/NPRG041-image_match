add_library(image
    "${CMAKE_CURRENT_SOURCE_DIR}/../include/image_match/image.hpp"
    image.cpp
    )

target_link_libraries(image
    PRIVATE spdlog::spdlog_header_only
    PRIVATE stb
    )

target_include_directories(image PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/../include")
