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

#include <linux/gpio/consumer.h>
#include <linux/interrupt.h>
#include <linux/i2c.h>
#include <linux/input.h>
#include <linux/input/mt.h>
#include <linux/input/touchscreen.h>
#include <linux/of.h>
#include <linux/module.h>

#define ZET62_TS_NAME "zet62xx"
#define CMD_INFO 0xB2
#define CMD_INFO_LENGTH 17

struct zet62xx_data {
	struct i2c_client *client;
	struct input_dev *input;
	struct touchscreen_properties prop;
};

static int zet62_ts_start(struct input_dev *dev)
{
		struct zet62xx_data *data = input_get_drvdata(dev);
		dev_info(&data->client->dev, "ts_start\n");

		return 0;
}

static void zet62_ts_stop(struct input_dev *dev)
{
		struct zet62xx_data *data = input_get_drvdata(dev);
}

static irqreturn_t irqreturn_t_zet62xx(int irq, void *dev_id)
{
	struct zet62xx_data *data = dev_id;
	struct device *dev = &data->client->dev;

	dev_info(dev, "irqreturn!\n");

	return IRQ_HANDLED;
}

static int zet62_ts_probe(struct i2c_client *client, const struct i2c_device_id *id)
{
	struct device *dev = &client->dev;
	struct zet62xx_data *data;
	struct input_dev *input;
	u8 buf[CMD_INFO_LENGTH];
	u8 cmd = CMD_INFO;
	int ret, max_x, max_y, fingernum;

	if (!client->irq) {
		dev_err(dev, "Error no irq specified\n");
		return -EINVAL;
	}

	dev_info(dev, "zet62_ts_probe\n");

	data = devm_kzalloc(dev, sizeof(*data), GFP_KERNEL);
	if (!data)
		return -ENOMEM;

	ret = i2c_master_send(client, &cmd, 1);
	if (ret < 0) {
		dev_err(dev, "touchpanel info cmd failed : %d\n", ret);
		return -ENODEV;
	}

	ret = i2c_master_recv(client, buf, CMD_INFO_LENGTH);
	if (ret < 0) {
		dev_err(dev, "cannot retrieve touchpanel info data : %d\n", ret);
		return -ENODEV;
	}

	max_x = buf[9] & 0xff;
	max_x = (max_x << 8) | (buf[8] & 0xff);

	max_y = buf[11] & 0xff;
	max_y = (max_y << 8) | (buf[10] & 0xff);

	fingernum = buf[15] && 0x7f;
	dev_info(dev, "resolution-x: %d, resolution-y: %d, fingernum: %d\n", max_x, max_y, fingernum);

	input = devm_input_allocate_device(dev);
	if (!input)
		return -ENOMEM;

	input_set_abs_params(input, ABS_MT_POSITION_X, 0, max_x, 0, 0);
	input_set_abs_params(input, ABS_MT_POSITION_Y, 0, max_y, 0, 0);
	touchscreen_parse_properties(input, true, &data->prop);

	input->name = client->name;
	input->id.bustype = BUS_I2C;
	input->dev.parent = dev;
	input->open = zet62_ts_start;
	input->close = zet62_ts_stop;

	ret = input_mt_init_slots(input, fingernum, INPUT_MT_DIRECT | INPUT_MT_DROP_UNUSED | INPUT_MT_TRACK);
	if (ret)
		return ret;

	data->client = client;
	data->input = input;

	input_set_drvdata(input, data);
	dev_info(dev, "client->irq %d\n", client->irq);

	ret = devm_request_threaded_irq(dev, client->irq, NULL, irqreturn_t_zet62xx,
					  IRQF_ONESHOT, client->name, data);
	if (ret) {
		dev_err(dev, "Error requesting irq: %d\n", ret);
		return ret;
	}

	ret = input_register_device(input);
	if (ret)
		return ret;

	i2c_set_clientdata(client, data);

	return 0;
}

static const struct of_device_id zet62_of_match[] = {
	{ .compatible = "zeitec,zet62xx" },
	{ }
};
MODULE_DEVICE_TABLE(of, zet62_of_match);

static const struct i2c_device_id zet62_id[] = {
	{ "zet62_ts", 0},
	{ "zet62xx", 0},
	{ }
};
MODULE_DEVICE_TABLE(i2c, zet62_id);

static struct i2c_driver zet62_ts_driver = {
	.driver = {
		.name = ZET62_TS_NAME,
		.of_match_table = of_match_ptr(zet62_of_match),
	},
	.probe = zet62_ts_probe,
	.id_table = zet62_id
};

module_i2c_driver(zet62_ts_driver);

MODULE_AUTHOR("Jelle van der Waa <jelle@vdwaa.nl>");
MODULE_DESCRIPTION("ZEITEC zet622x I2C touchscreen driver");
MODULE_LICENSE("GPL");
