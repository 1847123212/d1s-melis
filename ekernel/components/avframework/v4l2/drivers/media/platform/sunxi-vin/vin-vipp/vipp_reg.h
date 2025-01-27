/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/vin-vipp/vipp_reg.h
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
 *
 * Authors:  Zhao Wei <zhaowei@allwinnertech.com>
 *
 * This software is licensed under the terms of the GNU General Public
 * License version 2, as published by the Free Software Foundation, and
 * may be copied, distributed, and modified under those terms.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 */

#ifndef __VIPP__REG__H__
#define __VIPP__REG__H__

#include <melis/typedef.h>

#if defined CONFIG_ARCH_SUN8IW16P1
#define MAX_OVERLAY_NUM 8
#define OSD_PARA_SIZE 0
#define OSD_STAT_SIZE (2048 * 8)
#define MAX_OSD_NUM 2
#define MAX_COVER_NUM 8
#define MAX_ORL_NUM 0
#elif defined CONFIG_SOC_SUN8IW19P1
#define MAX_OVERLAY_NUM 0
#define OSD_PARA_SIZE 0
#define OSD_STAT_SIZE 0
#define MAX_OSD_NUM 2
#define MAX_COVER_NUM 0
#define MAX_ORL_NUM 16
#elif defined CONFIG_ARCH_SUN8IW12P1
#define MAX_OVERLAY_NUM 64
#define OSD_PARA_SIZE (MAX_OVERLAY_NUM * 8 + MAX_COVER_NUM * 8 + MAX_COVER_NUM * 8)
#define OSD_STAT_SIZE (MAX_OVERLAY_NUM * 8)
#define MAX_OSD_NUM VIN_MAX_SCALER
#define MAX_COVER_NUM 8
#define MAX_ORL_NUM 0
#else
#define MAX_OVERLAY_NUM 0
#define OSD_PARA_SIZE 0
#define OSD_STAT_SIZE 0
#define MAX_OSD_NUM 0
#define MAX_COVER_NUM 0
#define MAX_ORL_NUM 0
#endif

#define VIPP_REG_SIZE 0X400

/*register value*/
enum vipp_ready_flag {
	NOT_READY = 0,
	HAS_READY = 1,
};

enum vipp_update_flag {
	NOT_UPDATED = 0,
	HAS_UPDATED = 1,
};

enum vipp_status_sel {
	REG_LOAD_PD = 0X1,
	BM_ERROR_PD = 0X4,
	STATUS_ALL = 0X5,
};

enum vipp_format {
	YUV420 = 0,
	YUV422 = 1,
};

enum vipp_osd_argb {
	ARGB1555 = 0,
	ARGB4444 = 1,
	ARGB8888 = 2,
};

/*register data struct*/

struct vipp_version {
	unsigned int ver_big;
	unsigned int ver_small;
};

struct vipp_feature_list {
	unsigned int yuv422to420;
	unsigned int osd_exit;
};

struct vipp_status {
	unsigned int reg_load_pd;
	unsigned int bm_error_pd;
};

struct vipp_scaler_config {
	enum vipp_format sc_out_fmt;
	unsigned int sc_x_ratio;
	unsigned int sc_y_ratio;
	unsigned int sc_w_shift;
};

struct vipp_scaler_size {
	unsigned int sc_width;
	unsigned int sc_height;
};

struct vipp_osd_config {
	unsigned char osd_ov_en;
	unsigned char osd_cv_en;
	unsigned char osd_orl_en;
	enum vipp_osd_argb osd_argb_mode;
	unsigned char osd_stat_en;
	unsigned int osd_orl_width;
	short osd_ov_num;
	short osd_cv_num;
	short osd_orl_num;
};

struct vipp_rgb2yuv_factor {
	unsigned int jc0;
	unsigned int jc1;
	unsigned int jc2;
	unsigned int jc3;
	unsigned int jc4;
	unsigned int jc5;
	unsigned int jc6;
	unsigned int jc7;
	unsigned int jc8;
	unsigned int jc9;
	unsigned int jc10;
	unsigned int jc11;
};

struct vipp_crop {
	unsigned int hor;
	unsigned int ver;
	unsigned int width;
	unsigned int height;
};

struct vipp_osd_overlay_cfg {
	unsigned int h_start;
	unsigned int h_end;
	unsigned int v_start;
	unsigned int v_end;
	unsigned int alpha;
	unsigned int inv_en;
	unsigned int inv_th;
	unsigned int inv_w_rgn;
	unsigned int inv_h_rgn;
};

struct vipp_osd_cover_cfg {
	unsigned int h_start;
	unsigned int h_end;
	unsigned int v_start;
	unsigned int v_end;
};

struct vipp_osd_cover_data {
	unsigned int y;
	unsigned int u;
	unsigned int v;
};

struct vipp_osd_para_config {
	struct vipp_osd_overlay_cfg overlay_cfg[MAX_OVERLAY_NUM];
	struct vipp_osd_cover_cfg cover_cfg[MAX_COVER_NUM];
	struct vipp_osd_cover_data cover_data[MAX_COVER_NUM];
	struct vipp_osd_cover_cfg orl_cfg[MAX_ORL_NUM];
	struct vipp_osd_cover_data orl_data[MAX_ORL_NUM];
};

int vipp_set_base_addr(unsigned int id, unsigned long addr);
void vipp_top_clk_en(unsigned int id, unsigned int en);
void vipp_enable(unsigned int id);
void vipp_disable(unsigned int id);
void vipp_ver_en(unsigned int id, unsigned int en);
void vipp_version_get(unsigned int id, struct vipp_version *v);
void vipp_feature_list_get(unsigned int id, struct vipp_feature_list *fl);

void vipp_set_para_ready(unsigned int id, enum vipp_ready_flag flag);
void vipp_set_osd_ov_update(unsigned int id, enum vipp_update_flag flag);
void vipp_set_osd_cv_update(unsigned int id, enum vipp_update_flag flag);
void vipp_set_osd_para_load_addr(unsigned int id, unsigned long dma_addr);
int vipp_map_osd_para_load_addr(unsigned int id, unsigned long vaddr);
void vipp_set_osd_stat_load_addr(unsigned int id, unsigned long dma_addr);
void vipp_set_osd_bm_load_addr(unsigned int id, unsigned long dma_addr);
void vipp_set_reg_load_addr(unsigned int id, unsigned long dma_addr);
int vipp_map_reg_load_addr(unsigned int id, unsigned long vaddr);
void vipp_get_status(unsigned int id, struct vipp_status *status);
void vipp_clr_status(unsigned int id, enum vipp_status_sel sel);

void vipp_scaler_en(unsigned int id, unsigned int en);
void vipp_osd_en(unsigned int id, unsigned int en);
void vipp_chroma_ds_en(unsigned int id, unsigned int en);
void vipp_scaler_cfg(unsigned int id, struct vipp_scaler_config *cfg);
void vipp_scaler_output_fmt(unsigned int id, enum vipp_format);
void vipp_scaler_output_size(unsigned int id, struct vipp_scaler_size *size);

void vipp_output_fmt_cfg(unsigned int id, enum vipp_format fmt);
void vipp_osd_cfg(unsigned int id, struct vipp_osd_config *cfg);
void vipp_osd_rgb2yuv(unsigned int id, struct vipp_rgb2yuv_factor *factor);
void vipp_set_crop(unsigned int id, struct vipp_crop *crop);
void vipp_osd_hvflip(unsigned int id, int hflip, int vflip);
void vipp_osd_inverse(unsigned int id, int *inverse, int cnt);
void vipp_osd_para_cfg(unsigned int id, struct vipp_osd_para_config *para,
				struct vipp_osd_config *cfg);

#endif /* __VIPP__REG__H__ */
