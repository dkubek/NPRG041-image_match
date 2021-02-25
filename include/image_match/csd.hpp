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

struct CSD
{
    using descriptor = std::vector<float>;

    CSDType type;
    descriptor data;

    CSD(const image& im, CSDType type);
};

float compare(CSD desc1, CSD desc2);

}

#endif
