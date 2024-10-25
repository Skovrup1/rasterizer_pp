#include "core.hpp"
#include "vector.hpp"

typedef struct {
    int a;
    int b;
    int c;
} face;

typedef struct {
    Vec2 points[3];
    u32 color;
    f32 avg_depth;
} triangle;

void draw_filled_triangle(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, u32 color);

