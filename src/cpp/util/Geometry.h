#pragma once

#include <stdint.h>
#include <vector>

namespace Geometry
{
    struct CubeGeometry
    {
        std::vector<float> positions;
        std::vector<float> texcoords;
        std::vector<float> normals;
        std::vector<uint32_t> indices;
    };

    CubeGeometry GetCubeData();
};