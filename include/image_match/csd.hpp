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
    CSDType type;
    std::vector<float> data;

    CSD(const image& im, CSDType type);
};

}

#endif
