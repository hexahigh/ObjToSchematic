#pragma once

#include <stdint.h>
#include <vector>

class VoxelMesh;

class VoxelMeshRenderBuffer
{
public:
    VoxelMeshRenderBuffer(const VoxelMesh* owner);

    std::vector<float> GetPositions(const size_t start_index, const size_t end_index) const;
    std::vector<float> GetColours(const size_t start_index, const size_t end_index) const;

private:
    const VoxelMesh* owner;
};
