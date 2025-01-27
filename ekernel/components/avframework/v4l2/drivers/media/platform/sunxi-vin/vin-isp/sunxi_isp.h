/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/vin-isp/sunxi_isp.h
 *
 * Copyright (c) 2007-2017 Allwinnertech Co., Ltd.
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

#ifndef _SUNXI_ISP_H_
#define _SUNXI_ISP_H_
#include <linux/videodev2.h>
#include <v4l2-ctrls.h>
#include <media/v4l2-subdev.h>

#ifdef CONFIG_ARCH_DMA_ADDR_T_64BIT
typedef u64 dma_addr_t;
#else
typedef u32 dma_addr_t;
#endif

#include "../vin-video/vin_core.h"
#if defined CONFIG_SOC_SUN8IW19P1 || defined CONFIG_SOC_SUN20IW1P1
#include "isp521/isp521_reg_cfg.h"
#endif
#include "../vin-stat/vin_h3a.h"

enum isp_pad {
	ISP_PAD_SINK,
	ISP_PAD_SOURCE_ST,
	ISP_PAD_SOURCE,
	ISP_PAD_NUM,
};

struct isp_pix_fmt {
	u32 mbus_code;
	enum isp_input_seq infmt;
	u32 fourcc;
};

struct sunxi_isp_ctrls {
	struct v4l2_ctrl_handler handler;

	struct v4l2_ctrl *wb_gain[4];	/* wb gain cluster */
	struct v4l2_ctrl *ae_win[4];	/* wb win cluster */
	struct v4l2_ctrl *af_win[4];	/* af win cluster */
};

struct isp_dev {
	struct v4l2_subdev subdev;
	struct media_pad isp_pads[ISP_PAD_NUM];
	//struct v4l2_event event;
	struct sunxi_isp_ctrls ctrls;
	struct rt_mutex subdev_lock;
	struct vin_mm isp_stat;
	struct vin_mm isp_load;
	struct vin_mm isp_save;
	struct vin_mm isp_lut_tbl;
	struct vin_mm isp_drc_tbl;
	struct vin_mm d3d_pingpong[3];
	struct vin_mm wdr_pingpong[2];
#if defined CONFIG_SOC_SUN8IW19P1
	struct isp_lbc_cfg wdr_raw_lbc;
	struct isp_lbc_cfg d3d_k_lbc;
	struct isp_lbc_cfg d3d_raw_lbc;
#endif
	struct isp_size err_size;
	struct isp_debug_mode isp_dbg;
	struct isp_pix_fmt *isp_fmt;
	struct isp_size_settings isp_ob;
	struct v4l2_mbus_framefmt mf;
	struct isp_stat h3a_stat;
	void __iomem *base;
	struct rt_work s_sensor_stby_task;
	int irq;
	unsigned int ptn_isp_cnt;
	unsigned int event_lost_cnt;
	unsigned int hb_max;
	unsigned int hb_min;
	unsigned int isp_frame_number;
	unsigned char id;
	char is_empty;
	char use_isp;
	char runtime_flag;
	bool nosend_ispoff;
	bool bind_sensor;
	char have_init;
	char load_flag;
	char f1_after_librun;/*fisrt frame after server run*/
	char left_right;/*0: process left, 1: process right*/
	char use_cnt;
	char capture_mode;  //V4L2_MODE_VIDEO
	char wdr_mode;
	char sensor_lp_mode;
	char ptn_type;
	char large_image;/*2:get merge yuv, 1: get pattern raw (save in kernel), 0: normal*/
#ifdef SUPPORT_PTN
	char load_shadow[ISP_LOAD_DRAM_SIZE*3];
#else
	char load_shadow[ISP_LOAD_DRAM_SIZE];
#endif
};

void sunxi_isp_reset(struct isp_dev *isp);
void sunxi_isp_sensor_type(struct v4l2_subdev *sd, int use_isp);
void sunxi_isp_sensor_fps(struct v4l2_subdev *sd, int fps);
void sunxi_isp_debug(struct v4l2_subdev *sd, struct isp_debug_mode *isp_debug);
void sunxi_isp_ptn(struct v4l2_subdev *sd, unsigned int ptn_type);
void sunxi_isp_frame_sync_isr(struct v4l2_subdev *sd);
struct v4l2_subdev *sunxi_isp_get_subdev(int id);
struct v4l2_subdev *sunxi_stat_get_subdev(int id);
int sunxi_isp_platform_register(void);
void sunxi_isp_platform_unregister(void);

#endif /*_SUNXI_ISP_H_*/
