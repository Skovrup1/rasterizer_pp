#pragma once

#include "core.hpp"
#include "triangle.hpp"
#include "vector.hpp"

struct Mesh {
    std::vector<Vec3> vertex_buffer;  // dynamic array of vertices
    std::vector<u32> index_buffer;    // dynamic array of vertex indexes
    std::vector<Vec2> uv_buffer;      // dynamic array of vertex uv
    std::vector<u32> uv_index_buffer; // dynamic array of vertex color
    Vec3 rotation = {0, 0, 0};
    Vec3 scale = {1, 1, 1};
    Vec3 translate = {0, 0, 0};
};

Mesh load_cube_mesh_data();

Mesh load_obj(const char *path);

// debug function
void print_tokens();
