#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>

# define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_DEBUG
#include "spdlog/spdlog.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

int
main(int argc, char* argv[])
{
    std::vector<std::string> args{ argv, argv + argc };

    int x,y,n;
    unsigned char *data = stbi_load("image.png", &x, &y, &n, 0);
    stbi_image_free(data);

    spdlog::debug("Image height: {}, image width: {}", x, y);

    return EXIT_SUCCESS;
}
