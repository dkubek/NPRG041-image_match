#include "image_match/hmmd.hpp"
#include <algorithm>
#include <cmath>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/spdlog.h"

#include "image_match/csd.hpp"
#include "image_match/image.hpp"

namespace image_match {

double
compute_scale_coefficient(const image& im)
{
    double w, h;
    w = static_cast<double>(im.width());
    h = static_cast<double>(im.height());

    double p = std::max(0.0, std::floor(std::log2(std::sqrt(w * h)) - 7.5));

    return std::pow(2, p);
}

CSD::CSD(const image& im, CSDType type)
{
    SPDLOG_DEBUG("Generating Color Structure Desriptor type={}", type);

    double sc = compute_scale_coefficient(im);
    auto res_im = resized(im, sc);

    SPDLOG_DEBUG(
      "Resized width: {}, height: {}", res_im.width(), res_im.height());

    rgb2hmmd(res_im);

    // Quantize
    // Scan
    // Normalize
}

}
