/**
 * @file image.hpp
 * @author Dávid Kubek
 * @date 1 March 2021
 * @brief Generate the Color Structure Desriptor
 *
 *
 * +--------------------------------------------------------------------------+
 * | Color Structure Histogram generation:                                    |
 * +--------------------------------------------------------------------------+
 *
 * The color structure descriptor (CSD) represents an image by both the color
 * distribution of the image (similar to a color histogram) and the local
 * spatial structure of the color.
 *
 * Extraction is a five step process:
 *  1. The input image is subsampled to a more standardized size to improve
 *     performance. The subsamplig factor is given by K = 2^p, where
 *
 *              p = max{0, \log{WH} – 7.5]}
 *
 *     where W and H are the width and height of the image.
 *
 *  2. The image is converted to the HMMD color space.
 *
 *  3. The pixels in the image are nonuniformly quantized (more detail below)
 *
 *  4. The image is scanned at every position with a structuring element (of
 *     size 8x8 pixels), constructing the desired color structure histohram.
 *     At each position, the histogram is updated on the basis of the colors
 *     present within the element. Each of the corresponding values in found in
 *     the structuring element is incremented by one.
 *
 *  5. Normalization of the histogram.
 *
 *
 * +--------------------------------------------------------------------------+
 * | HMMD color space quantization.                                           |
 * +--------------------------------------------------------------------------+
 *
 * Four nonuniform quantizations of HMMD are defined in the MPEG-7 Standard.
 * The four quantizations partition the space into 256, 128, 64 and 32 cells,
 * respectively.
 *
 * Each quantization is defined via five subspaces of HMMD as follows. The
 * diff-axis, itself defined on the interval [0, 255], is cut into five
 * subintervals: [0,6), [6, 20), [20, 60), [60, 110) and [110, 255).  This
 * partition of the diff-axis implicitly defines five subspaces numbered 0,
 * 1,..., 4, respectively. Each subspace is that subset of HMMD where sum and
 * hue are allowed to take all values in their respective ranges and where diff
 * is restricted to one of the five intervals.
 *
 * A partition of HMMD is obtained by partitioning the ranges of hue and sum
 * into uniform intervals within each subspace according to table given below.
 *
 *  +==========+=======+=======+=======+=======+======+======+======+======+
 *  | Subspace | 256 H | 256 S | 128 H | 128 S | 64 H | 64 S | 32 H | 32 S |
 *  +==========+=======+=======+=======+=======+======+======+======+======+
 *  |        0 |     1 |    32 |     1 |    16 |    1 |    8 |    1 |    8 |
 *  +----------+-------+-------+-------+-------+------+------+------+------+
 *  |        1 |     4 |     8 |     4 |     4 |    4 |    4 |             |
 *  +----------+-------+-------+-------+-------+------+------+    4      4 |
 *  |        2 |    16 |     4 |     8 |     4 |    4 |    4 |             |
 *  +----------+-------+-------+-------+-------+------+------+------+------+
 *  |        3 |    16 |     4 |     8 |     4 |    8 |    2 |    4 |    1 |
 *  +----------+-------+-------+-------+-------+------+------+------+------+
 *  |        5 |    16 |     4 |     8 |     4 |    8 |    1 |    4 |    1 |
 *  +----------+-------+-------+-------+-------+------+------+------+------+
 *
 *
 * Sources:
 *  Manjunath, B. & Salembier, Philippe & Sikora, Thomas. (2002). Introduction
 *  to MPEG-7: Multimedia Content Description Interface.
 *
 *  Ben Abdelali, Abdessalem & Krifa, M.N. & Touil, Lamjed & Abdellatif, Mtibaa
 *  & Bourennane, El-Bay. (2009). Astudy of the color structure descriptor for
 *  shot boundary detection. International Journal of Sciences and Techniques
 *  of Automatic control & computer engineering, IJ-STA. 3. 956−971.
 *
 *  Manjunath, B. & Ohm, J.R. & Vasudevan, Vinod & Yamada, Akio. (2001). Color
 *  and Texture Descriptors. Circuits and Systems for Video Technology, IEEE
 *  Transactions on. 11. 703 - 715. 10.1109/76.927424.
 *
 */
#ifndef _IMAGE_MATCH_CSD_GUARD
#define _IMAGE_MATCH_CSD_GUARD

#include <vector>

#include "image_match/image.hpp"

namespace image_match {

#define STRUCTURING_ELEMENT_SIZE 8

enum CSDType
{
    Bin32,
    Bin64,
    Bin128,
    Bin256
};

/**
 * @brief Return the CSDType from number of bins.
 *
 * The available number of bins are 32, 64, 128 and 256.  Throws
 * invalid_argument otherwise.
 */
CSDType
csd_from_int(int);

/// Color structure descriptor
struct CSD
{
    using descriptor = std::vector<float>;

    CSDType type;
    descriptor data;

    CSD(const image& im, CSDType type);
    CSD(const descriptor& desc, CSDType type);
};

float
compare(const CSD& desc1, const CSD& desc2);

}

#endif
