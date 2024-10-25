#include "mesh.hpp"
#include "core.hpp"

Vec3 cube_vertices[N_CUBE_VERTICES] = {
    {-1, -1, -1}, // 1
    {-1, 1, -1},  // 2
    {1, 1, -1},   // 3
    {1, -1, -1},  // 4
    {1, 1, 1},    // 5
    {1, -1, 1},   // 6
    {-1, 1, 1},   // 7
    {-1, -1, 1},  // 8
};

face cube_faces[N_CUBE_FACES] = {
    // front
    {.a = 1, .b = 2, .c = 3},
    {.a = 1, .b = 3, .c = 4},
    // right
    {.a = 4, .b = 3, .c = 5},
    {.a = 4, .b = 5, .c = 6},
    // back
    {.a = 6, .b = 5, .c = 7},
    {.a = 6, .b = 7, .c = 8},
    // left
    {.a = 8, .b = 7, .c = 2},
    {.a = 8, .b = 2, .c = 1},
    // top
    {.a = 2, .b = 7, .c = 5},
    {.a = 2, .b = 5, .c = 3},
    // bottom
    {.a = 6, .b = 8, .c = 1},
    {.a = 6, .b = 1, .c = 4}};

u32 cube_colors[N_CUBE_FACES] = {
    0xffff0000, 0xffff0000, 0xff00ff00, 0xff00ff00, 0xff0000ff, 0xff0000ff,
    0xffffff00, 0xffffff00, 0xffff00ff, 0xffff00ff, 0xff00ffff, 0xff00ffff,
};

Mesh load_cube_mesh_data() {
    Mesh new_mesh{};
    for (u32 i = 0; i < N_CUBE_VERTICES; i++) {
        Vec3 cube_vertex = cube_vertices[i];
        new_mesh.vertex_buffer.push_back(cube_vertex);
    }
    for (u32 i = 0; i < N_CUBE_FACES; i++) {
        face cube_face = cube_faces[i];
        new_mesh.index_buffer.push_back(cube_face);
    }
    for (u32 i = 0; i < N_CUBE_FACES; i++) {
        u32 cube_color = cube_colors[i];
        new_mesh.color_buffer.push_back(cube_color);
    }

    new_mesh.scale = Vec3f{1, 1, 1};

    return new_mesh;
}

struct Token {
    enum Type {
        UNINITIALIZED,
        VERTEX,  // v
        NORMAL,  // vn
        UV,      // vt
        FACE,    // f
        SURFACE, // s
        OBJECT,  // o
        GROUP,   // g
        USEMTL,  // usemtl
        MTLLIB,  // mtllib
        INT,     // 0
        FLOAT,   // 0.0
        IDENTIFIER,
    } tag;
    typedef enum Type type;

    union {
        u32 u;
        i32 i;
        f32 f;
    };
};

std::vector<char> buffer;
std::vector<Token> token_buffer;
u32 cursor = 0;
i32 cursor_start = 0;
i32 cursor_line = 1;
i32 cursor_prev_line = 0;

std::vector<const char *> id_buffer;

 bool is_at_end() { return cursor >= buffer.size(); }

 char advance() { return buffer[cursor++]; }

 void add_token_i(Token::Type tag, i32 i) {
    Token token = {.tag = tag, .i = i};
    token_buffer.push_back(token);
}

 void add_token_f(Token::Type tag, f32 f) {
    Token token = {.tag = tag, .f = f};
    token_buffer.push_back(token);
}

 void add_token_u(Token::Type tag, u32 u) {
    Token token = {.tag = tag, .u = u};
    token_buffer.push_back(token);
}

 void add_token(Token::Type tag) { add_token_u(tag, 0); }

/* bool match(char expected) {
    if (is_at_end()) {
        return false;
    }
    if (buffer[cursor] != expected) {
        return false;
    }

    cursor++;
    return true;
}*/

 char peek() {
    if (is_at_end()) {
        return '\0';
    }

    return buffer[cursor];
}

 char peek_next() {
    if (cursor + 1 >= buffer.size()) {
        return '\0';
    }

    return buffer[cursor + 1];
}

 void number() {
    while (isdigit(peek())) {
        advance();
    }

    bool is_float = false;
    if (peek() == '.' && isdigit(peek_next())) {
        advance();
        is_float = true;

        while (isdigit(peek())) {
            advance();
        }
    }

    std::string num(buffer.begin() + cursor_start, buffer.begin() + cursor);
    if (is_float) {
        f32 f = std::stof(num);
        add_token_f(Token::FLOAT, f);
    } else {
        i32 i = std::stoi(num);
        add_token_i(Token::FLOAT, i);
    }
}

 void keyword() {
    while (isalpha(peek())) {
        advance();
    }

    if (isgraph(peek())) {
        advance();

        while (isgraph(peek())) {
            advance();
        }
    }

    std::string word(buffer.begin() + cursor_start, buffer.begin() + cursor);

    const char *keyword[] = {"v", "vn", "vt",     "f",     "s",
                             "o", "g",  "usemtl", "mtllib"};
    u32 i;
    for (i = 0; i < std::size(keyword); i++) {
        const char *kword = keyword[i];
        if (kword == word) {
            add_token(static_cast<Token::Type>(i + 1));
            return;
        };
    }

    id_buffer.push_back(word.data());
    add_token_u(Token::IDENTIFIER, id_buffer.size() - 1);
}

 void scan_token() {
    char c = advance();
    switch (c) {
    case '#':
        while (peek() != '\n' && !is_at_end()) {
            c = advance();
        }
        break;
    case '\n':
        cursor_line++;
        cursor_prev_line = cursor;
        break;
    case ' ':
    case '/':
    case '\r':
    case '\t':
        // ignore these characters
        break;
    default:
        if (c == '-' || isdigit(c)) {
            number();
        } else if (isalpha(c)) {
            keyword();
        } else {
            printf("Error: unexpected character c='%c' at %d,%d\n", c,
                   cursor_line, cursor - cursor_prev_line);
        }
    }
    return;
}

/* void print_tokens() {
    i32 num = 0;
    i32 num_len = 3;
    Token::Type prev_tag = Token::UNINITIALIZED;
    for (u32 i = 0; i < token_buffer.size(); i++) {
        if (num == num_len) {
            printf("\n");
            num = 0;
        }
        switch (token_buffer[i].tag) {
        case Token::UNINITIALIZED:
            break;
        case Token::FACE:
            printf("FACE ");
            num_len = 9;
            break;
        case Token::NORMAL:
            printf("NORMAL ");
            num_len = 3;
            break;
        case Token::INT:
            if (prev_tag == Token::SURFACE) {
                printf("%d\n", token_buffer[i].i);
            } else {
                printf("%3d ", token_buffer[i].i);
                num++;
            }
            break;
        case Token::FLOAT:
            printf("%f ", token_buffer[i].f);
            num++;
            break;
        case Token::UV:
            printf("UV ");
            num_len = 2;
            break;
        case Token::VERTEX:
            printf("VERTEX ");
            num_len = 3;
            break;
        case Token::SURFACE:
            printf("SURFACE ");
            break;
        case Token::OBJECT:
            printf("OBJECT ");
            break;
        case Token::GROUP:
            printf("GROUP ");
            break;
        case Token::USEMTL:
            printf("USEMTL ");
            break;
        case Token::MTLLIB:
            printf("MTLLIB ");
            break;
        case Token::IDENTIFIER:
            printf("%s\n", id_buffer[token_buffer[i].u]);
            break;
        }
        prev_tag = token_buffer[i].tag;
    }
    printf("\n");
    fflush(stdout);
}*/

Mesh load_obj(const char *path) {
    std::ifstream file{path, std::ios::ate | std::ios::binary};
    if (!file.is_open()) {
        printf("Error: failed to open %s\n", path);
    }

    i32 file_size = file.tellg();

    // rewind to start of file
    file.seekg(0);

    buffer.resize(file_size);
    file.read(buffer.data(), file_size);

    file.close();

    while (!is_at_end()) {
        cursor_start = cursor;
        scan_token();
    }

    Mesh new_mesh{};
    for (usize i = 0; i < token_buffer.size(); i++) {
        switch (token_buffer[i].tag) {
        case Token::VERTEX:
            new_mesh.vertex_buffer.push_back({token_buffer[i + 1].f,
                                              token_buffer[i + 2].f,
                                              token_buffer[i + 3].f});
            i += 3;
            break;
        case Token::FACE:
            new_mesh.index_buffer.push_back({.a = token_buffer[i + 1].i,
                                             .b = token_buffer[i + 4].i,
                                             .c = token_buffer[i + 7].i});
            i += 7;
        default:
            break;
        }
    }

    return new_mesh;
}
