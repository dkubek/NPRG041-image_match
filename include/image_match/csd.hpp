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

CSDType
csd_from_int(int);

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
