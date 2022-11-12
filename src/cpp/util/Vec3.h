#pragma once

#include "../external/glm/glm/vec3.hpp"

namespace Vec3
{
    size_t hash(const glm::ivec3& vec)
    {
        return ((vec.x + 10000) << 24) + ((vec.y + 10000) << 12) + ((vec.z + 10000) << 0); 
    }
}