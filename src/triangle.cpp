#include "triangle.hpp"
#include "display.hpp"

void draw_triangle(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, u32 color) {
    draw_line_b(x0, y0, x1, y1, color);
    draw_line_b(x1, y1, x2, y2, color);
    draw_line_b(x2, y2, x0, y0, color);
}

void draw_filled_triangle(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2,
                          u32 color) {
    if (y0 >= y2) {
        std::swap(y0, y2);
        std::swap(x0, x2);
    }
    if (y0 >= y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }
    if (y1 >= y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
    }

    i32 my = y1;
    i32 mx = static_cast<f32>((x2 - x0) * (y1 - y0)) / (y2 - y0) + x0;

    // fill_flat_bottom_triangle();
    {
        // y slope
        f32 slope_1 = static_cast<f32>(x1 - x0) / (y1 - y0);
        f32 slope_2 = static_cast<f32>(mx - x0) / (my - y0);

        if (slope_1 > slope_2) {
            std::swap(slope_1, slope_2);
        }

        f32 x_start = x0;
        f32 x_end = x0;

        for (i32 y = y0; y <= my; y++) {
            for (i32 x = x_start; x <= x_end; x++) {
                draw_pixel(x, y, color);
            }
            x_start += slope_1;
            x_end += slope_2;
        }
    }

    // fill_flat_top_triangle();
    {
        // y slope
        f32 slope_1 = static_cast<f32>(x2 - x1) / (y2 - y1);
        f32 slope_2 = static_cast<f32>(x2 - mx) / (y2 - my);

        if (slope_1 < slope_2) {
            std::swap(slope_1, slope_2);
        }

        f32 x_start = x2;
        f32 x_end = x2;

        for (i32 y = y2; y >= y1; y--) {
            for (i32 x = x_start; x <= x_end; x++) {
                draw_pixel(x, y, color);
            }
            x_start -= slope_1;
            x_end -= slope_2;
        }
    }
}

void draw_texel(i32 x, i32 y, Vec2 a, Vec2 b, Vec2 c, f32 u0, f32 v0, f32 u1,
                f32 v1, f32 u2, f32 v2, const u32 *texture) {
    Vec2 p = {static_cast<f32>(x), static_cast<f32>(y)};
    Vec3 weights = barycentric_weights(a, b, c, p);

    f32 alpha = weights.x;
    f32 beta = weights.y;
    f32 gamma = weights.z;

    f32 interpolated_u = u0 * alpha + u1 * beta + u2 * gamma;
    f32 interpolated_v = v0 * alpha + v1 * beta + v2 * gamma;

    const u32 texture_width = 64;
    const u32 texture_height = 64;
    i32 tex_x = abs(interpolated_u * texture_width);
    i32 tex_y = abs(interpolated_v * texture_height);

    usize i = texture_width * tex_y + tex_x;

    if (i < texture_width * texture_height) {
        draw_pixel(x, y, texture[i]);
    }
}

void draw_textured_triangle(i32 x0, i32 y0, f32 u0, f32 v0, //
                            i32 x1, i32 y1, f32 u1, f32 v1, //
                            i32 x2, i32 y2, f32 u2, f32 v2,
                            const u32 *texture) {
    if (y0 >= y2) {
        std::swap(y0, y2);
        std::swap(x0, x2);
        std::swap(u0, u2);
        std::swap(v0, v2);
    }
    if (y0 >= y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
        std::swap(u0, u1);
        std::swap(v0, v1);
    }
    if (y1 >= y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
        std::swap(u1, u2);
        std::swap(v1, v2);
    }

    Vec2 a = {static_cast<f32>(x0), static_cast<f32>(y0)};
    Vec2 b = {static_cast<f32>(x1), static_cast<f32>(y1)};
    Vec2 c = {static_cast<f32>(x2), static_cast<f32>(y2)};

    i32 my = y1;
    i32 mx = static_cast<f32>((x2 - x0) * (y1 - y0)) / (y2 - y0) + x0;
    // fill_flat_bottom_triangle();
    {
        // y slope
        f32 slope_1 = static_cast<f32>(x1 - x0) / (y1 - y0);
        f32 slope_2 = static_cast<f32>(mx - x0) / (my - y0);

        if (slope_1 > slope_2) {
            std::swap(slope_1, slope_2);
        }

        f32 x_start = x0;
        f32 x_end = x0;

        for (i32 y = y0; y <= my; y++) {
            for (i32 x = x_start; x <= x_end; x++) {
                draw_texel(x, y, a, b, c, u0, v0, u1, v1, u2, v2, texture);
            }
            x_start += slope_1;
            x_end += slope_2;
        }
    }
    // fill_flat_top_triangle();
    {
        // y slope
        f32 slope_1 = static_cast<f32>(x2 - x1) / (y2 - y1);
        f32 slope_2 = static_cast<f32>(x2 - mx) / (y2 - my);

        if (slope_1 < slope_2) {
            std::swap(slope_1, slope_2);
        }

        f32 x_start = x2;
        f32 x_end = x2;

        for (i32 y = y2; y >= y1; y--) {
            for (i32 x = x_start; x <= x_end; x++) {
                draw_texel(x, y, a, b, c, u0, v0, u1, v1, u2, v2, texture);
            }
            x_start -= slope_1;
            x_end -= slope_2;
        }
    }
}

Vec3 barycentric_weights(Vec2 a, Vec2 b, Vec2 c, Vec2 p) {
    Vec2 ac = c - a;
    Vec2 ab = b - a;
    Vec2 ap = p - a;
    Vec2 pc = c - p;
    Vec2 pb = b - p;

    // 2d cross product
    f32 area_parallelogram_abc = ac.x * ab.y - ac.y * ab.x;
    f32 area_parallelogram_pbc = pc.x * pb.y - pc.y * pb.x;
    f32 area_parallelogram_apc = ac.x * ap.y - ac.y * ap.x;

    f32 alpha = area_parallelogram_pbc / area_parallelogram_abc;
    f32 beta = area_parallelogram_apc / area_parallelogram_abc;
    f32 gamma = 1 - alpha - beta;

    return {alpha, beta, gamma};
}
