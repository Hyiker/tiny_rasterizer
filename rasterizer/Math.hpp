#ifndef MATH_HPP
#define MATH_HPP
#include <array>
#include <cmath>
#include <iostream>
#define DEG2RAD(d) ((float(d)) / 180.0f * M_PI)
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

Rasterizer::Vec3 operator*(float coe, const Rasterizer::Vec3& v);
Rasterizer::Vec3 operator*(const Rasterizer::Vec3& v, float coe);
Rasterizer::Vec3 operator/(const Rasterizer::Vec3& v, float coe);
Rasterizer::Vec4 operator/(const Rasterizer::Vec4& v, float coe);
std::ostream& operator<<(std::ostream& os, const Rasterizer::Vec3& v);
std::ostream& operator<<(std::ostream& os, const Rasterizer::Vec4& v);
std::ostream& operator<<(std::ostream& os, const Rasterizer::Mat4& mat);
}  // namespace Rasterizer

namespace math {
template <typename T>
T clamp(T val, T f, T c) {
    return std::max(f, std::min(val, c));
}
template <typename T>
T lerp(float r, T v0, T v1) {
    return r * v0 + (1.0f - r) * v1;
}
}  // namespace math
#endif /* MATH_HPP */
