/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/vin-csi/parser_reg.h
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

#ifndef __CSIC__PARSER__REG__H__
#define __CSIC__PARSER__REG__H__

#include <stdbool.h>
#include <sunxi_camera_v2.h>
//#include <v4l2-types.h>
#include <v4l2-mediabus.h>

#define MAX_CH_NUM 4


/*register value*/

enum prs_mode {
	PRS_NCSI = 0,
	PRS_MCSI,
};

enum ref_pol {
	ACTIVE_LOW,		/* active low */
	ACTIVE_HIGH,		/* active high */
};

enum edge_pol {
	FALLING,		/* active falling */
	RISING,			/* active rising */
};

enum prs_if {
	/* YUV(separate syncs) */
	/* YUYV422 Interleaved or RAW * (All data in one data bus) */
	PRS_IF_INTLV = 0x00,
	PRS_IF_INTLV_16BIT = 0x01, /* 16 bit YUYV422 Interleaved */

	/* CCIR656(embedded syncs) */
	PRS_IF_BT656_1CH = 0x04,   /* BT656 1 channel */
	PRS_IF_BT1120_1CH = 0x05,  /* 16bit BT656(BT1120 like) 1 channel */
	/* BT656 2 channels (All data * interleaved in one data bus) */
	PRS_IF_BT656_2CH = 0x0c,
	/* 16bit BT656(BT1120 like) 2 channels * (All data interleaved in one data bus) */
	PRS_IF_BT1120_2CH = 0x0d,
	/* BT656 4 channels (All data * interleaved in one data bus) */
	PRS_IF_BT656_4CH = 0x0e,
	/* 16bit BT656(BT1120 like) 4 channels * (All data interleaved in one data bus) */
	PRS_IF_BT1120_4CH = 0x0f,
};

enum output_mode {
	FIELD_MODE = 0,
	FRAME_MODE = 1,
};

enum input_seq {
	/* valid input is yuv422 or yuv420*/
	SEQ_YUYV = 0,
	SEQ_YVYU,
	SEQ_UYVY,
	SEQ_VYUY,
};

enum if_data_width {
	DW_8BIT = 0,
	DW_10BIT = 1,
	DW_12BIT = 2,
	DW_8PLUS2 = 3,
	DW_2MULTI8 = 4,
};

enum seq_8plus2 {
	B6_D98_D70 = 0,
	D92_B6_D10 = 1,
	D70_D98_B6 = 2,
	D70_B6_D98 = 3,
};

/*
 *  field dt mode
 */
enum field_dt_mode {
	FIELD_VSYNC = 0,
	FIELD_ONLY = 1,
	VSYNC_ONLY = 2,
};

/*
 * data clock edge
 */
enum clk_pol {
	CLK_RISING,		/* active rising */
	CLK_FALLING,	/* active falling */
};

/*
 * input reference polarity
 */
enum refer_pol {
	REF_NEGATIVE,		/* active low */
	REF_POSITIVE,		/* active high */
};

enum field_pol {
	/* For YUV HV timing, Field polarity */
	FIELD_NEG = 0,	/* field=0 indicate odd, field=1 indicate even */
	FIELD_POS = 1,	/* field=1 indicate odd, field=0 indicate even */
	/* For BT656 timing, Field sequence */
	FIELD_TF = 0,	/* top filed first */
	FIELD_BF = 1,	/* bottom field first */
};

enum src_type {
	PROGRESSED = 0,	/* progressed */
	INTERLACE = 1,	/* interlace */
};

enum cap_mode {
	SCAP = 1,
	VCAP,
};

enum prs_input_fmt {
	FMT_RAW = 0,		/* raw stream */
	FMT_YUV422 = 3,		/* yuv422 */
	FMT_YUV420 = 4,		/* yuv420 */
};


/*
 * parser interrupt select
 */
enum prs_int_sel {
	PRS_INT_PARA0 = 0X1,
	PRS_INT_PARA1 = 0X2,
	PRS_INT_MUL_ERR = 0X4,
};

/*
 * csi buffer
 */

enum csi_buf_sel {
	CSI_BUF_0_A = 0,    /* FIFO for Y address A */
	CSI_BUF_0_B,        /* FIFO for Y address B */
	CSI_BUF_1_A,        /* FIFO for Cb address A */
	CSI_BUF_1_B,        /* FIFO for Cb address B */
	CSI_BUF_2_A,        /* FIFO for Cr address A */
	CSI_BUF_2_B,        /* FIFO for Cr address B */
};

struct bus_timing {
	enum ref_pol href_pol;
	enum ref_pol vref_pol;
	enum edge_pol pclk_sample;
	enum ref_pol field_even_pol; /*field 0/1 0:odd 1:even*/
};

struct frame_size {
	unsigned int width;	/* in pixel unit */
	unsigned int height;	/* in pixel unit */
};

struct frame_offset {
	unsigned int hoff;	/* in pixel unit */
	unsigned int voff;	/* in pixel unit */
};

struct bus_info {
	enum v4l2_mbus_type bus_if;
	struct bus_timing bus_tmg;
	u32 bus_ch_fmt[MAX_CH_NUM]; /* define the same as V4L2 */
	unsigned int ch_total_num;
};

/*
 * frame arrangement
 * Indicate that how the channel images are put together into one buffer
 */
struct frame_arrange {
	unsigned char column;
	unsigned char row;
};

struct frame_info {
	struct frame_arrange arrange;
	struct frame_size ch_size[MAX_CH_NUM];
	struct frame_offset ch_offset[MAX_CH_NUM];
	unsigned int pix_ch_fmt[MAX_CH_NUM];
	enum v4l2_field ch_field[MAX_CH_NUM]; /* define the same as V4L2 */
	unsigned int frm_byte_size;
};

/*register data struct*/

struct prs_ncsi_if_cfg {
	unsigned int pclk_shift;
	enum src_type type;
	enum field_pol field;
	enum refer_pol vref;
	enum refer_pol href;
	enum clk_pol clk;
	enum field_dt_mode field_dt;
	bool ddr_sample;
	enum seq_8plus2 seq_8_2;
	enum if_data_width dw;
	enum input_seq seq;
	enum output_mode mode;
	enum prs_if intf;
};

struct prs_mcsi_if_cfg {
	enum input_seq seq;
	enum output_mode mode;
};

struct prs_cap_mode {
	unsigned int cap_mask;
	enum cap_mode mode;
};

struct prs_signal_status {
	unsigned int pclk_sta;
	unsigned int data_sta;
};

struct prs_ncsi_bt656_header {
	unsigned int ch0_id;
	unsigned int ch1_id;
	unsigned int ch2_id;
	unsigned int ch3_id;
};

struct prs_output_size {
	unsigned int hor_len;
	unsigned int hor_start;
	unsigned int ver_len;
	unsigned int ver_start;
};

struct prs_input_para {
	unsigned int src_type;
	unsigned int input_vt;
	unsigned int input_ht;
	unsigned int input_hb;
	unsigned int input_hs;
};

struct prs_int_status {
	bool input_src_pd0;
	bool input_src_pd1;
	bool mul_err_pd;
};

struct prs_fps_ds {
	unsigned int ch0_fps_ds;
	unsigned int ch1_fps_ds;
	unsigned int ch2_fps_ds;
	unsigned int ch3_fps_ds;
};

int csic_prs_set_base_addr(unsigned int sel, unsigned long addr);
void csic_prs_enable(unsigned int sel);
void csic_prs_disable(unsigned int sel);
void csic_prs_mode(unsigned int sel, enum prs_mode mode);
void csic_prs_pclk_en(unsigned int sel, unsigned int en);
void csic_prs_ncsi_en(unsigned int sel, unsigned int en);
void csic_prs_mcsi_en(unsigned int sel, unsigned int en);
void csic_prs_ch_en(unsigned int sel, unsigned int en);
void csic_prs_ncsi_if_cfg(unsigned int sel, struct prs_ncsi_if_cfg *if_cfg);
void csic_prs_mcsi_if_cfg(unsigned int sel, struct prs_mcsi_if_cfg *if_cfg);
void csic_prs_capture_start(unsigned int sel, unsigned int ch_total_num,
				struct prs_cap_mode *mode);
void csic_prs_capture_stop(unsigned int sel);
void csic_prs_fps_ds(unsigned int sel, struct prs_fps_ds *prs_fps_ds);
void csic_prs_signal_status(unsigned int sel,
				struct prs_signal_status *status);
void csic_prs_ncsi_bt656_header_cfg(unsigned int sel,
				struct prs_ncsi_bt656_header *header);
void csic_prs_input_fmt_cfg(unsigned int sel, unsigned int ch,
				enum prs_input_fmt fmt);
void csic_prs_output_size_cfg(unsigned int sel, unsigned int ch,
				struct prs_output_size *size);
void csic_prs_set_pclk_dly(unsigned int sel, unsigned int pclk_dly);
/*for csic sync*/
void csic_prs_sync_en_cfg(unsigned int sel, struct csi_sync_ctrl *sync);
void csic_prs_sync_en(unsigned int sel, struct csi_sync_ctrl *sync);
void csic_prs_sync_cfg(unsigned int sel, struct csi_sync_ctrl *sync);
void csic_prs_sync_wait_N(unsigned int sel, struct csi_sync_ctrl *sync);
void csic_prs_sync_wait_M(unsigned int sel, struct csi_sync_ctrl *sync);

void csic_prs_xs_en(unsigned int sel, struct csi_sync_ctrl *sync);
void csic_prs_xs_period_len_register(unsigned int sel, struct csi_sync_ctrl *sync);
/* for debug */
void csic_prs_input_para_get(unsigned int sel, unsigned int ch,
				struct prs_input_para *para);
void csic_prs_int_enable(unsigned int sel, unsigned int ch,
				enum prs_int_sel interrupt);
void csic_prs_int_disable(unsigned int sel, unsigned int ch,
				enum prs_int_sel interrupt);
void csic_prs_int_get_status(unsigned int sel, unsigned int ch,
				struct prs_int_status *status);
void csic_prs_int_clear_status(unsigned int sel, unsigned int ch,
				enum prs_int_sel interrupt);

#endif /* __CSIC__PARSER__REG__H__ */
