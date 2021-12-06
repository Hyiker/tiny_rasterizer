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
GEN_MAT4XMAT4(100000)
GEN_MAT4XMAT4(10000000)
GEN_MAT4XMAT4(100000000)
#endif /* MATHBENCH_H */
