/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/vin-mipi/protocol/protocol_reg_i.h
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


#ifndef __PROTOCOL_REG_I__H__
#define __PROTOCOL_REG_I__H__

#ifdef __cplusplus
extern "C" {
#endif

#define MIPI_CSI2_CTL_OFF               0x0000
#define MIPI_CSI2_CFG_OFF               0x0004
#define MIPI_CSI2_VCDT_RX_OFF           0x0008
#define MIPI_CSI2_RX_PKT_NUM_OFF        0x000C
#define MIPI_CSI2_VER_OFF               0x003C
#define MIPI_CSI2_CH_CFG_OFF            0x0040
#define MIPI_CSI2_CH_INT_EN_OFF         0x0050
#define MIPI_CSI2_CH_INT_PD_OFF         0x0058
#define MIPI_CSI2_CH_DT_TRM_OFF         0x0060
#define MIPI_CSI2_CH_CUR_PH_OFF         0x0070
#define MIPI_CSI2_CH_ECC_OFF            0x0074
#define MIPI_CSI2_CH_CKS_OFF            0x0078
#define MIPI_CSI2_CH_FRAME_NUM_OFF      0x007C
#define MIPI_CSI2_CH_LINE_NUM_OFF       0x0080

#define MIPI_CSI2_CH_OFF                0x0100

#define MIPI_CSI2_CTL_ADDR              (MIPI_CSI0_VBASE + MIPI_CSI2_CTL_OFF)
#define MIPI_CSI2_CFG_ADDR              (MIPI_CSI0_VBASE + MIPI_CSI2_CFG_OFF)
#define MIPI_CSI2_VCDT_RX_ADDR          (MIPI_CSI0_VBASE + MIPI_CSI2_VCDT_RX_OFF)
#define MIPI_CSI2_RX_PKT_NUM_ADDR       (MIPI_CSI0_VBASE + MIPI_CSI2_RX_PKT_NUM_OFF)
#define MIPI_CSI2_VER_ADDR              (MIPI_CSI0_VBASE + MIPI_CSI2_VER_OFF)
#define MIPI_CSI2_CH_CFG_ADDR           (MIPI_CSI0_VBASE + MIPI_CSI2_CH_CFG_OFF)
#define MIPI_CSI2_CH_INT_EN_ADDR        (MIPI_CSI0_VBASE + MIPI_CSI2_CH_INT_EN_OFF)
#define MIPI_CSI2_CH_INT_PD_ADDR        (MIPI_CSI0_VBASE + MIPI_CSI2_CH_INT_PD_OFF)
#define MIPI_CSI2_CH_DT_TRM_ADDR        (MIPI_CSI0_VBASE + MIPI_CSI2_CH_DT_TRM_OFF)
#define MIPI_CSI2_CH_CUR_PH_ADDR        (MIPI_CSI0_VBASE + MIPI_CSI2_CH_CUR_PH_OFF)
#define MIPI_CSI2_CH_ECC_ADDR           (MIPI_CSI0_VBASE + MIPI_CSI2_CH_ECC_OFF)
#define MIPI_CSI2_CH_CKS_ADDR           (MIPI_CSI0_VBASE + MIPI_CSI2_CH_CKS_OFF)
#define MIPI_CSI2_CH_FRAME_NUM_ADDR     (MIPI_CSI0_VBASE + MIPI_CSI2_CH_FRAME_NUM_OFF)
#define MIPI_CSI2_CH_LINE_NUM_ADDR      (MIPI_CSI0_VBASE + MIPI_CSI2_CH_LINE_NUM_OFF)

typedef union {
	unsigned int dwval;
	struct {
		unsigned int en:1;
		unsigned int unpk_en:1;
		unsigned int res0:28;
		unsigned int ver_en:1;
		unsigned int rst:1;
	} bits;
} MIPI_CSI2_CTL_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int dl_cfg:2;
		unsigned int res0:2;
		unsigned int ph_byte_ord:2;
		unsigned int ph_bit_ord:1;
		unsigned int pl_bit_ord:1;
		unsigned int ch_mode:2;
		unsigned int res1:22;
	} bits;
} MIPI_CSI2_CFG_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int ch0_dt:6;
		unsigned int ch0_vc:2;
		unsigned int ch1_dt:6;
		unsigned int ch1_vc:2;
		unsigned int ch2_dt:6;
		unsigned int ch2_vc:2;
		unsigned int ch3_dt:6;
		unsigned int ch3_vc:2;
	} bits;
} MIPI_CSI2_VCDT_RX_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int pkt_num;
	} bits;
} MIPI_CSI2_RX_PKT_NUM_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int ver;
	} bits;
} MIPI_CSI2_VER_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int line_sync:1;
		unsigned int embd_dat_en:1;
		unsigned int itl_sync:1;
		unsigned int src_sel:1;
		unsigned int res0:28;
	} bits;
} MIPI_CSI2_CH_CFG_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int fifo_over_int:1;
		unsigned int res0:7;
		unsigned int frame_end_sync_int:1;
		unsigned int frame_start_sync_int:1;
		unsigned int line_end_sync_int:1;
		unsigned int line_start_sync_int:1;
		unsigned int res1:4;
		unsigned int ph_update_int:1;
		unsigned int pf_int:1;
		unsigned int emb_data_int:1;
		unsigned int res2:5;
		unsigned int frame_sync_err_int:1;
		unsigned int line_sync_err_int:1;
		unsigned int ecc_err_int:1;
		unsigned int ecc_wrn_int:1;
		unsigned int chksum_err_int:1;
		unsigned int eot_err_int:1;
		unsigned int res3:2;
	} bits;
} MIPI_CSI2_CH_INT_EN_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int fifo_over_pd:1;
		unsigned int res0:7;
		unsigned int frame_end_sync_pd:1;
		unsigned int frame_start_sync_pd:1;
		unsigned int line_end_sync_pd:1;
		unsigned int line_start_sync_pd:1;
		unsigned int res1:4;
		unsigned int ph_update_pd:1;
		unsigned int pf_pd:1;
		unsigned int emb_data_pd:1;
		unsigned int res2:5;
		unsigned int frame_sync_err_pd:1;
		unsigned int line_sync_err_pd:1;
		unsigned int ecc_err_pd:1;
		unsigned int ecc_wrn_pd:1;
		unsigned int chksum_err_pd:1;
		unsigned int eot_err_pd:1;
		unsigned int res3:2;
	} bits;
} MIPI_CSI2_CH_INT_PD_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int fs:1;
		unsigned int fe:1;
		unsigned int ls:1;
		unsigned int le:1;
		unsigned int res0:4;
		unsigned int gs0:1;
		unsigned int gs1:1;
		unsigned int gs2:1;
		unsigned int gs3:1;
		unsigned int gs4:1;
		unsigned int gs5:1;
		unsigned int gs6:1;
		unsigned int gs7:1;
		unsigned int gl:1;
		unsigned int yuv:1;
		unsigned int rgb:1;
		unsigned int raw:1;
		unsigned int res1:12;
	} bits;
} MIPI_CSI2_CH_DT_TRM_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int cur_dt:6;
		unsigned int cur_vc:2;
		unsigned int res0:8;
		unsigned int cur_wc:16;
	} bits;
} MIPI_CSI2_CH_CUR_PH_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int rxd_ecc:8;
		unsigned int res0:8;
		unsigned int cal_ecc:8;
		unsigned int res1:8;
	} bits;
} MIPI_CSI2_CH_ECC_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int rxd_cks:16;
		unsigned int cal_cks:16;
	} bits;
} MIPI_CSI2_CH_CKS_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int frame_num:16;
		unsigned int res0:16;
	} bits;
} MIPI_CSI2_CH_FRAME_NUM_t;

typedef union {
	unsigned int dwval;
	struct {
		unsigned int line_num:16;
		unsigned int res0:16;
	} bits;
} MIPI_CSI2_CH_LINE_NUM_t;

#ifdef __cplusplus
}
#endif

#endif
