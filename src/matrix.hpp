#include "core.hpp"
#include "vector.hpp"

struct Mat3x3f {
    f32 data[3][3];

    f32 *operator[](usize i) {
        assert(i < size());

        return data[i];
    }

    const f32 *operator[](const usize &i) const {
        assert(i < size());

        return data[i];
    }

    friend Mat3x3f operator*(const Mat3x3f &a, const Mat3x3f &b) {
        Mat3x3f c{};
        for (i32 m = 0; m < a.rows(); m++) {
            for (i32 k = 0; k < a.cols(); k++) {
                for (i32 n = 0; n < b.cols(); n++) {
                    c[n][m] += a[k][m] * b[n][k];
                }
            }
        }
        return c;
    }

    friend Vec3f operator*(const Mat3x3f &m, const Vec3f &v) {
        Vec3f result{};
        result.x = m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z;
        result.y = m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z;
        result.z = m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z;
        return result;
    }

    static constexpr usize rows() { return 3; }

    static constexpr usize cols() { return 3; }

    static constexpr usize size() { return 3 * 3; }

    template <typename... Args>
    static Mat3x3f scale(Args... args) {
        f32 scalars[] = {static_cast<f32>(args)...};
        static_assert(std::size(scalars) <= rows());

        Mat3x3f result{};
        for (usize i = 0; i < std::size(scalars); i++) {
            result[i][i] = scalars[i];
        }
        return result;
    }

    static constexpr Mat3x3f identity() {
        Mat3x3f result{};
        for (usize i = 0; i < rows(); i++) {
            result[i][i] = 1;
        }
        return result;
    }
};

struct Mat4x4f {
    f32 data[4][4];

    f32 *operator[](usize i) {
        assert(i < size());

        return data[i];
    }

    const f32 *operator[](const usize &i) const {
        assert(i < size());

        return data[i];
    }

    friend Mat4x4f operator*(const Mat4x4f &a, const Mat4x4f &b) {
        Mat4x4f c{};
        for (i32 m = 0; m < a.rows(); m++) {
            for (i32 k = 0; k < a.cols(); k++) {
                for (i32 n = 0; n < b.cols(); n++) {
                    c[n][m] += a[k][m] * b[n][k];
                }
            }
        }
        return c;
    }

    friend Vec4f operator*(const Mat4x4f &m, const Vec4f &v) {
        Vec4f result{};
        result.x =
            m[0][0] * v.x + m[0][1] * v.y + m[0][2] * v.z + m[0][3] * v.w;
        result.y =
            m[1][0] * v.x + m[1][1] * v.y + m[1][2] * v.z + m[1][3] * v.w;
        result.z =
            m[2][0] * v.x + m[2][1] * v.y + m[2][2] * v.z + m[2][3] * v.w;
        result.w =
            m[3][0] * v.x + m[3][1] * v.y + m[3][2] * v.z + m[3][3] * v.w;
        return result;
    }

    static constexpr usize rows() { return 4; }

    static constexpr usize cols() { return 4; }

    static constexpr usize size() { return 4 * 4; }

    static constexpr Mat4x4f rotation_x(f32 angle) {
        Mat4x4f result = Mat4x4f::identity();
        result[1][1] = cos(angle);
        result[1][2] = -sin(angle);
        result[2][1] = sin(angle);
        result[2][2] = cos(angle);
        return result;
    }

    static constexpr Mat4x4f rotation_y(f32 angle) {
        Mat4x4f result = Mat4x4f::identity();
        result[0][0] = cos(angle);
        result[0][2] = -sin(angle);
        result[2][0] = sin(angle);
        result[2][2] = cos(angle);
        return result;
    }

    static constexpr Mat4x4f rotation_z(f32 angle) {
        Mat4x4f result = Mat4x4f::identity();
        result[0][0] = cos(angle);
        result[0][1] = -sin(angle);
        result[1][0] = sin(angle);
        result[1][1] = cos(angle);
        return result;
    }

    static constexpr Mat4x4f perspective(f32 fov, f32 aspect, f32 near,
                                         f32 far) {
        Mat4x4f m{};
        m[0][0] = aspect * (1 / tan(fov / 2));
        m[1][1] = 1 / tan(fov / 2);
        m[2][2] = far / (far - near);
        m[2][3] = (-far * near) / (far - near);
        m[3][2] = 1.0;
        return m;
    }

    template <typename... Args>
    static constexpr Mat4x4f scale(Args... args) {
        f32 scalars[] = {static_cast<f32>(args)...};
        static_assert(std::size(scalars) <= rows());

        Mat4x4f result = Mat4x4f::identity();
        for (usize i = 0; i < std::size(scalars); i++) {
            result[i][i] *= scalars[i];
        }
        return result;
    }

    template <typename... Args>
    static constexpr Mat4x4f translate(Args... args) {
        f32 scalars[] = {static_cast<f32>(args)...};
        static_assert(std::size(scalars) <= rows());

        Mat4x4f result = Mat4x4f::identity();
        for (usize i = 0; i < std::size(scalars); i++) {
            result[i][cols() - 1] = scalars[i];
        }
        return result;
    }

    static constexpr Mat4x4f identity() {
        Mat4x4f result{};
        for (usize i = 0; i < rows(); i++) {
            result[i][i] = 1;
        }
        return result;
    }
};
