add_executable(image_match image_match.cpp)
target_link_libraries(image_match
    PRIVATE CLI11
    PRIVATE spdlog::spdlog_header_only
    PRIVATE image
    PRIVATE csd
    PRIVATE RapidJSON
    )
