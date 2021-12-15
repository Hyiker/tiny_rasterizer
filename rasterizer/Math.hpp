#ifndef MATH_H
#define MATH_H
#include <array>
#include <cmath>
#include <cstdlib>
#include <iostream>
#define OPT_EN
#ifdef OPT_EN
#if __has_include(<arm_neon.h>)
#define OPT_NEON
#endif
#endif
#define DEG2RAD(d) ((float(d)) / 180.0f * M_PI)
#define EPSILON 0.001f
namespace Rasterizer {

// each color portion regularized to [0, 1]
class Vec3;
class Vec4;
using RGBColor = Vec3;
using RGBAColor = Vec4;
class Vec4 {
   public:
    float x, y, z, w;
    Vec4() : x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f} {};
    Vec4(float v) : x{v}, y{v}, z{v}, w{v} {};
    Vec4(float x, float y, float z, float w) : x{x}, y{y}, z{z}, w{w} {}
};

class Vec3 {
   public:
    float x, y, z;
    Vec3() : x{0.0f}, y{0.0f}, z{0.0f} {}
    Vec3(float v) : Vec3(v, v, v) {}
    Vec3(float x, float y, float z) : x{x}, y{y}, z{z} {}
    Vec3(const Vec4& v4) : x{v4.x}, y{v4.y}, z{v4.z} {}
    float dot(const Vec3& v) const { return x * v.x + y * v.y + z * v.z; }
    Vec3 cross(const Vec3 v) const {
        return Vec3(y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x);
    }
    inline float norm2() const { return x * x + y * y + z * z; }
    inline float norm() const { return std::sqrt(x * x + y * y + z * z); }
    Vec3 normalized() const { return (*this) / this->norm(); }
    Vec3& normalize() {
        float n = this->norm();
        x /= n;
        y /= n;
        z /= n;
        return (*this);
    }
    Vec3 operator+(const Vec3& v) const {
        return Vec3(x + v.x, y + v.y, z + v.z);
    }
    Vec3 operator-(const Vec3& v) const {
        return Vec3(x - v.x, y - v.y, z - v.z);
    }
    Vec3 operator-() const { return Vec3(-x, -y, -z); }
    Vec3& operator=(const Vec4& v4) {
        x = v4.x;
        y = v4.y;
        z = v4.z;
        return *this;
    }
    Vec3 sin() const { return Vec3(std::sin(x), std::sin(y), std::sin(z)); }
    Vec3 cos() const { return Vec3(std::cos(x), std::cos(y), std::cos(z)); }
    Vec3 cwiseProduct(const Vec3& v2) const {
        return Vec3(x * v2.x, y * v2.y, z * v2.z);
    }
    Vec3 max(const Vec3 v) const {
        return Vec3(std::max(v.x, x), std::max(v.y, y), std::max(v.z, z));
    }

    Vec3 min(const Vec3 v) const {
        return Vec3(std::min(v.x, x), std::min(v.y, y), std::min(v.z, z));
    }
    Vec4 toVec4(float w = 1.0f) const { return Vec4(x, y, z, w); }
    RGBColor rgbNormalized() const { return (*this) / 255.0f; }
    RGBColor& rgbNormalize() {
        x /= 255.0f;
        y /= 255.0f;
        z /= 255.0f;
        return *this;
    }

    friend Vec3 operator*(float, const Vec3&);
    friend Vec3 operator*(const Vec3&, float);
    friend Vec3 operator/(const Vec3&, float);
};
class Mat4 {
   private:
    class MatProxy {
       public:
        Mat4& ref;
        int index;
        MatProxy(Mat4& mat, int index = 1) : ref{mat}, index{index} {}
        template <typename N>
        MatProxy& operator,(N number) {
            ref.m[index / 4][index % 4] = number;
            index++;
            return *this;
        }
    };

   public:
    float m[4][4];
    Mat4() { std::memset(m, 0, sizeof(m)); };

    template <typename N>
    MatProxy operator<<(N number) {
        m[0][0] = number;
        return MatProxy(*this);
    }
    Vec4 operator*(const Vec4& vec) const {
        Vec4 result;
        result.x = m[0][0] * vec.x + m[0][1] * vec.y + m[0][2] * vec.z +
                   m[0][3] * vec.w;

        result.y = m[1][0] * vec.x + m[1][1] * vec.y + m[1][2] * vec.z +
                   m[1][3] * vec.w;

        result.z = m[2][0] * vec.x + m[2][1] * vec.y + m[2][2] * vec.z +
                   m[2][3] * vec.w;

        result.w = m[3][0] * vec.x + m[3][1] * vec.y + m[3][2] * vec.z +
                   m[3][3] * vec.w;
        return std::move(result);
    }
    Mat4 operator*(const Mat4& mat) const;
    Mat4 operator/(float div) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                result.m[i][j] = this->m[i][j] / div;
            }
        }
        return result;
    }
    Mat4 cfactor(int p, int q, int n = 4) const {
        Mat4 mat;
        int i = 0, j = 0;
        for (int r = 0; r < n; r++) {
            for (int c = 0; c < n; c++) {
                if (r != p && c != q) {
                    mat.m[i][j++] = this->m[r][c];
                    if (j == n - 1) {
                        j = 0;
                        i++;
                    }
                }
            }
        }
        return std::move(mat);
    }
    float determinant(int n = 4) const {
        float d = 0.0;
        if (n == 1) {
            return this->m[0][0];
        }
        float s = 1;
        for (int f = 0; f < n; f++) {
            Mat4 t = this->cfactor(0, f);
            d += s * this->m[0][f] * t.determinant(n - 1);
            s = -s;
        }
        return d;
    }
    Mat4 adjoint() const {
        Mat4 mat;
        float s = 1;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                Mat4 t = this->cfactor(i, j);
                s = ((i + j) & 1) == 0 ? 1.0 : -1.0;
                mat.m[j][i] = s * t.determinant(3);
            }
        }
        return std::move(mat);
    }
    Mat4 inverse() const {
        float det = this->determinant();
        if (det == 0.0) {
            // bad inverse
            return Mat4::identity();
        }
        Mat4 adj = this->adjoint();
        return adj / det;
    }

    Mat4 transpose() const {
        Mat4 mat;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                mat.m[i][j] = this->m[j][i];
            }
        }
        return std::move(mat);
    }
    static Mat4 identity() {
        Mat4 mat;
        mat << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
        return std::move(mat);
    }
    static Mat4 ortho(float near, float far, float lrtb = 1.0f) {
        Mat4 mat(Mat4::identity());
        near = -near;
        far = -far;
        mat.m[0][0] = 1.0f / lrtb;
        mat.m[1][1] = 1.0f / lrtb;
        mat.m[2][2] = 2 / (near - far);
        mat.m[2][3] = -(near + far) / (near - far);
        return mat;
    }
    static Mat4 persp(float near, float far, float fov, float aspect_ratio) {
        Mat4 projection;
        float z_rng = far - near;
        float tan_fov_2 = std::tan(fov / 2.f);
        projection << 1.0f / (aspect_ratio * tan_fov_2), 0, 0, 0, 0,
            1 / tan_fov_2, 0, 0, 0, 0, -(far + near) / (far - near),
            -2.f * far * near / (far - near), 0, 0, -1, 0;
        return projection;
    }

    static Mat4 lookAt(const Vec3& eye_pos, const Vec3& target,
                       const Vec3& up) {
        Mat4 mat(Mat4::identity());
        Vec3 z_axis = (eye_pos - target).normalized();
        Vec3 x_axis = up.cross(z_axis).normalized();
        Vec3 y_axis = z_axis.cross(x_axis);

        mat.m[0][0] = x_axis.x;
        mat.m[0][1] = x_axis.y;
        mat.m[0][2] = x_axis.z;

        mat.m[1][0] = y_axis.x;
        mat.m[1][1] = y_axis.y;
        mat.m[1][2] = y_axis.z;

        mat.m[2][0] = z_axis.x;
        mat.m[2][1] = z_axis.y;
        mat.m[2][2] = z_axis.z;

        mat.m[0][3] = -x_axis.dot(eye_pos);
        mat.m[1][3] = -y_axis.dot(eye_pos);
        mat.m[2][3] = -z_axis.dot(eye_pos);
        return mat;
    }
};

Rasterizer::Vec3 operator*(float coe, const Rasterizer::Vec3& v);
Rasterizer::Vec3 operator*(const Rasterizer::Vec3& v, float coe);
Rasterizer::Vec3 operator/(const Rasterizer::Vec3& v, float coe);
Rasterizer::Vec4 operator/(const Rasterizer::Vec4& v, float coe);
std::ostream& operator<<(std::ostream& os, const Rasterizer::Vec3& v);
std::ostream& operator<<(std::ostream& os, const Rasterizer::Vec4& v);
std::ostream& operator<<(std::ostream& os, const Rasterizer::Mat4& mat);
}  // namespace Rasterizer

namespace math {
static constexpr float M_PI2 = M_PI * 2.f;

template <typename T>
inline T clamp(T val, T f, T c) {
    return std::max(f, std::min(val, c));
}
template <typename T>
inline T lerp(float r, T v0, T v1) {
    return r * v0 + (1.0f - r) * v1;
}
template <typename T>
inline T interpolate(const std::array<T, 3>& arr, float alpha, float beta,
                     float gamma) {
    return alpha * arr[0] + beta * arr[1] + gamma * arr[2];
}
static float rand_2to1(const Rasterizer::Vec3& uv) {
    // 0 - 1
    constexpr float a = 12.9898, b = 78.233, c = 43758.5453;
    float dt = uv.x * a + uv.y * b, sn = std::fmod(dt, M_PI);
    float _;
    return std::modf(std::sin(sn) * c, &_);
}
template <int num_samples = 16, int num_rings = 10>
inline void poissonDiskSamples(
    std::array<Rasterizer::Vec3, num_samples>& poisson_disk,
    const Rasterizer::Vec3& seed) {
    constexpr float ANGLE_STEP = M_PI2 * float(num_rings) / float(num_samples);
    constexpr float inv_num_samples = 1.0 / float(num_samples);

    float angle = rand_2to1(seed) * M_PI2;
    float radius = inv_num_samples;
    float radiusStep = radius;

    for (int i = 0; i < num_samples; i++) {
        poisson_disk[i] =
            Rasterizer::Vec3(std::cos(angle), std::sin(angle), 0.f) *
            std::pow(radius, 0.75);
        radius += radiusStep;
        angle += ANGLE_STEP;
    }
}
}  // namespace math
#endif /* MATH_H */
