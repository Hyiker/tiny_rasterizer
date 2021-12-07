#ifndef MATHBENCH_H
#define MATHBENCH_H

#include "rasterizer/Math.hpp"
#define ll long long
#define GEN_MAT4XMAT4(x)                                    \
    void Mat4xMat4_##x() {                                  \
        Rasterizer::Mat4 mat(Rasterizer::Mat4::identity()); \
        mat = mat / 3.0f;                                   \
        for (ll i = 0; i < x; i++) {                        \
            mat = mat * mat;                                \
        }                                                   \
    }

#define GEN_MAT4TRANSPOSE(x)                                                  \
    void Mat4Transpose_##x() {                                                \
        Rasterizer::Mat4 mat;                                                 \
        mat << 1.23, 123.2, 45, 3, 99, 122, 33, 21, 1223, 442, 2316, 213, 11, \
            4, 5, 14;                                                         \
        for (ll i = 0; i < x; i++) {                                          \
            mat = mat.transpose();                                            \
        }                                                                     \
    }
#define GEN_MAT4INV(x)                                                        \
    void Mat4Inv_##x() {                                                      \
        Rasterizer::Mat4 mat;                                                 \
        mat << 1.23, 123.2, 45, 3, 99, 122, 33, 21, 1223, 442, 2316, 213, 11, \
            4, 5, 14;                                                         \
        for (ll i = 0; i < x; i++) mat = mat.inverse();                       \
    }
GEN_MAT4XMAT4(100000)
GEN_MAT4XMAT4(10000000)
GEN_MAT4XMAT4(100000000)

GEN_MAT4TRANSPOSE(100000)
GEN_MAT4TRANSPOSE(10000000)
GEN_MAT4TRANSPOSE(100000000)

GEN_MAT4INV(10000)
GEN_MAT4INV(1000000)

#endif /* MATHBENCH_H */
