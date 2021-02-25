#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <numeric>
#include <stdexcept>

#ifndef NDEBUG
#define SPDLOG_ACTIVE_LEVEL SPDLOG_LEVEL_TRACE
#endif
#include "spdlog/spdlog.h"

#include "image_match/csd.hpp"
#include "image_match/hmmd.hpp"

namespace image_match {

template<size_t N>
using u8arr = typename std::array<std::uint8_t, N>;

constexpr u8arr<4> BIN32_SUBSPACE_BOUNDRIES{ 5, 59, 109, 255 };
constexpr u8arr<5> SUBSPACE_BOUNDRIES{ 5, 19, 59, 109, 255 };

constexpr u8arr<4> BIN32_HUE_BIT_BIN_SIZES{ 0, 2, 2, 2 };
constexpr u8arr<4> BIN32_SUM_BIT_BIN_SIZES{ 3, 2, 0, 0 };

constexpr u8arr<5> BIN64_HUE_BIT_BIN_SIZES{ 0, 2, 2, 3, 3 };
constexpr u8arr<5> BIN64_SUM_BIT_BIN_SIZES{ 3, 2, 2, 1, 0 };

constexpr u8arr<5> BIN128_HUE_BIT_BIN_SIZES{ 0, 2, 3, 3, 3 };
constexpr u8arr<5> BIN128_SUM_BIT_BIN_SIZES{ 4, 2, 2, 2, 2 };

constexpr u8arr<5> BIN256_HUE_BIT_BIN_SIZES{ 0, 2, 4, 4, 4 };
constexpr u8arr<5> BIN256_SUM_BIT_BIN_SIZES{ 5, 3, 2, 2, 2 };

std::uint32_t
compute_subsample_shift(const image& im)
{
    double w, h;
    w = static_cast<double>(im.width());
    h = static_cast<double>(im.height());

    return static_cast<std::uint32_t>(
      std::max(0.0, std::floor(std::log2(std::sqrt(w * h)) - 7.5)));
}

using quantized_map = std::vector<std::vector<std::uint8_t>>;

template<size_t N>
quantized_map
quantize(const image& im,
         const u8arr<N>& sub_bounds,
         const u8arr<N>& hue_bin_bits,
         const u8arr<N>& sum_bin_bits)
{
    SPDLOG_DEBUG("Quantizing image with bins, width={}, height={}",
                 im.width(),
                 im.height());

    quantized_map qm;
    qm.resize(im.height());
    for (auto&& row : qm)
        row.resize(im.width());

    // Compute offsets of individual bins
    std::vector<std::uint8_t> offsets(N, 0);
    for (size_t i = 0; i + 1 < N; ++i)
        offsets[i + 1] =
          offsets[i] + (1 << hue_bin_bits[i]) * (1 << sum_bin_bits[i]);

    std::uint8_t subspace, hue_bits, sum_bits;
    for (size_t y = 0; y < im.height(); ++y) {
        for (size_t x = 0; x < im.width(); ++x) {
            pixel p = im[y][x];

            // Determine the pixel subspace
            subspace = 0;
            while (p[2] > sub_bounds[subspace])
                subspace++;

            // Place the color into bins according to subspace
            hue_bits = p[0] >> (8 - hue_bin_bits[subspace]);
            sum_bits = p[1] >> (8 - sum_bin_bits[subspace]);

            // Compute the final bin value
            qm[y][x] = offsets[subspace] +
                       ((hue_bits << sum_bin_bits[subspace]) | sum_bits);
        }
    }

    return qm;
}

quantized_map
quantize(const image& im, CSDType type)
{
    SPDLOG_DEBUG("Quantizing image, width={}, height={}, type={}",
                 im.width(),
                 im.height(),
                 type);

    switch (type) {
        case CSDType::Bin32:
            return quantize(im,
                            BIN32_SUBSPACE_BOUNDRIES,
                            BIN32_HUE_BIT_BIN_SIZES,
                            BIN32_SUM_BIT_BIN_SIZES);
        case CSDType::Bin64:
            return quantize(im,
                            SUBSPACE_BOUNDRIES,
                            BIN64_HUE_BIT_BIN_SIZES,
                            BIN64_SUM_BIT_BIN_SIZES);
        case CSDType::Bin128:
            return quantize(im,
                            SUBSPACE_BOUNDRIES,
                            BIN128_HUE_BIT_BIN_SIZES,
                            BIN128_SUM_BIT_BIN_SIZES);
        case CSDType::Bin256:
            return quantize(im,
                            SUBSPACE_BOUNDRIES,
                            BIN256_HUE_BIT_BIN_SIZES,
                            BIN256_SUM_BIT_BIN_SIZES);
    }
}

void
scan_sector(const quantized_map& qm,
            CSD::descriptor& d,
            std::vector<bool>& seen,
            size_t x_begin,
            size_t y_begin)
{
    // Reset seen bit vector
    std::fill(seen.begin(), seen.end(), false);

    size_t val;
    for (size_t dy = 0; dy < STRUCTURING_ELEMENT_SIZE; ++dy) {
        for (size_t dx = 0; dx < STRUCTURING_ELEMENT_SIZE; ++dx) {
            val = qm[y_begin + dy][x_begin + dx];
            if (!seen[val]) {
                seen[val] = true;
                ++d[val];
            }
        }
    }
}

CSD::descriptor
scan(const quantized_map& qm, size_t bin_size)
{
    SPDLOG_DEBUG("Scanning quantized map with bin_size={}", bin_size);

    CSD::descriptor d(bin_size, 0);
    std::vector<bool> seen(bin_size);

    size_t y_bound, x_bound;
    y_bound = qm.size() - STRUCTURING_ELEMENT_SIZE + 1;
    x_bound = qm[0].size() - STRUCTURING_ELEMENT_SIZE + 1;

    for (size_t y = 0; y < y_bound; ++y) {
        for (size_t x = 0; x < x_bound; ++x) {
            scan_sector(qm, d, seen, x, y);
        }
    }

    return d;
}

CSD::descriptor
scan(const quantized_map& qm, CSDType type)
{
    SPDLOG_DEBUG("Scanning quantized map for type={}", type);

    switch (type) {
        case CSDType::Bin32:
            return scan(qm, 32);
        case CSDType::Bin64:
            return scan(qm, 64);
        case CSDType::Bin128:
            return scan(qm, 128);
        case CSDType::Bin256:
            return scan(qm, 256);
    }
}

CSD::CSD(const image& im, CSDType type)
{
    SPDLOG_DEBUG("Generating Color Structure Desriptor type={}", type);

    auto p = compute_subsample_shift(im);
    auto resized = subsampled_shift(
      im, p, STRUCTURING_ELEMENT_SIZE, STRUCTURING_ELEMENT_SIZE);
    rgb2hmmd(resized);
    auto qm = quantize(resized, type);
    auto descriptor = scan(qm, type);

    // Normalize the descriptor
    for (auto&& val : descriptor)
        val /= (im.width() - STRUCTURING_ELEMENT_SIZE + 1) *
               (im.height() - STRUCTURING_ELEMENT_SIZE + 1);

    data = descriptor;
}

float
compare(CSD desc1, CSD desc2)
{
    SPDLOG_DEBUG("Comparing descriptors.");

    if (desc1.type != desc2.type)
        throw std::invalid_argument("Non-matching descriptor types.");

    float acc = 0;
    for (size_t i = 0; i < desc1.data.size(); ++i)
        acc += std::fabs(desc1.data[i] - desc2.data[i]);

    return acc;
}

}
