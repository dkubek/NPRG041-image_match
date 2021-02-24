#include "image_match/hmmd.hpp"

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/spdlog.h"

namespace image_match {

void
rgb2hmmd_pixel_inplace(pixel p)
{
    pixel::value_type min, max;

    min =
      p[0] < p[1] ? (p[0] < p[2] ? p[0] : p[2]) : (p[1] < p[2] ? p[1] : p[2]);
    max =
      p[0] > p[1] ? (p[0] > p[2] ? p[0] : p[2]) : (p[1] > p[2] ? p[1] : p[2]);

    pixel::value_type sum, diff, hue;
    diff = max - min;
    sum = static_cast<pixel::value_type>(
      (static_cast<int>(max) + static_cast<int>(min)) >> 1);
    hue = 0;

    // Hue is defined same as in HSV
    if (diff) {
        if (max == p[0])
            hue = 43 * (p[1] - p[2]) / (max - min);
        else if (max == p[1])
            hue = 85 + 43 * (p[2] - p[0]) / (max - min);
        else
            hue = 171 + 43 * (p[0] - p[1]) / (max - min);
    }

    p[0] = hue;
    p[1] = sum;
    p[2] = diff;
}

void
rgb2hmmd(image& im)
{
    SPDLOG_DEBUG("Converting image to HMMD color space.");

    for (size_t y = 0; y < im.height(); ++y)
        for (size_t x = 0; x < im.width(); ++x)
            rgb2hmmd_pixel_inplace(im[y][x]);
}

}
