#include "VoxelMeshRenderBuffer.h"

#include <stdio.h>

#include "Core.h"
#include "VoxelMesh.h"
#include "./util/Geometry.h"

VoxelMeshRenderBuffer::VoxelMeshRenderBuffer(const VoxelMesh* in_owner)
    : owner(in_owner)
{
}

std::vector<float> VoxelMeshRenderBuffer::GetPositions(const size_t start_index, const size_t end_index) const
{
    ScopedTimer("VoxelMeshRenderBuffer::GetPositions");

    static const Geometry::CubeGeometry cube_data = Geometry::GetCubeData();
    static const size_t num_cube_vertices = cube_data.positions.size();

    const std::vector<int32_t>& raw_positions = this->owner->voxel_positions_;

    const size_t num_voxels = end_index - start_index;

    std::vector<float> result(num_cube_vertices * num_voxels);

    for (size_t base_voxel_index = 0; base_voxel_index < num_voxels; ++base_voxel_index)
    {
        const size_t voxel_index = base_voxel_index + start_index;
        for (size_t vertex_index = 0; vertex_index < num_cube_vertices; ++vertex_index)
        {
            const size_t result_index = base_voxel_index * num_cube_vertices + vertex_index;
            result[result_index] = cube_data.positions[vertex_index] + raw_positions[(voxel_index * 3) + (vertex_index % 3)];
        }
    }

    return result;
}

std::vector<float> VoxelMeshRenderBuffer::GetColours(const size_t start_index, const size_t end_index) const
{
    ScopedTimer("VoxelMeshRenderBuffer::GetColours");
    static const size_t num_cube_vertices = 8;

    static const size_t component_offset = 4 * 6 * 4;

    const std::vector<float>& raw_colours = this->owner->voxel_colours_;

    const size_t num_voxels = end_index - start_index;

    std::vector<float> result(component_offset * num_voxels);

    for (size_t base_voxel_index = 0; base_voxel_index < num_voxels; ++base_voxel_index)
    {
        const size_t voxel_index = base_voxel_index + start_index;

        const std::vector<float> colour_array = {
            raw_colours[voxel_index * 4 + 0],
            raw_colours[voxel_index * 4 + 1],
            raw_colours[voxel_index * 4 + 2],
            raw_colours[voxel_index * 4 + 3]
        };
        
        for (size_t i = 0; i < component_offset; ++i)
        {
            result[base_voxel_index * component_offset + i] = colour_array[i % 4];
        }
    }

    return result;
}