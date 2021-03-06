/**
 * @file image.cpp
 * @author Dávid Kubek
 * @date 1 March 2021
 */

#include <algorithm>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <memory>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/spdlog.h"

#define STBI_FAILURE_USERMSG
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"

#include "image_match/image.hpp"

#define MAGIC_NUMBER_BYTES 8

namespace image_match {

namespace fs = std::filesystem;

void
image_wrapper_deleter::operator()(unsigned char* data) const
{
    stbi_image_free(data);
}

image::image(const fs::path& image_path)
{
    int width, height, channels;
    auto image_data = image_wrapper(
      stbi_load(image_path.c_str(), &width, &height, &channels, 3));

    if (!image_data.get() || channels != 3) {
        SPDLOG_DEBUG("Error reading file {}", image_path.string());

        fail_ = true;
        fail_msg_ = stbi_failure_reason();
        return;
    }

    width_ = width;
    height_ = height;
    channels_ = channels;

    data_ = std::move(image_data);
}

image::image(size_t width, size_t height, size_t channels)
  : width_{ width }
  , height_{ height }
  , channels_{ channels }
{
    data_ = image_wrapper(new unsigned char[width * height * channels]);
}

image
subsampled_shift(const image& im,
                 std::uint32_t shift,
                 size_t min_width,
                 size_t min_height)
{
    SPDLOG_DEBUG("Subsampling image: width={}, height={}, channels={} "
                 "shift={}, min_width={}, min_height={}",
                 im.width(),
                 im.height(),
                 im.channels(),
                 shift,
                 min_width,
                 min_height);

    int out_w = std::max(min_width, im.width() >> shift);
    int out_h = std::max(min_height, im.height() >> shift);

    SPDLOG_DEBUG("out_w={}, out_h={}", out_w, out_h);

    image new_image(out_w, out_h, im.channels());

    stbir_resize_uint8(im.data().get(),
                       im.width(),
                       im.height(),
                       0,
                       new_image.data().get(),
                       out_w,
                       out_h,
                       0,
                       im.channels());

    SPDLOG_DEBUG("Subsampled width: {}, height: {}", out_w, out_h);

    return new_image;
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
get_image_paths(const std::filesystem::path& root)
{
    auto normalized = fs::absolute(root).lexically_normal();
    spdlog::info("Searching for images in {}", normalized.string());

    std::vector<fs::path> image_paths;
    for (auto&& dir_entry : fs::recursive_directory_iterator(normalized)) {
        SPDLOG_DEBUG("Found file {}", dir_entry.path().string());

        if (is_valid_file(dir_entry.path())) {
            SPDLOG_DEBUG("Adding file {}", dir_entry.path().string());

            image_paths.push_back(
              fs::absolute(dir_entry.path()).lexically_normal());
        }
    }

    return image_paths;
}

}
