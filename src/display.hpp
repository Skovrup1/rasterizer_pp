#pragma once

#include "core.hpp"

#define TARGET_FRAMETIME 1000.0 / 60.0

extern u32 window_width;
extern u32 window_height;

extern SDL_Window *window;
extern SDL_Renderer *renderer;

extern std::vector<u32> frame_buffer;
extern SDL_Texture *frame_buffer_texture;

bool initialize_window();
void destroy_window();
void draw_pixel(u32 x, u32 y, u32 color);
void draw_grid(u32 grid_x_spacing, u32 grid_y_spacing);
void draw_rect(i32 x, i32 y, u32 width, u32 height, u32 color);
void draw_line(i32 x0, i32 y0, i32 x1, i32 y1, u32 color);
void draw_line_b(i32 x0, i32 y0, i32 x1, i32 y1, u32 color);
void draw_triangle(i32 x0, i32 y0, i32 x1, i32 y1, i32 x2, i32 y2, u32 color);
void render_frame_buffer();
void clear_frame_buffer(u32 color);
