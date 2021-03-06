add_executable(image_match image_match.cpp)
set_target_properties(image_match
    PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
    )
target_link_libraries(image_match
    PRIVATE image
    PRIVATE csd
    PRIVATE CLI11
    PRIVATE spdlog
    PRIVATE RapidJSON
    )
