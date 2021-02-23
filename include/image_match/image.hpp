#ifndef _IMAGE_MATCH_IMAGE_GUARD
#define _IMAGE_MATCH_IMAGE_GUARD

#include <array>
#include <cstdint>
#include <filesystem>

namespace image_match {

static const std::array<std::string, 6> SUPPORTED_IMAGE_EXTENSIONS{
    ".png", ".jpeg", ".jpg", ".bmp"
};

class image_wrapper
{
  public:
    image_wrapper(std::filesystem::path image_path);

    image_wrapper(image_wrapper& other) = delete;
    image_wrapper& operator=(image_wrapper& other) = delete;

    image_wrapper(image_wrapper&& other);
    image_wrapper& operator=(image_wrapper&& other);

    int width() const { return width_; };
    int height() const { return height_; };
    int channels() const { return channels_; };

    ~image_wrapper();

  private:
    unsigned char* data_;
    int width_;
    int height_;
    int channels_;
};

std::vector<std::filesystem::path>
get_image_paths(std::filesystem::path root);

}

#endif
