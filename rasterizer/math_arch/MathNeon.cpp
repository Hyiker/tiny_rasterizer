#include "rasterizer/Math.hpp"
#ifdef OPT_NEON
#include <arm_neon.h>

#include <cmath>
#include <cstdint>
using namespace Rasterizer;
using namespace std;
static inline void matrix_multiply_4x4_neon(const float32_t *A,
                                            const float32_t *B, float32_t *C) {
    // these are the rows A
    float32x4_t A0;
    float32x4_t A1;
    float32x4_t A2;
    float32x4_t A3;

    // these are the rows B
    float32x4_t B0;
    float32x4_t B1;
    float32x4_t B2;
    float32x4_t B3;

    // these are the rows C
    float32x4_t C0;
    float32x4_t C1;
    float32x4_t C2;
    float32x4_t C3;

    A0 = vld1q_f32(A);
    A1 = vld1q_f32(A + 4);
    A2 = vld1q_f32(A + 8);
    A3 = vld1q_f32(A + 12);

    B0 = vld1q_f32(B);
    B1 = vld1q_f32(B + 4);
    B2 = vld1q_f32(B + 8);
    B3 = vld1q_f32(B + 12);

    // Zero accumulators for C values
    C0 = vmovq_n_f32(0);
    C1 = vmovq_n_f32(0);
    C2 = vmovq_n_f32(0);
    C3 = vmovq_n_f32(0);

    // Multiply accumulate in 4x1 blocks, i.e. each column in C
    C0 = vfmaq_laneq_f32(C0, B0, A0, 0);
    C0 = vfmaq_laneq_f32(C0, B1, A0, 1);
    C0 = vfmaq_laneq_f32(C0, B2, A0, 2);
    C0 = vfmaq_laneq_f32(C0, B3, A0, 3);
    vst1q_f32(C, C0);

    C1 = vfmaq_laneq_f32(C1, B0, A1, 0);
    C1 = vfmaq_laneq_f32(C1, B1, A1, 1);
    C1 = vfmaq_laneq_f32(C1, B2, A1, 2);
    C1 = vfmaq_laneq_f32(C1, B3, A1, 3);
    vst1q_f32(C + 4, C1);

    C2 = vfmaq_laneq_f32(C2, B0, A2, 0);
    C2 = vfmaq_laneq_f32(C2, B1, A2, 1);
    C2 = vfmaq_laneq_f32(C2, B2, A2, 2);
    C2 = vfmaq_laneq_f32(C2, B3, A2, 3);
    vst1q_f32(C + 8, C2);

    C3 = vfmaq_laneq_f32(C3, B0, A3, 0);
    C3 = vfmaq_laneq_f32(C3, B1, A3, 1);
    C3 = vfmaq_laneq_f32(C3, B2, A3, 2);
    C3 = vfmaq_laneq_f32(C3, B3, A3, 3);
    vst1q_f32(C + 12, C3);
}

Mat4 Rasterizer::Mat4::operator*(const Mat4 &mat) const {
    Mat4 result;
    matrix_multiply_4x4_neon(reinterpret_cast<const float32_t *>(m),
                             reinterpret_cast<const float32_t *>(mat.m),
                             reinterpret_cast<float32_t *>(result.m));
    return std::move(result);
}
#endif