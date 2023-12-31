/*
 *
 * Copyright (c) 2018, Alliance for Open Media. All rights reserved
 *
 * This source code is subject to the terms of the BSD 2 Clause License and
 * the Alliance for Open Media Patent License 1.0. If the BSD 2 Clause License
 * was not distributed with this source code in the LICENSE file, you can
 * obtain it at www.aomedia.org/license/software. If the Alliance for Open
 * Media Patent License 1.0 was not distributed with this source code in the
 * PATENTS file, you can obtain it at www.aomedia.org/license/patent.
 */

#include <arm_neon.h>
#include <assert.h>

#include "aom/aom_integer.h"
#include "aom_dsp/aom_dsp_common.h"
#include "aom_dsp/blend.h"
#include "aom_dsp/arm/mem_neon.h"
#include "aom_ports/mem.h"
#include "config/aom_dsp_rtcd.h"

void aom_blend_a64_vmask_neon(uint8_t *dst, uint32_t dst_stride,
                              const uint8_t *src0, uint32_t src0_stride,
                              const uint8_t *src1, uint32_t src1_stride,
                              const uint8_t *mask, int w, int h) {
  uint8x8_t tmp0, tmp1;
  uint8x16_t tmp0_q, tmp1_q, res_q;
  uint16x8_t res, res_low, res_high;
  assert(IMPLIES(src0 == dst, src0_stride == dst_stride));
  assert(IMPLIES(src1 == dst, src1_stride == dst_stride));

  assert(h >= 2);
  assert(w >= 2);
  assert(IS_POWER_OF_TWO(h));
  assert(IS_POWER_OF_TWO(w));

  if (w >= 16) {
    for (int i = 0; i < h; ++i) {
      const uint8x8_t m = vdup_n_u8((uint8_t)mask[i]);
      const uint8x8_t max_minus_m = vdup_n_u8(64 - (uint8_t)mask[i]);
      for (int j = 0; j < w; j += 16) {
        __builtin_prefetch(src0);
        __builtin_prefetch(src1);
        tmp0_q = vld1q_u8(src0);
        tmp1_q = vld1q_u8(src1);
        res_low = vmull_u8(m, vget_low_u8(tmp0_q));
        res_low = vmlal_u8(res_low, max_minus_m, vget_low_u8(tmp1_q));
        res_high = vmull_u8(m, vget_high_u8(tmp0_q));
        res_high = vmlal_u8(res_high, max_minus_m, vget_high_u8(tmp1_q));
        res_q = vcombine_u8(vrshrn_n_u16(res_low, AOM_BLEND_A64_ROUND_BITS),
                            vrshrn_n_u16(res_high, AOM_BLEND_A64_ROUND_BITS));
        vst1q_u8(dst, res_q);
        src0 += 16;
        src1 += 16;
        dst += 16;
      }
      src0 += src0_stride - w;
      src1 += src1_stride - w;
      dst += dst_stride - w;
    }
  } else if (w == 8) {
    for (int i = 0; i < h; ++i) {
      __builtin_prefetch(src0);
      __builtin_prefetch(src1);
      const uint8x8_t m = vdup_n_u8((uint8_t)mask[i]);
      const uint8x8_t max_minus_m = vdup_n_u8(64 - (uint8_t)mask[i]);
      tmp0 = vld1_u8(src0);
      tmp1 = vld1_u8(src1);
      res = vmull_u8(m, tmp0);
      res = vmlal_u8(res, max_minus_m, tmp1);
      vst1_u8(dst, vrshrn_n_u16(res, AOM_BLEND_A64_ROUND_BITS));
      src0 += src0_stride;
      src1 += src1_stride;
      dst += dst_stride;
    }
  } else if (w == 4) {
    for (int i = 0; i < h; i += 2) {
      __builtin_prefetch(src0 + 0 * src0_stride);
      __builtin_prefetch(src0 + 1 * src0_stride);
      __builtin_prefetch(src1 + 0 * src1_stride);
      __builtin_prefetch(src1 + 1 * src1_stride);
      const uint16x4_t m1 = vdup_n_u16((uint16_t)mask[i]);
      const uint16x4_t m2 = vdup_n_u16((uint16_t)mask[i + 1]);
      const uint8x8_t m = vmovn_u16(vcombine_u16(m1, m2));
      const uint16x4_t max_minus_m1 = vdup_n_u16(64 - (uint16_t)mask[i]);
      const uint16x4_t max_minus_m2 = vdup_n_u16(64 - (uint16_t)mask[i + 1]);
      const uint8x8_t max_minus_m =
          vmovn_u16(vcombine_u16(max_minus_m1, max_minus_m2));
      tmp0 = load_unaligned_u8_4x2(src0, src0_stride);
      tmp1 = load_unaligned_u8_4x2(src1, src1_stride);
      res = vmull_u8(m, tmp0);
      res = vmlal_u8(res, max_minus_m, tmp1);
      const uint8x8_t result = vrshrn_n_u16(res, AOM_BLEND_A64_ROUND_BITS);
      store_unaligned_u8_4x1(dst + 0 * dst_stride, result, 0);
      store_unaligned_u8_4x1(dst + 1 * dst_stride, result, 1);
      src0 += (2 * src0_stride);
      src1 += (2 * src1_stride);
      dst += (2 * dst_stride);
    }
  } else if (w == 2) {
    for (int i = 0; i < h; i += 2) {
      __builtin_prefetch(src0 + 0 * src0_stride);
      __builtin_prefetch(src0 + 1 * src0_stride);
      __builtin_prefetch(src1 + 0 * src1_stride);
      __builtin_prefetch(src1 + 1 * src1_stride);
      const uint8x8_t m1 = vdup_n_u8(mask[i]);
      const uint8x8_t m2 = vdup_n_u8(mask[i + 1]);
      const uint16x4x2_t m_trn =
          vtrn_u16(vreinterpret_u16_u8(m1), vreinterpret_u16_u8(m2));
      const uint8x8_t m = vreinterpret_u8_u16(m_trn.val[0]);
      const uint8x8_t max_minus_m1 = vdup_n_u8(64 - mask[i]);
      const uint8x8_t max_minus_m2 = vdup_n_u8(64 - mask[i + 1]);
      const uint16x4x2_t max_minus_m_trn = vtrn_u16(
          vreinterpret_u16_u8(max_minus_m1), vreinterpret_u16_u8(max_minus_m2));
      const uint8x8_t max_minus_m = vreinterpret_u8_u16(max_minus_m_trn.val[0]);
      tmp0 = load_unaligned_u8_2x2(src0, src0_stride);
      tmp1 = load_unaligned_u8_2x2(src1, src1_stride);
      res = vmull_u8(m, tmp0);
      res = vmlal_u8(res, max_minus_m, tmp1);
      const uint8x8_t result = vrshrn_n_u16(res, AOM_BLEND_A64_ROUND_BITS);
      store_unaligned_u8_2x1(dst + 0 * dst_stride, result, 0);
      store_unaligned_u8_2x1(dst + 1 * dst_stride, result, 1);
      src0 += (2 * src0_stride);
      src1 += (2 * src1_stride);
      dst += (2 * dst_stride);
    }
  }
}
