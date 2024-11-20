#include "core.hpp"
#include "vector.hpp"
#include <cstdlib>

typedef struct {
    int a;
    int b;
    int c;
} face;

typedef struct {
    Vec4 points[3];
    Vec2 uv[3];
    u32 color;
    f32 avg_depth;
} triangle;

void draw_triangle(i32 x0, i32 y0, //
                   i32 x1, i32 y1, //
                   i32 x2, i32 y2, u32 color);
void draw_filled_triangle(i32 x0, i32 y0, //
                          i32 x1, i32 y1, //
                          i32 x2, i32 y2, u32 color);
void draw_textured_triangle(i32 x0, i32 y0, f32 z0, f32 w0, f32 u0, f32 v0, //
                            i32 x1, i32 y1, f32 z1, f32 w1, f32 u1, f32 v1, //
                            i32 x2, i32 y2, f32 z2, f32 w2, f32 u2, f32 v2,
                            const u32 *texture);

Vec3 barycentric_weights(Vec2 a, Vec2 b, Vec2 c, Vec2 p);
