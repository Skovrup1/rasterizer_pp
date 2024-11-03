#pragma once

#include "core.hpp"
#include "triangle.hpp"
#include "vector.hpp"

constexpr uint32_t N_CUBE_VERTICES = 8;

constexpr uint32_t N_CUBE_FACES = 6 * 2; // 6 cube faces, 2 triangles per face

struct Mesh {
    std::vector<Vec3> vertex_buffer; // dynamic array of vertices
    std::vector<u32> index_buffer;  // dynamic array of vertex indexes
    std::vector<Vec2> uv_buffer;   // dynamic array of vertex uv
    std::vector<u32> uv_index_buffer;   // dynamic array of vertex color
    Vec3 rotation;
    Vec3 scale;
    Vec3 translate;
};

Mesh load_cube_mesh_data();

Mesh load_obj(const char *path);
