#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <ios>
#include <iostream>
#include <memory>
#include <stdexcept>
#include <vector>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/spdlog.h"

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "image_match/image.hpp"

#define MAGIC_NUMBER_BYTES 8

namespace image_match {

namespace fs = std::filesystem;

image_wrapper::image_wrapper(fs::path image_path)
{
    data_ = stbi_load(image_path.c_str(), &width_, &height_, &channels_, 0);

    if (!data_) {
        SPDLOG_DEBUG("Error reading file {}", image_path.string());
        throw std::runtime_error(stbi_failure_reason());
    }
}

image_wrapper::image_wrapper(image_wrapper&& other)
{
    this->~image_wrapper();

    data_ = other.data_;
    width_ = other.width_;
    height_ = other.height_;
    channels_ = other.channels_;
}

image_wrapper&
image_wrapper::operator=(image_wrapper&& other)
{
    this->~image_wrapper();

    data_ = other.data_;
    width_ = other.width_;
    height_ = other.height_;
    channels_ = other.channels_;

    return *this;
}

image_wrapper::~image_wrapper()
{
    stbi_image_free(data_);
}

bool
has_supported_extension(const std::filesystem::path& ext)
{
    SPDLOG_DEBUG("Checking extension {}", ext.string());

    return std::find(SUPPORTED_IMAGE_EXTENSIONS.begin(),
                     SUPPORTED_IMAGE_EXTENSIONS.end(),
                     ext.string()) != SUPPORTED_IMAGE_EXTENSIONS.end();
}

bool
check_magic_number(const std::filesystem::path& filepath)
{
    SPDLOG_DEBUG("Checking magic number for {}", filepath.string());

    std::ifstream input_file(filepath, std::ios_base::binary);
    if (!input_file) {
        spdlog::warn("Could not open file {}", filepath.string());
        return false;
    }

    std::uint8_t buff[MAGIC_NUMBER_BYTES];
    if (!input_file.read(reinterpret_cast<char*>(buff), sizeof(buff))) {
        spdlog::warn("Could not read magic number for file {}",
                     filepath.string());
        return false;
    }

    // Compare magic number
    switch (buff[0]) {
        case static_cast<std::uint8_t>('\xFF'):
            SPDLOG_DEBUG("Found JPEG magic number.");
            return (!strncmp((const char*)buff, "\xFF\xD8\xFF", 3)) ? true
                                                                    : false;

        case static_cast<std::uint8_t>('\x89'):
            SPDLOG_DEBUG("Found PNG magic number.");
            return (!strncmp(
                     (const char*)buff, "\x89\x50\x4E\x47\x0D\x0A\x1A\x0A", 8))
                     ? true
                     : false;

        case 'B':
            SPDLOG_DEBUG("Found BMP magic number.");
            return ((buff[1] == 'M')) ? true : false;
    }

    SPDLOG_DEBUG("Magic number unsupported or invalid.");

    return false;
}

bool
is_valid_file(const std::filesystem::path& filepath)
{
    return (fs::is_regular_file(filepath) || fs::is_symlink(filepath)) &&
           has_supported_extension(filepath.extension()) &&
           check_magic_number(filepath);
}

std::vector<std::filesystem::path>
get_image_paths(std::filesystem::path root)
{
    root = fs::absolute(root);
    spdlog::info("Searching for images in {}", root.string());

    std::vector<fs::path> image_paths;
    for (auto&& dir_entry : fs::recursive_directory_iterator(root)) {
        SPDLOG_DEBUG("Found file {}", dir_entry.path().string());

        if (is_valid_file(dir_entry.path())) {
            SPDLOG_DEBUG("Adding file {}", dir_entry.path().string());

            image_paths.push_back(dir_entry.path());
        }
    }

    return image_paths;
}

}
