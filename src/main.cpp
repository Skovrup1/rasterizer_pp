#include "core.hpp"
#include "display.hpp"
#include "matrix.hpp"
#include "mesh.hpp"
#include "texture.hpp"
#include "vector.hpp"

u64 counter_frequency;
u64 frame_start;
u64 frame_end;
f64 frame_time;

bool is_running = false;

Mesh mesh;
const u32 *mesh_texture;

std::vector<triangle> triangles_to_render;

Vec3 camera_position = {0, 0, 0};
Mat4x4f proj_matrix;

enum RenderMode {
    FILL = 0b1,
    FILL_WIREFRAME = 0b10,
    WIREFRAME = 0b100,
    WIREFRAME_REDDOT = 0b1000,
    TEXTURED = 0b10000,
    TEXTURED_WIREFRAME = 0b100000,
    //... = 0x10000000,
    //... = 0x100000000,
};

RenderMode render_mode = RenderMode::TEXTURED_WIREFRAME;

u32 fill_color = 0xffdddddd;
u32 wireframe_color = 0xff000000;
u32 dot_color = 0xffff0000;

bool use_color = true;
bool cull_mode = true;

Vec3 light = {0.0, 0.0, 1.0};

void setup() {
    counter_frequency = SDL_GetPerformanceFrequency() / 1000;

    frame_buffer = std::vector<u32>();
    frame_buffer.resize(sizeof(u32) * window_width * window_height);

    frame_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             window_width, window_height);
    f32 fov = std::numbers::pi / 3.0;
    f32 aspect = window_height / static_cast<float>(window_width);
    f32 near = 0.1;
    f32 far = 100;
    proj_matrix = Mat4x4f::perspective(fov, aspect, near, far);

    mesh = load_cube_mesh_data();
    mesh_texture = reinterpret_cast<const u32 *>(REDBRICK_TEXTURE);
}

void input() {
    SDL_Event event;
    SDL_PollEvent(&event);

    switch (event.type) {
    case SDL_EVENT_QUIT:
        is_running = false;
        break;
    case SDL_EVENT_KEY_DOWN:
        switch (event.key.key) {
        case SDLK_ESCAPE:
            is_running = false;
            break;
        case SDLK_1:
            render_mode = RenderMode::WIREFRAME_REDDOT;
            wireframe_color = 0xff00ff00;
            break;
        case SDLK_2:
            render_mode = RenderMode::WIREFRAME;
            wireframe_color = 0xff00ff00;
            break;
        case SDLK_3:
            render_mode = RenderMode::FILL;
            break;
        case SDLK_4:
            render_mode = RenderMode::FILL_WIREFRAME;
            wireframe_color = 0xff000000;
            break;
        case SDLK_5:
            render_mode = RenderMode::TEXTURED;
            break;
        case SDLK_6:
            render_mode = RenderMode::TEXTURED_WIREFRAME;
            wireframe_color = 0xff000000;
            break;
        case SDLK_C:
            cull_mode = true;
            break;
        case SDLK_D:
            cull_mode = false;
            break;
        case SDLK_V:
            use_color = true;
            break;
        case SDLK_F:
            use_color = false;
            break;
        }
    }
}

u32 light_apply_intensity(u32 color, f32 factor) {
    if (factor < 0) {
        factor = 0;
    }
    if (factor > 1) {
        factor = 1;
    }

    u32 a = (color & 0xff000000);
    u32 r = (color & 0x00ff0000) * factor;
    u32 g = (color & 0x0000ff00) * factor;
    u32 b = (color & 0x000000ff) * factor;

    u32 new_color = a | (r & 0x00ff0000) | (g & 0x0000ff00) | (b & 0x000000ff);

    return new_color;
}

void update() {
    mesh.translate.z = 5;
    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    const Mat4x4f world_matrix =
        Mat4x4f::identity() *
        Mat4x4f::scale(mesh.scale.x, mesh.scale.y, mesh.scale.z) *
        Mat4x4f::rotation_x(mesh.rotation.x) *
        Mat4x4f::rotation_y(mesh.rotation.y) *
        Mat4x4f::rotation_z(mesh.rotation.z) *
        Mat4x4f::translate(mesh.translate.x, mesh.translate.y,
                           mesh.translate.z);

    size_t vertices = mesh.index_buffer.size();
    for (size_t i = 0; i < vertices - 2; i += 3) {
        Vec2 face_uv[3] = {mesh.uv_buffer[mesh.uv_index_buffer[i]],
                           mesh.uv_buffer[mesh.uv_index_buffer[i + 1]],
                           mesh.uv_buffer[mesh.uv_index_buffer[i + 2]]};

        Vec3 face_vertices[3] = {
            mesh.vertex_buffer[mesh.index_buffer[i]],
            mesh.vertex_buffer[mesh.index_buffer[i + 1]],
            mesh.vertex_buffer[mesh.index_buffer[i + 2]],
        };

        Vec4 transformed_vertices[3];
        for (int j = 0; j < 3; j++) {
            Vec4 transformed_vertex = Vec4{face_vertices[j]};

            transformed_vertices[j] = world_matrix * transformed_vertex;
        }

        Vec3 a = transformed_vertices[0];
        Vec3 b = transformed_vertices[1];
        Vec3 c = transformed_vertices[2];

        Vec3 ab = b - a;
        Vec3 ac = c - a;
        ab = norm(ab);
        ac = norm(ac);

        Vec3 normal = cross(ab, ac);
        normal = norm(normal);

        // vector between a and camera
        Vec3 cam_ray = camera_position - a;

        // backface culling
        if (cull_mode) {
            float alignment = dot(normal, cam_ray);
            if (alignment < 0) {
                continue;
            }
        }

        f32 avg_depth = (a.z + b.z + c.z) / 3;

        f32 factor = -dot(normal, light);
        u32 color = fill_color;
        color = light_apply_intensity(color, factor);

        Vec4 proj_points[3];
        for (uint32_t j = 0; j < 3; j++) {
            proj_points[j] = project(proj_matrix, transformed_vertices[j]);

            // invert in y
            proj_points[j].y *= -1;
            //
            // scale into view
            proj_points[j].x *= window_width / 2.0;
            proj_points[j].y *= window_height / 2.0;

            // translate to middle of screen
            proj_points[j].x += window_width / 2.0;
            proj_points[j].y += window_height / 2.0;
        }

        triangle projected_triangle = {
            .points = {proj_points[0], proj_points[1], proj_points[2]},
            .uv = {face_uv[0], face_uv[1], face_uv[2]},
            .color = color,
            .avg_depth = avg_depth,
        };

        triangles_to_render.push_back(projected_triangle);
    }

    // sort front to back
    std::sort(
        triangles_to_render.begin(), triangles_to_render.end(),
        [&](triangle a, triangle b) { return a.avg_depth > b.avg_depth; });
}

void render() {
    draw_grid(40, 40);

    u32 num_triangles = triangles_to_render.size();
    for (size_t i = 0; i < num_triangles; i++) {
        triangle triangle = triangles_to_render[i];

        if (render_mode & RenderMode::WIREFRAME_REDDOT) {
            draw_rect(triangle.points[0].x, triangle.points[0].y, 4, 4,
                      dot_color);
            draw_rect(triangle.points[1].x, triangle.points[1].y, 4, 4,
                      dot_color);
            draw_rect(triangle.points[2].x, triangle.points[2].y, 4, 4,
                      dot_color);
        }

        if (render_mode & (RenderMode::FILL_WIREFRAME | RenderMode::FILL)) {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                                 triangle.points[1].x, triangle.points[1].y,
                                 triangle.points[2].x, triangle.points[2].y,
                                 triangle.color);
        }

        if (render_mode &
            (RenderMode::TEXTURED | RenderMode::TEXTURED_WIREFRAME)) {
            draw_textured_triangle(triangle.points[0].x, triangle.points[0].y,
                                   triangle.points[0].z,
                                   triangle.points[0].w,               //
                                   triangle.uv[0].r, triangle.uv[0].g, //
                                   triangle.points[1].x, triangle.points[1].y,
                                   triangle.points[1].z,
                                   triangle.points[1].w,               //
                                   triangle.uv[1].r, triangle.uv[1].g, //
                                   triangle.points[2].x, triangle.points[2].y,
                                   triangle.points[2].z,
                                   triangle.points[2].w,               //
                                   triangle.uv[2].r, triangle.uv[2].g, //
                                   mesh_texture);
        }

        if (render_mode &
            (RenderMode::FILL_WIREFRAME | RenderMode::WIREFRAME |
             RenderMode::WIREFRAME_REDDOT | RenderMode::TEXTURED_WIREFRAME)) {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          wireframe_color);
        }
    }

    triangles_to_render.clear();

    render_frame_buffer();

    clear_frame_buffer(0xff222222);

    SDL_RenderPresent(renderer);
}

int main() {
    is_running = initialize_window();

    setup();

    while (is_running) {
        frame_start = SDL_GetPerformanceCounter();

        input();
        update();
        render();

        frame_end = SDL_GetPerformanceCounter();
        frame_time = (frame_end - frame_start) / (f64)counter_frequency;

        // sleep for most of the time
        if (frame_time < TARGET_FRAMETIME) {
            SDL_Delay((TARGET_FRAMETIME - frame_time) * 0.9);
        }

        // busy wait until target frame time is hit
        while (frame_time < TARGET_FRAMETIME) {
            frame_end = SDL_GetPerformanceCounter();
            frame_time = (frame_end - frame_start) / (f64)counter_frequency;
        }
    }

    destroy_window();

    return 0;
}
