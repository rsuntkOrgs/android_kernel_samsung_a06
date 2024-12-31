/*
 * cx2560x battery charging driver
 *
 * Copyright (C) 2013 SGM
 *
 * This package is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 * THIS PACKAGE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */
#ifndef _LINUX_CX2560X_I2C_H
#define _LINUX_CX2560X_I2C_H
#include <linux/power_supply.h>
struct cx2560x_charge_param {
	int vlim;
	int ilim;
	int ichg;
	int vreg;
};
enum stat_ctrl {
	STAT_CTRL_STAT,
	STAT_CTRL_ICHG,
	STAT_CTRL_INDPM,
	STAT_CTRL_DISABLE,
};
enum vboost {
	BOOSTV_4850 = 4850,
	BOOSTV_5000 = 5000,
	BOOSTV_5150 = 5150,
	BOOSTV_5300 = 5300,
};
enum iboost {
	BOOSTI_500 = 500,
	BOOSTI_1200 = 1200,
};
enum vac_ovp {
	VAC_OVP_5500 = 5500,
	VAC_OVP_6500 = 6500,
	VAC_OVP_10500 = 10500,
	VAC_OVP_14000 = 14000,
};
struct cx2560x_platform_data {
	struct cx2560x_charge_param usb;
	int iprechg;
	int iterm;
	enum stat_ctrl statctrl;
	enum vboost boostv;	// options are 4850,
	enum iboost boosti; // options are 500mA, 1200mA
	enum vac_ovp vac_ovp;
};

/* A06 code for P240514-07707 by xiongxiaoliang at 20240524 start */
extern void cx2560_afc_switch_ctrl(bool level);
/* A06 code for P240514-07707 by xiongxiaoliang at 20240524 end */
#endif
