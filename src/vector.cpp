#include "vector.hpp"

Vec2f::Vec2f(Vec4f v) {
    for (usize i = 0; i < size(); i++) {
        data[i] = v[i];
    }
}

Vec3f::Vec3f(Vec4f v) {
    for (usize i = 0; i < size(); i++) {
        data[i] = v[i];
    }
}
