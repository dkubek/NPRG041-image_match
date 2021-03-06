/**
 * @file hmmd.hpp
 * @author Dávid Kubek
 * @date 1 March 2021
 * @brief Convert image to HMMD color scheme.
 *
 * The HMMD space is defined in three dimensions using sum- and diff-axes as
 * well as hue angle, where:
 *
 *      Max = max(R,G,B)
 *      Min = min(R,G,B)
 *      Diff = Max - Min
 *      Sum = (Max + Min) / 2
 *      Hue = *defined same as for HSV color space*
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
#ifndef _IMAGE_MATCH_HMMD_GUARD
#define _IMAGE_MATCH_HMMD_GUARD

#include "image_match/image.hpp"

namespace image_match {

/// Convert the image to HMMD color space in place.
void
rgb2hmmd(image& im);

}

#endif
