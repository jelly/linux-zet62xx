/*
 * Copyright (C) 2016, Jelle van der Waa <jelle@vdwaa.nl>
 *
 *  This program is free software; you can redistribute it and/or modify it
 *  under the terms of the GNU General Public License as published by the Free
 *  Software Foundation; either version 2 of the License, or (at your option)
 *  any later version.
 *
 *  This program is distributed in the hope that it will be useful, but WITHOUT
 *  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 *  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 *  more details.
 */

#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/input/touchscreen.h>
#include <linux/module.h>

#define ZET62_TS_NAME "zet62xx"

static int zet62_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;

	dev_info(dev, "zet62_ts_probe\n");


	return 0;
}

static const struct i2c_device_id zet62_id[] = {
	{ "zet62_ts", 0},
	{ "zet62xx", 0},
	{ }
};

MODULE_DEVICE_TABLE(i2c, zet62_id);

static struct i2c_driver zet62_ts_driver = {
	.probe = zet62_ts_probe,
	.driver = {
		.name = ZET62_TS_NAME
	},
	.id_table = zet62_id
};

module_i2c_driver(zet62_ts_driver);

MODULE_AUTHOR("Jelle van der Waa <jelle@vdwaa.nl>");
MODULE_DESCRIPTION("ZEITEC zet622x I2C touchscreen driver");
MODULE_LICENSE("GPL");
