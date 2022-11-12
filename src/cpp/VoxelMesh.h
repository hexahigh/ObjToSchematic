#pragma once

#include <napi.h>
#include <vector>
#include <unordered_map>
#include "./external/glm/glm/vec3.hpp"

#include "Texture.h"

class RGBA;
class VoxelMeshRenderBuffer;

enum class Direction
{
    x,
    y,
    z
};

struct Ray
{
    glm::ivec3 origin;
    Direction direction;
};

struct Vertex;

class VoxelMesh
{
public:
    VoxelMesh();

    void InternalSetVertices(const float *array, size_t length);
    void InternalSetTexcoords(const float *array, size_t length);
    void InternalSetTriangles(const int32_t *array, size_t length);
    void InternalVoxelise(const uint32_t desired_height);
    void InternalVoxeliseTriangle(const size_t i);

    Vertex InternalGetVertex(const size_t index, const size_t uv_index);
    bool InternalIntersect(const Ray &ray, const glm::vec3 &vertex_a, const glm::vec3 &vertex_b, const glm::vec3 &vertex_c, glm::vec3 &out_vertex);

    void InternalAddVoxel(const glm::ivec3 &position, const RGBA &colour);
    void Finalise();

    size_t InternalGetNeighbourIndex(const glm::ivec3 &neighbour) const;
    size_t &InternalGetNeighbours(const glm::ivec3 &neighbour);

    size_t GetNumVoxels() const { return voxel_positions_.size() / 3; }

    RGBA GetVoxelColour(const Vertex& v0, const Vertex& v1, const Vertex& v2, const glm::vec3& position) const;

    std::vector<float> vertices_;
    std::vector<float> texcoords_;
    std::vector<uint32_t> triangles_;

    std::vector<int32_t> voxel_positions_;
    std::vector<float> voxel_colours_;

    std::unordered_map<size_t, size_t> voxel_hash_;
    std::unordered_map<size_t, size_t> neighbour_map_;
    glm::ivec3 bounds_min;
    glm::ivec3 bounds_max;

    VoxelMeshRenderBuffer* render_buffer_;
    Texture texture;
};