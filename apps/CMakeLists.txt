add_executable(image_match image_match.cpp)
target_link_libraries(image_match
    PRIVATE image
    PRIVATE csd
    PRIVATE CLI11
    PRIVATE spdlog
    PRIVATE RapidJSON
    )
