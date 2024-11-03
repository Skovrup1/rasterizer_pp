#pragma once

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wmissing-braces"

#include "core.hpp"

struct Vec2f;
struct Vec3f;
struct Vec4f;

struct Vec2f {
    union {
        struct {
            f32 x, y;
        };
        struct {
            f32 r, g;
        };
        f32 data[2];
    };

    Vec2f() {}

    Vec2f(std::initializer_list<f32> list) {
        usize i = 0;
        for (; i < list.size(); i++) {
            data[i] = list.begin()[i];
        }
        for (; i < size(); i++) {
            data[i] = 0;
        }
    }

    Vec2f(Vec4f v);

    static constexpr usize size() { return 2; }

    f32 &operator[](usize i) { return data[i]; }

    Vec2f operator+(Vec2f v) { return {x + v.x, y + v.y}; }

    Vec2f operator-(Vec2f v) { return {x - v.x, y - v.y}; }

    Vec2f operator*(Vec2f v) { return {x * v.x, y * v.x}; }

    Vec2f operator*(f32 s) { return {x * s, y * s}; }

    Vec2f operator/(Vec2f v) { return {x / v.x, y / v.y}; }

    Vec2f operator/(f32 s) { return {x / s, y / s}; }

    friend f32 len(Vec2f v) { return sqrt(len_squared(v)); }

    friend f32 len_squared(Vec2f v) { return v.x * v.x + v.y * v.y; }

    friend f32 dot(Vec2f a, Vec2f b) { return a.x * b.x + a.y * b.y; }

    friend Vec2f norm(Vec2f v) { return v / len(v); }
};

struct Vec3f {
    union {
        struct {
            f32 x, y, z;
        };
        struct {
            f32 r, g, b;
        };
        f32 data[3];
    };

    Vec3f() {}

    Vec3f(std::initializer_list<f32> list) {
        usize i = 0;
        for (; i < list.size(); i++) {
            data[i] = list.begin()[i];
        }
        for (; i < size(); i++) {
            data[i] = 0;
        }
    }

    Vec3f(Vec4f v);

    static constexpr usize size() { return 3; }

    f32 &operator[](usize i) { return data[i]; }
    f32 operator[](usize i) const { return data[i]; }

    Vec3f operator+(Vec3f v) { return {x + v.x, y + v.y, z + v.z}; }

    Vec3f operator-(Vec3f v) { return {x - v.x, y - v.y, z - v.z}; }

    Vec3f operator*(Vec3f v) { return {x * v.x, y * v.x, z * v.z}; }

    Vec3f operator*(f32 s) { return {x * s, y * s, z * s}; }

    Vec3f operator/(Vec3f v) { return {x / v.x, y / v.y, z / v.z}; }

    Vec3f operator/(f32 s) { return {x / s, y / s, z / s}; }

    friend f32 dot(Vec3f a, Vec3f b) {
        return a.x * b.x + a.y * b.y + a.z * b.z;
    }

    friend Vec3f cross(Vec3f a, Vec3f b) {
        return {a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z,
                a.x * b.y - a.y * b.x};
    }

    friend f32 len_squared(Vec3f v) {
        return v.x * v.x + v.y * v.y + v.z * v.z;
    }

    friend f32 len(Vec3f v) { return sqrt(len_squared(v)); }

    friend Vec3f norm(Vec3f v) { return v / len(v); }

    friend Vec3f rotate_x(Vec3f v, f32 angle) {
        return {
            v.x,
            v.y * cos(angle) - v.z * sin(angle),
            v.y * sin(angle) + v.z * cos(angle),
        };
    }

    friend Vec3f rotate_y(Vec3f v, f32 angle) {
        return {
            v.x * cos(angle) - v.z * sin(angle),
            v.y,
            v.x * sin(angle) + v.z * cos(angle),
        };
    }

    friend Vec3f rotate_z(Vec3f v, f32 angle) {
        return {
            v.x * cos(angle) - v.y * sin(angle),
            v.x * sin(angle) + v.y * cos(angle),
            v.z,
        };
    }
};

struct Vec4f {
    union {
        struct {
            f32 x, y, z, w;
        };
        struct {
            f32 r, g, b, a;
        };
        f32 data[4];
    };

    Vec4f() : w{1} {}

    Vec4f(std::initializer_list<f32> list) {
        usize i = 0;
        for (; i < list.size(); i++) {
            data[i] = list.begin()[i];
        }
        for (; i < size(); i++) {
            data[i] = 0;
        }
        w = 1;
    }

    Vec4f(Vec3f v) {
        x = v.x;
        y = v.y;
        z = v.z;
        w = 1;
    }

    static constexpr usize size() { return 4; }

    f32 &operator[](usize i) { return data[i]; }
    f32 operator[](usize i) const { return data[i]; }

    Vec4f operator+(Vec4f v) { return {x + v.x, y + v.y, z + v.z, w + v.w}; }

    Vec4f operator-(Vec4f v) { return {x - v.x, y - v.y, z - v.z, w - v.w}; }

    Vec4f operator*(Vec4f v) { return {x * v.x, y * v.x, z * v.z, w * v.w}; }

    Vec4f operator*(f32 s) { return {x * s, y * s, z * s, w * s}; }

    Vec4f operator/(Vec4f v) { return {x / v.x, y / v.y, z / v.z, w / v.w}; }

    Vec4f operator/(f32 s) { return {x / s, y / s, z / s, w / s}; }

    friend f32 dot(Vec4f a, Vec4f b) {
        return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
    }

    friend f32 len_squared(Vec4f v) {
        return v.x * v.x + v.y * v.y + v.z * v.z + v.w * v.w;
    }

    friend f32 len(Vec4f v) { return sqrt(len_squared(v)); }

    friend Vec4f norm(Vec4f v) { return v / len(v); }
};

using Vec2 = Vec2f;
using Vec3 = Vec3f;
using Vec4 = Vec4f;
