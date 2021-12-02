#ifndef MATH_HPP
#define MATH_HPP
#include <array>
#include <cmath>
#include <iostream>
#define DEG2RAD(d) ((d) / 180 * M_PI)
namespace Rasterizer {

// each color portion regularized to [0, 1]
class Vec3;
using RGBColor = Vec3;
class Vec4 {
   public:
    float x, y, z, w;
    Vec4() : x{0.0f}, y{0.0f}, z{0.0f}, w{0.0f} {};
    Vec4(float x, float y, float z, float w) : x{x}, y{y}, z{z}, w{w} {}
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
    std::array<std::array<float, 4>, 4> m;
    Mat4() {
        for (int i = 0; i < 4; i++) {
            m[i].fill(0);
        }
    };

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
    Mat4 operator*(const Mat4& mat) const {
        Mat4 result;
        for (int i = 0; i < 4; i++) {
            for (int j = 0; j < 4; j++) {
                for (int k = 0; k < 4; k++) {
                    result.m[i][j] += m[i][k] * mat.m[k][j];
                }
            }
        }
        return std::move(result);
    }
    static Mat4 identity() {
        Mat4 mat;
        mat << 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1, 0, 0, 0, 0, 1;
        return std::move(mat);
    }
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
    Vec3 operator*(const Vec3& v) const {
        return Vec3(x * v.x, y * v.y, z * v.z);
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
    Vec3 sin() const { return Vec3(std::sin(x), std::sin(y), std::sin(z)); }
    Vec3 cos() const { return Vec3(std::cos(x), std::cos(y), std::cos(z)); }
    Vec4 toVec4() const { return Vec4(x, y, z, 1); }
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
Vec3 operator*(float coe, const Vec3& v) {
    return Vec3(v.x * coe, v.y * coe, v.z * coe);
}
Vec3 operator*(const Vec3& v, float coe) {
    return Vec3(v.x * coe, v.y * coe, v.z * coe);
}
Vec3 operator/(const Vec3& v, float coe) {
    coe = 1 / coe;
    return coe * Vec3(v.x, v.y, v.z);
}
std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
std::ostream& operator<<(std::ostream& os, const Vec4& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}
std::ostream& operator<<(std::ostream& os, const Mat4& mat) {
    for (int i = 0; i < 4; i++) {
        for (int j = 0; j < 4; j++) {
            os << mat.m[i][j];
            if (j != 3) {
                os << ", ";
            }
        }
        os << '\n';
    }
    return os;
}
}  // namespace Rasterizer

namespace math {
template <typename T>
T clamp(T val, T f, T c) {
    return std::max(f, std::min(val, c));
}
}  // namespace math
#endif /* MATH_HPP */