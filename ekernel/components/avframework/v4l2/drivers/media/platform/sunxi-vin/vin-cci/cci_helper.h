/*
 * ekernel/components/avframework/v4l2/platform/sunxi-vin/vin-cci/cci_helper.h
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

/*
 ******************************************************************************
 *
 * cci_helper.h
 *
 * Hawkview ISP - cci_helper.h module
 *
 * Copyright (c) 2014 by Allwinnertech Co., Ltd.  http://www.allwinnertech.com
 *
 * Version         Author         Date          Description
 *
 *   2.0         Yang Feng     2014/06/06      Second Version
 *
 ******************************************************************************
 */

#ifndef __VFE__I2C__H__
#define __VFE__I2C__H__

#include <v4l2-subdev.h>
#include <v4l2-device.h>
enum cci_dev_type {
	CCI_TYPE_SENSOR,
	CCI_TYPE_ACT,
	CCI_TYPE_FLASH,
};

struct cci_driver {
	unsigned short id;
	char name[32];
	enum cci_dev_type type;
	struct rt_device cci_device;
	unsigned short cci_id;
	unsigned short cci_saddr;
	struct v4l2_subdev *sd;
	unsigned int is_registerd;
	unsigned int is_matched;

	int addr_width;
	int data_width;
	int read_flag;
	short read_value;
	struct rt_mutex cci_mutex;
	struct list_head cci_list;
};

struct reg_list_a8_d8 {
	unsigned char addr;
	unsigned char data;
};

struct reg_list_a8_d16 {
	unsigned char addr;
	unsigned short data;
};

struct reg_list_a16_d8 {
	unsigned short addr;
	unsigned char data;
};

struct reg_list_a16_d16 {
	unsigned short addr;
	unsigned short data;
};

struct reg_list_w_a16_d16 {
	unsigned short width;
	unsigned short addr;
	unsigned short data;
};

typedef unsigned short addr_type;
typedef unsigned short data_type;

extern void csi_cci_init_helper(unsigned int sel);
extern void csi_cci_exit_helper(unsigned int sel);

extern int cci_read_a8_d8(struct v4l2_subdev *sd, unsigned char addr,
			  unsigned char *value);
extern int cci_write_a8_d8(struct v4l2_subdev *sd, unsigned char addr,
			   unsigned char value);
extern int cci_read_a8_d16(struct v4l2_subdev *sd, unsigned char addr,
			   unsigned short *value);
extern int cci_write_a8_d16(struct v4l2_subdev *sd, unsigned char addr,
			    unsigned short value);
extern int cci_read_a16_d8(struct v4l2_subdev *sd, unsigned short addr,
			   unsigned char *value);
extern int cci_write_a16_d8(struct v4l2_subdev *sd, unsigned short addr,
			    unsigned char value);
extern int cci_read_a16_d16(struct v4l2_subdev *sd, unsigned short addr,
			    unsigned short *value);
extern int cci_write_a16_d16(struct v4l2_subdev *sd, unsigned short addr,
			     unsigned short value);
extern int cci_write_a0_d16(struct v4l2_subdev *sd, unsigned short value);
extern int cci_write_a16_d8_continuous_helper(struct v4l2_subdev *sd,
					      unsigned short addr,
					      unsigned char *vals, uint size);
extern int cci_read(struct v4l2_subdev *sd, addr_type addr, data_type *value);
extern int cci_write(struct v4l2_subdev *sd, addr_type addr, data_type value);

extern struct v4l2_subdev *cci_bus_match(char *name, unsigned short cci_id,
					 unsigned short cci_saddr);
extern void cci_bus_match_cancel(struct cci_driver *cci_drv_p);

extern void cci_lock(struct v4l2_subdev *sd);
extern void cci_unlock(struct v4l2_subdev *sd);

extern int cci_dev_probe_helper(struct v4l2_subdev *sd,
				const struct v4l2_subdev_ops *sensor_ops,
				struct cci_driver *cci_drv);
extern struct v4l2_subdev *cci_dev_remove_helper(struct cci_driver *cci_drv);

#endif /*__VFE__I2C__H__*/
