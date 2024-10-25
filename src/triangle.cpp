#include "triangle.hpp"
#include "display.hpp"

void fill_flat_bottom_triangle();
void fill_flat_top_triangle();

void draw_filled_triangle(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2,
                          u32 color) {
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
    }
    if (y1 > y2) {
        std::swap(y1, y2);
        std::swap(x1, x2);
    }
    if (y0 > y1) {
        std::swap(y0, y1);
        std::swap(x0, x1);
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
