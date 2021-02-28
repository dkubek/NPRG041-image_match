#ifndef _IMAGE_MATCH_IMAGE_GUARD
#define _IMAGE_MATCH_IMAGE_GUARD

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>

#include "gsl/gsl-lite.hpp"

namespace image_match {

static const std::array<std::string, 4> SUPPORTED_IMAGE_EXTENSIONS{ ".png",
                                                                    ".jpeg",
                                                                    ".jpg",
                                                                    ".bmp" };

struct image_wrapper_deleter
{
    void operator()(unsigned char* data) const;
};

using image_wrapper = std::unique_ptr<unsigned char, image_wrapper_deleter>;

using pixel = gsl::span<unsigned char>;

class image
{
  public:
    struct row_proxy
    {
      public:
        row_proxy(size_t row, const image& im)
          : base_{ row * im.width() * im.channels() }
          , im_{ im }
        {}

        pixel operator[](size_t index)
        {
            return { im_.data_.get() + base_ + index * im_.channels(),
                     im_.channels() };
        }

        const pixel operator[](size_t index) const
        {
            return { im_.data_.get() + base_ + index * im_.channels(),
                     im_.channels() };
        }

      private:
        size_t base_;
        const image& im_;
    };

    image(const std::filesystem::path& image_path);
    image(size_t width, size_t height, size_t channels);

    size_t width() const { return width_; };
    size_t height() const { return height_; };
    size_t channels() const { return channels_; };

    image_wrapper& data() { return data_; };
    const image_wrapper& data() const { return data_; };

    row_proxy operator[](size_t index) { return row_proxy(index, *this); };
    const row_proxy operator[](size_t index) const
    {
        return row_proxy(index, *this);
    };

    operator bool() { return !fail_; }
    std::string fail_msg() const { return fail_msg_; }

    friend image subsampled_shift(const image& im,
                                  std::uint32_t shift,
                                  size_t min_width,
                                  size_t min_height);

  private:
    image_wrapper data_;

    size_t width_;
    size_t height_;
    size_t channels_;

    bool fail_ = false;
    std::string fail_msg_;
};

std::vector<std::filesystem::path>
get_image_paths(const std::filesystem::path& root);

}

#endif
