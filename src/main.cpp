#include "core.hpp"
#include "display.hpp"
#include "matrix.hpp"
#include "mesh.hpp"
#include "vector.hpp"

u64 counter_frequency;

bool is_running = false;

Mesh mesh;

std::vector<triangle> triangles_to_render;

Vec3 camera_position = {0, 0, 0};

const f32 fov_factor = 640;

enum RenderMode {
    FILL = 0x1,
    FILL_WIREFRAME = 0x10,
    WIREFRAME = 0x1000,
    WIREFRAME_REDDOT = 0x10000,
};

u32 fill_color = 0xffdddddd;
u32 wireframe_color = 0xff00ff00;
u32 dot_color = 0xffff0000;

RenderMode render_mode = RenderMode::WIREFRAME;

bool use_color = true;
bool cull_mode = true;

void setup() {
    counter_frequency = SDL_GetPerformanceFrequency() / 1000;

    frame_buffer.reserve(sizeof(u32) * window_width * window_height);

    frame_buffer_texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                             SDL_TEXTUREACCESS_STREAMING,
                                             window_width, window_height);
    mesh = load_cube_mesh_data();
    // test_mesh = load_obj("assets/f22.obj");
}

void process_input() {
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
            wireframe_color = 0xff000000;
            break;
        case SDLK_4:
            render_mode = RenderMode::FILL_WIREFRAME;
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

// project a 3d vector to a 2d vector
Vec2 project(Vec3 p) {
    Vec2 proj_p = {
        .x = fov_factor * p.x / p.z,
        .y = fov_factor * p.y / p.z,
    };

    return proj_p;
}

void update() {
    mesh.translate.z = 5;
    mesh.rotation.x += 0.01;
    mesh.rotation.y += 0.01;
    mesh.rotation.z += 0.01;

    u32 num_faces = mesh.index_buffer.size();
    for (size_t i = 0; i < num_faces; i++) {
        face mesh_face = mesh.index_buffer[i];
        u32 face_color = mesh.color_buffer[i];

        Vec3 face_vertices[3];
        face_vertices[0] = mesh.vertex_buffer[mesh_face.a - 1];
        face_vertices[1] = mesh.vertex_buffer[mesh_face.b - 1];
        face_vertices[2] = mesh.vertex_buffer[mesh_face.c - 1];

        Vec3 transformed_vertices[3];
        for (int j = 0; j < 3; j++) {
            Vec4 transformed_vertex = Vec4{face_vertices[j]};

            const Mat4x4f world_matrix =
                Mat4x4f::identity() *
                Mat4x4f::scale(mesh.scale.x, mesh.scale.y, mesh.scale.z) *
                Mat4x4f::rotation_x(mesh.rotation.x) *
                Mat4x4f::rotation_y(mesh.rotation.y) *
                Mat4x4f::rotation_z(mesh.rotation.z) *
                Mat4x4f::translate(mesh.translate.x, mesh.translate.y,
                                   mesh.translate.z);

            transformed_vertices[j] = world_matrix * transformed_vertex;
        }

        // backface culling
        if (cull_mode) {
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

            float alignment = dot(normal, cam_ray);
            if (alignment < 0) {
                continue;
            }
        }

        f32 avg_depth = (transformed_vertices[0].z + transformed_vertices[1].z +
                         transformed_vertices[2].z) /
                        3;

        triangle projected_triangle = {.color = face_color,
                                       .avg_depth = avg_depth};
        for (int j = 0; j < 3; j++) {
            Vec2 projected_point = project(transformed_vertices[j]);

            // scale and translate to middle of screen
            projected_point.x += window_width / 2.0;
            projected_point.y += window_height / 2.0;

            projected_triangle.points[j] = projected_point;
        }

        triangles_to_render.push_back(projected_triangle);
    }

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

        if (render_mode & (RenderMode::FILL_WIREFRAME | RenderMode::WIREFRAME |
                           RenderMode::WIREFRAME_REDDOT)) {
            draw_triangle(triangle.points[0].x, triangle.points[0].y,
                          triangle.points[1].x, triangle.points[1].y,
                          triangle.points[2].x, triangle.points[2].y,
                          wireframe_color);
        }

        u32 render_color = use_color ? triangle.color : fill_color;

        if (render_mode & (RenderMode::FILL_WIREFRAME | RenderMode::FILL)) {
            draw_filled_triangle(triangle.points[0].x, triangle.points[0].y,
                                 triangle.points[1].x, triangle.points[1].y,
                                 triangle.points[2].x, triangle.points[2].y,
                                 render_color);
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
        u64 frame_start = SDL_GetPerformanceCounter();

        process_input();
        update();
        render();

        u64 frame_end = SDL_GetPerformanceCounter();
        f64 frame_time = (frame_end - frame_start) / (f64)counter_frequency;

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
