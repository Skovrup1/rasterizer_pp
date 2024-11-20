#include "display.hpp"

u32 window_width = 800;
u32 window_height = 600;

SDL_Window *window = NULL;
SDL_Renderer *renderer = NULL;

std::vector<u32> frame_buffer;
SDL_Texture *frame_buffer_texture = NULL;

bool initialize_window() {
    // Init SDL
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        fprintf(stderr, "Error SDL_Init()\n");
        return false;
    }

    // Get all displays
    i32 count;
    SDL_DisplayID *ids = SDL_GetDisplays(&count);

    // Pick first display
    SDL_DisplayID display = ids[0];

    // Query window capabilities
    const SDL_DisplayMode *display_mode = SDL_GetCurrentDisplayMode(display);
    if (!display_mode) {
        fprintf(stderr, "Error SDL_GetCurrentDisplayMode() %s\n",
                SDL_GetError());
    }
    window_width = display_mode->w;
    window_height = display_mode->h;

    // Create SDL window
    SDL_WindowFlags flags = SDL_WINDOW_BORDERLESS;
    SDL_Window *window =
        SDL_CreateWindow(NULL, window_width, window_height, flags);
    if (!window) {
        fprintf(stderr, "Error SDL_CreateWindow() %s", SDL_GetError());
        return false;
    }

    SDL_SetWindowFullscreen(window, true);

    // Create SDL renderer
    renderer = SDL_CreateRenderer(window, NULL);
    if (!renderer) {
        fprintf(stderr, "Error SDL_CreateRenderer() %s", SDL_GetError());
    }

    return true;
}

void destroy_window() {
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void draw_pixel(const u32 x, const u32 y, const u32 color) {
    assert(x >= 0 && y >= 0 && x <= window_width && y <= window_height);

    frame_buffer[x + y * window_width] = color;
}

void draw_grid(const u32 grid_x_spacing, const u32 grid_y_spacing) {
    // const u32 dot_x_spacing = grid_x_spacing / 2;
    // const u32 dot_y_spacing = grid_y_spacing / 2;
    const u32 color = 0xff333333;

    /*
    for (size_t y = dot_y_spacing; y < window_height; y += dot_y_spacing * 2) {
      for (size_t x = dot_x_spacing; x < window_width; x += dot_x_spacing * 2) {
        frame_buffer[x + y * window_width] = color;
      }
    }
    */

    for (size_t y = 0; y < window_height; y++) {
        for (size_t x = 0; x < window_width; x++) {
            if (x % grid_x_spacing == 0 || y % grid_y_spacing == 0) {
                frame_buffer[x + y * window_width] = color;
            }
        }
    }
}

void draw_rect(i32 x, i32 y, u32 width, u32 height, u32 color) {
    for (size_t j = y; j < y + height; j++) {
        for (size_t i = x; i < x + width; i++) {
            draw_pixel(i - width / 2, j - height / 2, color);
        }
    }
}

// dda algorithm
void draw_line(i32 x0, i32 y0, i32 x1, i32 y1, u32 color) {
    i32 dx = x1 - x0;
    i32 dy = y1 - y0;

    i32 side_len = abs(dx) > abs(dy) ? abs(dx) : abs(dy);

    f32 x_inc = dx / (f32)side_len;
    f32 y_inc = dy / (f32)side_len;

    f32 x_acc = x0;
    f32 y_acc = y0;
    for (i32 i = 0; i <= side_len; i++) {
        draw_pixel(round(x_acc), round(y_acc), color);

        x_acc += x_inc;
        y_acc += y_inc;
    }
}

// bresenham's algorithm
void draw_line_b(i32 x0, i32 y0, i32 x1, i32 y1, u32 color) {
    i32 x = x0;
    i32 y = y0;
    i32 dx = abs(x1 - x0);
    i32 dy = abs(y1 - y0);
    i32 x_inc = x1 > x0 ? 1 : -1;
    i32 y_inc = y1 > y0 ? 1 : -1;

    if (dx >= dy) {
        i32 p = 2 * dy - dx;
        for (i32 i = 0; i <= dx; i++) {
            draw_pixel(x, y, color);

            if (p >= 0) {
                y += y_inc;
                p -= 2 * dx;
            }

            x += x_inc;
            p += 2 * dy;
        }
    } else {
        i32 p = 2 * dx - dy;
        for (i32 i = 0; i <= dy; i++) {
            draw_pixel(x, y, color);

            if (p >= 0) {
                x += x_inc;
                p -= 2 * dy;
            }

            y += y_inc;
            p += 2 * dx;
        }
    }
}

void render_frame_buffer() {
    SDL_UpdateTexture(frame_buffer_texture, NULL, frame_buffer.data(),
                      window_width * sizeof(u32));
    SDL_RenderTexture(renderer, frame_buffer_texture, NULL, NULL);
}

void clear_frame_buffer(u32 color) {
    for (size_t y = 0; y < window_height; y++) {
        for (size_t x = 0; x < window_width; x++) {
            frame_buffer[x + y * window_width] = color;
        }
    }
}
