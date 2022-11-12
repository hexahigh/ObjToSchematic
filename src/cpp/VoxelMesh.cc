#include "VoxelMesh.h"
#include <algorithm>
#include <cmath>
#include "RGBA.h"
#include "VoxelMeshRenderBuffer.h"
#include "Core.h"
#include "./util/Vec3.h"

#define MIN(a, b) a < b ? a : b
#define MAX(a, b) a > b ? a : b

struct UV
{
    UV(float in_u, float in_v)
        : u(in_u)
        , v(in_v)
    {
    }

    float u;
    float v;
};

struct Vertex
{
    Vertex(glm::vec3& in_position, UV& in_texcoord)
        : position(in_position)
        , texcoord(in_texcoord)
    {
    }

    glm::vec3 position;
    UV texcoord; 
};

static const std::vector<glm::ivec3> NEIGHBOURS = {
    glm::ivec3(1, 1, -1),
    glm::ivec3(0, 1, -1),
    glm::ivec3(-1, 1, -1),
    glm::ivec3(1, 0, -1),
    glm::ivec3(-1, 0, -1),
    glm::ivec3(1, -1, -1),
    glm::ivec3(0, -1, -1),
    glm::ivec3(-1, -1, -1),
    glm::ivec3(1, 1, 0),
    glm::ivec3(-1, 1, 0),
    glm::ivec3(1, -1, 0),
    glm::ivec3(-1, -1, 0),
    glm::ivec3(1, 1, 1),
    glm::ivec3(0, 1, 1),
    glm::ivec3(-1, 1, 1),
    glm::ivec3(1, 0, 1),
    glm::ivec3(-1, 0, 1),
    glm::ivec3(1, -1, 1),
    glm::ivec3(0, -1, 1),
    glm::ivec3(-1, -1, 1),
};

VoxelMesh::VoxelMesh()
    : bounds_min(-10000, 100000, -10000), bounds_max(10000, 10000, 10000)
{
    render_buffer_ = new VoxelMeshRenderBuffer(this);
}

void VoxelMesh::InternalSetVertices(const float *data, size_t length)
{
    this->vertices_.assign(data, data + length);
}

void VoxelMesh::InternalSetTexcoords(const float *data, size_t length)
{
    this->texcoords_.assign(data, data + length);
}

void VoxelMesh::InternalSetTriangles(const int32_t *data, size_t length)
{
    this->triangles_.assign(data, data + length);
}

void VoxelMesh::InternalVoxelise(const uint32_t desired_height)
{
    {
        ScopedTimer("Load Texture");
        this->texture.Load("/Users/lucasdower/ObjToSchematic/res/samples/skull.jpg");
    }

    {
        ScopedTimer("Voxelise");

        static const float mesh_desired_height = 8.0f;

        const float scale = desired_height / mesh_desired_height;
        const float y_offset = desired_height % 2 == 0 ? 0.5f : 0.0f;

        // Transform the vertices
        const size_t num_vertices = this->vertices_.size() / 3;
        for (size_t i = 0; i < num_vertices; ++i)
        {
            this->vertices_[i * 3 + 0] *= scale;
            this->vertices_[i * 3 + 1] *= scale;
            this->vertices_[i * 3 + 2] *= scale;

            this->vertices_[i * 3 + 1] += y_offset;
        }

        const size_t num_triangles = this->triangles_.size() / 6;
        for (size_t i = 0; i < num_triangles; ++i)
        {

            this->InternalVoxeliseTriangle(i);
        }
    }

    this->Finalise();
}

bool VoxelMesh::InternalIntersect(const Ray &ray, const glm::vec3 &v0, const glm::vec3 &v1, const glm::vec3 &v2, glm::vec3 &out_vertex)
{
    const glm::vec3 edge1(v1.x - v0.x, v1.y - v0.y, v1.z - v0.z);
    const glm::vec3 edge2(v2.x - v0.x, v2.y - v0.y, v2.z - v0.z);

    const glm::vec3 ray_direction(ray.direction == Direction::x, ray.direction == Direction::y, ray.direction == Direction::z);
    const glm::vec3 h(ray_direction.y * edge2.z - ray_direction.z * edge2.y,
                  ray_direction.z * edge2.x - ray_direction.x * edge2.z,
                  ray_direction.x * edge2.y - ray_direction.y * edge2.x);
    const float a = (edge1.x * h.x) + (edge1.y * h.y) + (edge1.z * h.z);

    static const float EPSILON = 1e-7;
    if (a > -EPSILON && a < EPSILON)
    {
        return false; // Ray is parallel to triangle
    }

    const float f = 1.0f / a;
    const glm::vec3 s(ray.origin.x - v0.x, ray.origin.y - v0.y, ray.origin.z - v0.z);
    const float u = f * (s.x * h.x + s.y * h.y + s.z * h.z);

    if (u < 0.0 || u > 1.0)
    {
        return false;
    }

    const glm::vec3 q(s.y * edge1.z - s.z * edge1.y,
                  s.z * edge1.x - s.x * edge1.z,
                  s.x * edge1.y - s.y * edge1.x);
    const float v = f * (ray_direction.x * q.x + ray_direction.y * q.y + ray_direction.z * q.z);

    if (v < 0.0 || u + v > 1.0)
    {
        return false;
    }

    const float t = f * (edge2.x * q.x + edge2.y * q.y + edge2.z * q.z);

    if (t > EPSILON)
    {
        out_vertex.x = ray.origin.x + (ray_direction.x * t);
        out_vertex.y = ray.origin.y + (ray_direction.y * t);
        out_vertex.z = ray.origin.z + (ray_direction.z * t);

        return true;
    }

    return false;
}

Vertex VoxelMesh::InternalGetVertex(const size_t pos_index, const size_t uv_index)
{
    float &x = this->vertices_[pos_index * 3 + 0];
    float &y = this->vertices_[pos_index * 3 + 1];
    float &z = this->vertices_[pos_index * 3 + 2];
    glm::vec3 pos(x, y, z);

    const float u = this->texcoords_[uv_index * 2 + 0];
    const float v = this->texcoords_[uv_index * 2 + 1];
    UV texcoord(u, v);

    Vertex vertex(pos, texcoord);

    return vertex;
}

void VoxelMesh::InternalVoxeliseTriangle(const size_t i)
{
    const Vertex& vertex_a = this->InternalGetVertex(this->triangles_[i * 6 + 0], this->triangles_[i * 6 + 3]);
    const Vertex& vertex_b = this->InternalGetVertex(this->triangles_[i * 6 + 1], this->triangles_[i * 6 + 4]);
    const Vertex& vertex_c = this->InternalGetVertex(this->triangles_[i * 6 + 2], this->triangles_[i * 6 + 5]);

    std::vector<Ray> ray_list;
    {
        glm::ivec3 bounds_min(0.0f, 0.0f, 0.0f);
        bounds_min.x = (int32_t)std::floor(std::min(std::min(vertex_a.position.x, vertex_b.position.x), vertex_c.position.x));
        bounds_min.y = (int32_t)std::floor(std::min(std::min(vertex_a.position.y, vertex_b.position.y), vertex_c.position.y));
        bounds_min.z = (int32_t)std::floor(std::min(std::min(vertex_a.position.z, vertex_b.position.z), vertex_c.position.z));

        glm::ivec3 bounds_max(0.0f, 0.0f, 0.0f);
        bounds_max.x = (int32_t)std::ceil(std::max(std::max(vertex_a.position.x, vertex_b.position.x), vertex_c.position.x));
        bounds_max.y = (int32_t)std::ceil(std::max(std::max(vertex_a.position.y, vertex_b.position.y), vertex_c.position.y));
        bounds_max.z = (int32_t)std::ceil(std::max(std::max(vertex_a.position.z, vertex_b.position.z), vertex_c.position.z));

        // Traverse x
        for (int32_t y = bounds_min.y; y <= bounds_max.y; ++y)
        {
            for (int32_t z = bounds_min.z; z <= bounds_max.z; ++z)
            {
                ray_list.push_back({glm::ivec3(bounds_min.x - 1, y, z),
                                    Direction::x});
            }
        }

        // Traverse y
        for (int32_t x = bounds_min.x; x <= bounds_max.x; ++x)
        {
            for (int32_t z = bounds_min.z; z <= bounds_max.z; ++z)
            {
                ray_list.push_back({glm::ivec3(x, bounds_min.y - 1, z),
                                    Direction::y});
            }
        }

        // Traverse z
        for (int32_t x = bounds_min.x; x <= bounds_max.x; ++x)
        {
            for (int32_t y = bounds_min.y; y <= bounds_max.y; ++y)
            {
                ray_list.push_back({glm::ivec3(x, y, bounds_min.z - 1),
                                    Direction::z});
            }
        }
    }

    glm::vec3 intersection(0.0f, 0.0f, 0.0f);
    for (const Ray &ray : ray_list)
    {
        const bool success = this->InternalIntersect(ray, vertex_a.position, vertex_b.position, vertex_c.position, intersection);
        if (success)
        {
            const glm::ivec3 voxel_position(
                (int32_t)std::round(intersection.x),
                (int32_t)std::round(intersection.y),
                (int32_t)std::round(intersection.z));

            //const RGBA voxel_colour = RGBA(1.0, 1.0, 1.0, 1.0);
            const RGBA voxel_colour = this->texture.GetColour(vertex_a.texcoord.u, 1.0f - vertex_a.texcoord.v);

            this->InternalAddVoxel(voxel_position, voxel_colour);
        }
    }
}

void VoxelMesh::InternalAddVoxel(const glm::ivec3 &position, const RGBA &colour)
{
    const size_t hash = Vec3::hash(position);

    const auto &tmp = this->voxel_hash_.find(hash);
    const bool is_new_voxel = tmp == this->voxel_hash_.end();
    if (is_new_voxel)
    {
        const size_t base_index = this->voxel_positions_.size() / 3;
        this->voxel_hash_[hash] = base_index;

        this->voxel_positions_.push_back(position.x);
        this->voxel_positions_.push_back(position.y);
        this->voxel_positions_.push_back(position.z);

        this->voxel_colours_.push_back(colour.r / 255.0); // TODO: Fix
        this->voxel_colours_.push_back(colour.g / 255.0);
        this->voxel_colours_.push_back(colour.b / 255.0);
        this->voxel_colours_.push_back(colour.a / 255.0);
    }
    else
    {
        const size_t base_index = tmp->second;
        /*
        this->voxel_colours_[base_index * 4 + 0] = (float)rand()/ 1.0; // TODO: Fix
        this->voxel_colours_[base_index * 4 + 1] = RANDF;
        this->voxel_colours_[base_index * 4 + 2] = 1.0;
        this->voxel_colours_[base_index * 4 + 3] = 1.0;
        */
    }
}

size_t VoxelMesh::InternalGetNeighbourIndex(const glm::ivec3 &neighbour) const
{
    return 9 * (neighbour.x + 1) + 3 * (neighbour.y + 1) + (neighbour.z + 1);
}

size_t &VoxelMesh::InternalGetNeighbours(const glm::ivec3 &neighbour)
{
    const size_t hash = Vec3::hash(neighbour);

    const auto &tmp = this->neighbour_map_.find(hash);
    const bool is_new_voxel = tmp == this->neighbour_map_.end();
    if (is_new_voxel)
    {
        this->neighbour_map_[hash] = 0;
        return this->neighbour_map_[hash];
    }
    else
    {
        return tmp->second;
    }
}

void VoxelMesh::Finalise()
{
    // Update bounds
    {
        ScopedTimer("Update Bounds");

        for (size_t i = 0; i < this->voxel_positions_.size(); ++i)
        {
            const int32_t x = this->voxel_positions_[i * 3 + 0];
            const int32_t y = this->voxel_positions_[i * 3 + 1];
            const int32_t z = this->voxel_positions_[i * 3 + 2];

            if (x < bounds_min.x)
            {
                bounds_min.x = x;
            }
            if (y < bounds_min.y)
            {
                bounds_min.y = y;
            }
            if (z < bounds_min.z)
            {
                bounds_min.z = z;
            }

            if (x > bounds_max.x)
            {
                bounds_max.x = x;
            }
            if (y > bounds_max.y)
            {
                bounds_max.y = y;
            }
            if (z > bounds_max.z)
            {
                bounds_max.z = z;
            }
        }
    }

    // Update neighbours
    {
        ScopedTimer("Update Neighbours");

        for (size_t i = 0; i < this->voxel_positions_.size(); ++i)
        {
            for (const glm::ivec3 &neighbour : NEIGHBOURS)
            {
                const int32_t x = this->voxel_positions_[i * 3 + 0];
                const int32_t y = this->voxel_positions_[i * 3 + 1];
                const int32_t z = this->voxel_positions_[i * 3 + 2];

                const glm::ivec3 neighbour_voxel(neighbour.x + x,
                                               neighbour.y + y,
                                               neighbour.z + z);
                const glm::ivec3 inverse_offset(-neighbour_voxel.x, -neighbour_voxel.y, -neighbour_voxel.z);
                const size_t inverse_index = this->InternalGetNeighbourIndex(inverse_offset);
                size_t &neighbours = this->InternalGetNeighbours(neighbour_voxel);
                neighbours |= (1 << inverse_index);
            }
        }
    }
}

RGBA VoxelMesh::GetVoxelColour(const Vertex& v0, const Vertex& v1, const Vertex& v2, const glm::vec3& position) const
{
    return RGBA(0, 0, 0, 0);
}