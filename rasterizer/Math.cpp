#include "rasterizer/Math.hpp"

using namespace Rasterizer;
Vec3 Rasterizer::operator*(float coe, const Vec3& v) {
    return Vec3(v.x * coe, v.y * coe, v.z * coe);
}
Vec3 Rasterizer::operator*(const Vec3& v, float coe) {
    return Vec3(v.x * coe, v.y * coe, v.z * coe);
}
Vec3 Rasterizer::operator/(const Vec3& v, float coe) {
    coe = 1 / coe;
    return coe * Vec3(v.x, v.y, v.z);
}
Vec4 Rasterizer::operator/(const Vec4& v, float coe) {
    return Vec4(v.x / coe, v.y / coe, v.z / coe, v.w / coe);
}

std::ostream& Rasterizer::operator<<(std::ostream& os, const Vec3& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
    return os;
}
std::ostream& Rasterizer::operator<<(std::ostream& os, const Vec4& v) {
    os << "(" << v.x << ", " << v.y << ", " << v.z << ", " << v.w << ")";
    return os;
}
std::ostream& Rasterizer::operator<<(std::ostream& os, const Mat4& mat) {
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