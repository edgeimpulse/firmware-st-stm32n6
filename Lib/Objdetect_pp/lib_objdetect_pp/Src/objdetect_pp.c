/*---------------------------------------------------------------------------------------------
 * Copyright (c) 2023 STMicroelectronics.
 * All rights reserved.
 *
 * This software is licensed under terms that can be found in the LICENSE file in
 * the root directory of this software component.
 * If no LICENSE file comes with this software, it is provided AS-IS.
 *--------------------------------------------------------------------------------------------*/

#include "objdetect_pp_loc.h"


/* return max value and it's index from an array */
#ifdef AI_YOLOV5_PP_MVEF_OPTIM
void objdetect_maxi(float32_t *arr, int32_t len_arr, float32_t *maxim, int32_t *index)
{

    float32x4_t    f32x4_max_val = vdupq_n_f32(F32_MIN);
    uint32x4_t     u32x4_max_idx = vdupq_n_u32(0);


    uint32x4_t u32x4_idx = vidupq_n_u32(0,1);
    float32_t *pSrc = arr;
    int32_t iter = len_arr;
    while(iter > 0)
    {
	mve_pred16_t p = vctp32q(iter);
        // load up to 4 float32_t
        float32x4_t f32x4_val = vldrwq_z_f32(pSrc, p);
	pSrc+=4;
        // Compare according to p to create p0
        mve_pred16_t p0 = vcmpgtq_m_f32(f32x4_val, f32x4_max_val, p);

        // according to p0: update with s8x16_val or keep s8x16_blk_minmax_val
        f32x4_max_val = vpselq_f32(f32x4_val, f32x4_max_val, p0);
        /* according to p0: store per-lane extrema indexes*/
        u32x4_max_idx = vpselq_u32(u32x4_idx, u32x4_max_idx, p0);
        u32x4_idx+=4;
	iter-=4;

    }
	/*
     * Get max value across the vector
     */
    *maxim = vmaxnmvq_f32(F32_MIN, f32x4_max_val);
    /*
     * set index for lower values to max possible index
     */
    mve_pred16_t p0 = vcmpgeq_n_f32(f32x4_max_val, F32_MIN);
    uint32x4_t indexVec = vpselq_u32(u32x4_max_idx, vdupq_n_u32(len_arr), p0);
    /*
     * Get min index which is thus for a max value
     */
    *index = (int32_t)vminvq_u32(len_arr, indexVec);

}
#else
void objdetect_maxi(float32_t *arr, int32_t len_arr, float32_t *maxim, int32_t *index)
{
    *index = 0;
    *maxim = arr[0];

    for (int i = 1; i < len_arr; i++)
    {
        if (arr[i] > *maxim)
        {
            *maxim = arr[i];
            *index = i;
        }
    }
}
#endif

#ifdef AI_YOLOV5_PP_MVEI_OPTIM
void objdetect_maxi_uint8(uint8_t *arr, int32_t len_arr, uint8_t *pMaxim, int32_t *pIndex)
{
    uint8_t maxValue = 0;

    uint8_t *pSrc = arr;
    uint8x16_t u8x16_max_val = vdupq_n_u8(0);
    uint8x16_t u8x16_max_idx;
    int32_t iter = len_arr;
    uint8x16_t u8x16_idx = vidupq_n_u8(0,1);
    uint8_t index =  0xFF;
    while(iter > 0)
    {
        mve_pred16_t p = vctp8q(iter);
        // load up to 16 int8
        uint8x16_t u8x16_val = vld1q_z_u8(pSrc, p);
        pSrc+=16;
        // Compare according to p to create p0
        mve_pred16_t p0 = vcmpcsq_m_u8(u8x16_val, u8x16_max_val, p);

        u8x16_max_val = vpselq_u8(u8x16_val, u8x16_max_val, p0);
        u8x16_max_idx = vpselq_u8(u8x16_idx, u8x16_max_idx, p0);

        u8x16_idx+=16;
        iter-=16;
    }

    maxValue = vmaxvq_u8(maxValue, u8x16_max_val);
    // get index for which value is equal to maxvalue and then get lower index
    mve_pred16_t p0 = vcmpeqq_n_u8(u8x16_max_val, maxValue);

    u8x16_max_idx = vpselq_u8( u8x16_max_idx, vdupq_n_u8(0xFF), p0);
    /*
     * Get min index which is thus for a max value
    */
    index = vminvq_p_u8(0xFF, u8x16_max_idx,p0);

    *pMaxim = maxValue;
    *pIndex = index;

}
void objdetect_maxi_uint8_large(uint8_t *arr, int32_t len_arr, uint8_t *pMaxim, int32_t *pIndex)
{
    uint8_t maxValue = 0;
    uint8_t maxValueLoc = 0;
    int32_t index = 0;

    int32_t iterLoop = len_arr;
    uint8_t *pSrc = arr;
    int32_t indexBlk = 0;
    while (iterLoop > 0)
    {
        uint8x16_t u8x16_max_val = vdupq_n_u8(0);
        uint8x16_t u8x16_max_idx = vdupq_n_u8(len_arr);
        int32_t iter = MIN(len_arr,0x100);
        iterLoop -= iter;
        uint8x16_t u8x16_idx = vidupq_n_u8(0,1);
        uint8_t indexLoc =  0xFF;
        while(iter > 0)
        {
            mve_pred16_t p = vctp8q(iter);
            // load up to 16 int8
            uint8x16_t u8x16_val = vld1q_z_u8(pSrc, p);
            pSrc+=16;
            // Compare according to p to create p0
            mve_pred16_t p0 = vcmpcsq_m_u8(u8x16_val, u8x16_max_val, p);

            u8x16_max_val = vpselq_u8(u8x16_val, u8x16_max_val, p0);
            u8x16_max_idx = vpselq_u8(u8x16_idx, u8x16_max_idx, p0);

            u8x16_idx+=16;
            iter-=16;

       }

        maxValueLoc = vmaxvq_u8(0, u8x16_max_val);
        mve_pred16_t p0 = vcmpeqq_n_u8(u8x16_max_val, maxValueLoc);

        u8x16_max_idx = vpselq_u8( u8x16_max_idx, vdupq_n_u8(0xFF), p0);
        /*
         * Get min index which is thus for a max value
        */
        indexLoc = vminvq_p_u8(0xFF, u8x16_max_idx,p0);

        if (maxValueLoc > maxValue) {
            maxValue = maxValueLoc;
            index = indexLoc + indexBlk;
        }
        indexBlk += 256;
    }
    *pMaxim = maxValue;
    *pIndex = index;

}

#else
void objdetect_maxi_uint8(uint8_t *arr, int32_t len_arr, uint8_t *maxim, int32_t *index)
{
    *index = 0;
    *maxim = arr[0];

    for (int i = 1; i < len_arr; i++)
    {
        if (arr[i] > *maxim)
        {
            *maxim = arr[i];
            *index = i;
        }
    }
}
#endif

/* return max value and it's index from a transposed array */
#ifdef AI_YOLOV8_PP_MVEF_OPTIM
void objdetect_maxi_transpose(float32_t *arr, int32_t len_arr, int32_t nb_total_boxes, float32_t *maxim, uint32_t *index, int32_t parallelize)
{

    float32x4_t    f32x4_max_val = vdupq_n_f32(F32_MIN);
    uint32x4_t     u32x4_max_idx = vdupq_n_u32(0);

    parallelize = MIN(parallelize, 4);
    mve_pred16_t p = vctp32q(parallelize);

    uint32x4_t u32x4_idx = vdupq_n_u32(0);
    for (int i = 0; i < len_arr; i++)
    {
        // load up to 16 int8
        float32x4_t f32x4_val = vld1q_z_f32(&arr[i*nb_total_boxes], p);
        // Compare according to p to create p0
        mve_pred16_t p0 = vcmpgtq_m_f32(f32x4_val, f32x4_max_val, p);

        // according to p0: update with s8x16_val or keep s8x16_blk_minmax_val
        f32x4_max_val = vpselq_f32(f32x4_val, f32x4_max_val, p0);
        /* according to p0: store per-lane extrema indexes*/
        u32x4_max_idx = vpselq_u32(u32x4_idx, u32x4_max_idx, p0);
        u32x4_idx+=1;

    }
    vstrwq_p_u32((uint32_t *)index,u32x4_max_idx,p);
    vstrwq_p_f32(maxim,f32x4_max_val,p);
}
#else
void objdetect_maxi_transpose(float32_t *arr, int32_t len_arr, int32_t nb_total_boxes, float32_t *maxim, int32_t *index)
{
    *index = 0;
    *maxim = arr[0];

    for (int i = 1; i < len_arr; i++)
    {
        if (arr[i*nb_total_boxes] > *maxim)
        {
            *maxim = arr[i*nb_total_boxes];
            *index = i;
        }
    }
}
#endif
/* return max value and it's index from a transposed array */
#ifdef AI_YOLOV8_PP_MVEI_OPTIM
void objdetect_maxi_transpose_int8_large(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, uint16_t *index, int32_t parallelize)
{
    int8x16_t  s8x16_max_val = vdupq_n_s8(Q7_MIN);
    uint8x16_t u8x16_max_idx = vdupq_n_u8(0);
    uint8x16_t u8x16_max_idx_blk = vdupq_n_u8(0);
    parallelize = MIN(parallelize, 16);
    mve_pred16_t    p = vctp8q(parallelize);
    if (len_arr > 0x10000) {
      return;
    }

    int8_t idx_blk = 0;
    while (len_arr > 0) {
        int maxIter = MIN(len_arr, 0x100);
        uint8x16_t u8x16_blk_idx = vdupq_n_u8(0);
        uint8x16_t u8x16_blk_max_idx = vdupq_n_u8(0);
        int8x16_t  s8x16_blk_max_val = vdupq_n_s8(Q7_MIN);
        mve_pred16_t p0;
        // Process each block up to 256 i.e. 0x100
        for (int i = 0; i < maxIter; i++)
        {
            // load up to 16 int8
            int8x16_t s8x16_val = vld1q_z_s8(&arr[i*nb_total_boxes], p);
            // Compare according to p to create p0
            p0 = vcmpgtq_m_s8(s8x16_val, s8x16_blk_max_val, p);

            // according to p0: update with s8x16_val or keep s8x16_blk_max_val
            s8x16_blk_max_val = vpselq_s8(s8x16_val, s8x16_blk_max_val, p0);
            /* according to p0: store per-lane extrema indexes*/
            u8x16_blk_max_idx = vpselq_u8(u8x16_blk_idx, u8x16_blk_max_idx, p0);
            u8x16_blk_idx+=1;
        }
        len_arr -= 0x100;
        // Compare according to global max to create p0
        p0 = vcmpgtq_m_s8(s8x16_blk_max_val, s8x16_max_val, p);

      // update global max value
      s8x16_max_val = vpselq_s8(s8x16_blk_max_val, s8x16_max_val, p0);
      u8x16_max_idx = vpselq_u8(u8x16_blk_max_idx, u8x16_max_idx, p0);
      u8x16_max_idx_blk = vdupq_m_n_u8(u8x16_max_idx_blk, idx_blk, p0);
               idx_blk++;
    }
    // From u8 to u16 build whole index
    uint16x8x2_t   u16x8x2_idx;
    uint16x8x2_t   u16x8x2_idx_blk;
   // idx = max_idx + max_idx_blk * <block_size>
    u16x8x2_idx.val[0] = vmovlbq_u8(u8x16_max_idx);
    u16x8x2_idx_blk.val[0] = vmovlbq_u8(u8x16_max_idx_blk);
    u16x8x2_idx.val[0] = vmlaq_n_u16(u16x8x2_idx.val[0], u16x8x2_idx_blk.val[0], 256);
    // idx = max_idx + max_idx_blk * <block_size>
    u16x8x2_idx.val[1] = vmovltq_u8(u8x16_max_idx);
    u16x8x2_idx_blk.val[1] = vmovltq_u8(u8x16_max_idx_blk);
    u16x8x2_idx.val[1] = vmlaq_n_u16(u16x8x2_idx.val[1], u16x8x2_idx_blk.val[1], 256);

    vst2q_u16((uint16_t *)index,u16x8x2_idx);
    vstrbq_p_s8((int8_t *)maxim,s8x16_max_val,p);

}
void objdetect_maxi_transpose_int8(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, uint8_t *index, int32_t parallelize)
{
    int8x16_t      s8x16_max_val = vdupq_n_s8(Q7_MIN);
    uint8x16_t     u8x16_max_idx = vdupq_n_u8(0);
    parallelize = MIN(parallelize, 16);
    mve_pred16_t    p = vctp8q(parallelize);
    if (len_arr > 0x100) {
      return;
    }

    uint8x16_t u8x16_idx = vdupq_n_u8(0);
    for (int i = 0; i < len_arr; i++)
    {
        // load up to 16 int8
        int8x16_t s8x16_val = vld1q_z_s8(&arr[i*nb_total_boxes], p);
        // Compare according to p to create p0
        mve_pred16_t p0 = vcmpgtq_m_s8(s8x16_val, s8x16_max_val, p);

        // according to p0: update with s8x16_val or keep s8x16_blk_minmax_val
        s8x16_max_val = vpselq_s8(s8x16_val, s8x16_max_val, p0);
        /* according to p0: store per-lane extrema indexes*/
        u8x16_max_idx = vpselq_u8(u8x16_idx, u8x16_max_idx, p0);
        u8x16_idx+=1;

    }
    vstrbq_p_u8((uint8_t *)index,u8x16_max_idx,p);
    vstrbq_p_s8((int8_t *)maxim,s8x16_max_val,p);

}
#else
void objdetect_maxi_transpose_int8(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, int32_t *index)
{
    *index = 0;
    *maxim = arr[0];

    for (int i = 1; i < len_arr; i++)
    {
        if (arr[i*nb_total_boxes] > *maxim)
        {
            *maxim = arr[i*nb_total_boxes];
            *index = i;
        }
    }
}
void objdetect_maxi_transpose_int8_large(int8_t *arr, int32_t len_arr, int32_t nb_total_boxes, int8_t *maxim, uint16_t *index)
{
    *index = 0;
    *maxim = arr[0];

    for (int i = 1; i < len_arr; i++)
    {
        if (arr[i*nb_total_boxes] > *maxim)
        {
            *maxim = arr[i*nb_total_boxes];
            *index = (uint16_t)i;
        }
    }
}
#endif

float32_t objdetect_sigmoid_f(float32_t x)
{
    return (1.0f / (1.0f + expf(-x)));
}


void objdetect_softmax_f(float32_t *input_x, float32_t *output_x, int32_t len_x, float32_t *tmp_x)
{
    float32_t sum = 0;

    for (int32_t i = 0; i < len_x; ++i)
    {
        tmp_x[i] = expf(input_x[i]);
        sum = sum + tmp_x[i];
    }
    sum = 1.0f / sum;
    for (int32_t i = 0; i < len_x; ++i)
    {
        tmp_x[i] *= sum;
    }
    memcpy(output_x, tmp_x, len_x * sizeof(float32_t));
}


//***************iou ********
//inline
float32_t overlap(float32_t x1, float32_t w1, float32_t x2, float32_t w2)
{
    float32_t l1 = x1 - w1 / 2;
    float32_t l2 = x2 - w2 / 2;
    float32_t left = l1 > l2 ? l1 : l2;
    float32_t r1 = x1 + w1 / 2;
    float32_t r2 = x2 + w2 / 2;
    float32_t right = r1 < r2 ? r1 : r2;
    return (right - left);
}

//inline
float32_t box_intersection(float32_t *a, float32_t *b)
{
    float32_t w = overlap(a[0], a[2], b[0], b[2]);
    float32_t h = overlap(a[1], a[3], b[1], b[3]);
    if (w < 0 || h < 0) return 0;
    float32_t area = w * h;
    return (area);
}

//inline
float32_t box_union(float32_t *a, float32_t *b)
{
    float32_t i = box_intersection(a, b);
    float32_t u = a[2] * a[3] + b[2] * b[3] - i;
    return (u);
}


float32_t objdetect_box_iou(float32_t *a, float32_t *b)
{

    float32_t I = box_intersection(a, b);
    float32_t U = box_union(a, b);
    if (I == 0 || U == 0) {
        return 0;
    }
    return (I / U);
}


void transpose_flattened_2D(float32_t *arr, int32_t rows, int32_t cols, float32_t *tmp_x)
{
    int32_t i, j, k;
    int32_t len_r_c = rows * cols;

    for (i = 0; i < len_r_c; i++)
    {
        tmp_x[i] = arr[i];
    }

    for (i = 0; i < cols; i++)
    {
        for (j = 0; j < rows; j++)
        {
            k = i * rows + j;
            arr[k] = tmp_x[j * cols + i];
        }
    }
}


void dequantize(int32_t* arr, float32_t* tmp, int32_t n, int32_t zero_point, float32_t scale)
{
    for (int32_t i = 0; i < n; i++)
    {
        tmp[i] = (float32_t)(arr[i] - zero_point) * scale;
    }
}

