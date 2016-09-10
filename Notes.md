Android source dmesg info
--------------------------

Resolution (800 x 480 ), finger_num=10, key_num=0

Firmware seems to be:
[  871.070255] [ZET] RST = LOW
[  871.070625] [ZET]: find /vendor/modules/zet62xx.bin
[  871.070708] [ZET]: Load from /vendor/modules/zet62xx.bin
[  871.218034] [VFE]mclk off


Datasheet
---------

http://linux-sunxi.org/images/2/22/ZP-HW-PS-0003_ZET6221_Product_Spec_v1_2.pdf

To get I2C working
------------------

Add the following to the dts file for your tablet, dts file I used was:
sun8i-a23-q8-tablet.dts. And uses and ZET6221 touchscreen.

```
&i2c0 {
	pinctrl-names = "default";
	pinctrl-0 = <&i2c0_pins_a>;
	status = "okay";

	//clock-frequency = <400000>;

	zet62xx: touchscreen@76 {
		 compatible = "zeitec,zet62xx";
		 reg = <0x76>;
		 interrupt-parent = <&pio>;
		 interrupts = <1 5 IRQ_TYPE_EDGE_FALLING>; /* PB5 */
		 power-gpios = <&pio 7 1 GPIO_ACTIVE_HIGH>; /* PH1 */
		 touchscreen-fw-name = "zeitec/zet62xx.fw";
	};
};

&pio {
	ts_power_pin_mid2407: ts_power_pin@0 {
		allwinner,pins = "PH1";
		allwinner,function = "gpio_out";
		allwinner,drive = <SUN4I_PINCTRL_10_MA>;
		allwinner,pull = <SUN4I_PINCTRL_NO_PULL>;
	};
};
```

The sun5i a13 inet 98v rev2 features a ZET6223 which does not require custom
firmware to be loaded into the device.

```
&i2c1 {
	pinctrl-names = "default";
	pinctrl-0 = <&i2c1_pins_a>;
	status = "okay";

	pcf8563: rtc@51 {
		compatible = "nxp,pcf8563";
		reg = <0x51>;
	};

	zet62xx: touchscreen@76 {
		 compatible = "zeitec,zet62xx";
		 reg = <0x76>;
		 interrupt-parent = <&pio>;
		 interrupts = <6 11 IRQ_TYPE_EDGE_FALLING>; /* PG11 */
		 power-gpios = <&pio 1 3 GPIO_ACTIVE_HIGH>; /* PB3 */
		 touchscreen-fw-name = "zeitec/zet62xx.fw";
	};
};
```

I2C Detection
-------------

Detection of slave address. (Or 1 for the a13 inet 98v rev2)

$ i2cdetect -y 0 

Slave address 0x76.

Fex file
---------

```
[ctp_para]
ctp_used = 1
ctp_name = "zet6221-ts"
ctp_twi_id = 0
ctp_twi_addr = 0x76
ctp_screen_max_x = 800
ctp_screen_max_y = 480
ctp_revert_x_flag = 0
ctp_revert_y_flag = 0
ctp_exchange_x_y_flag = 0
ctp_int_port = port:PB05<4><default><default><default>
ctp_wakeup = port:PH01<1><default><default><1>
```

Protocol
--------

Writing 0xB2 to the i2c device and then reading 17 bytes will return information
about the finger application information.

In C

```
int fd, ret;
unsigned char input[25];

fd = open("/dev/i2c-1", O_RDWR);
assert(fd > 0);

ret = ioctl(fd, I2C_SLAVE, SLAVE_ADDR);
assert(ret >= 0);

sleep(1);

input[0] = 0xB2;

ret = write(fd, input, 1);
assert(ret == 1);

ret = read(fd, input, 17);
assert(ret == 17);

int resolution_x = input[9] & 0xff;
resolution_x = (resolution_x << 8) | (input[8] & 0xff);

int resolution_y = input[11] & 0xff;
resolution_y = (resolution_y << 8) | (input[10] & 0xff);

int fingernum = (input[15] & 0x7f);
int keynum = (input[15] & 0x80);
printf("resolution x: %d, y: %d, keynum: %d, fingernum: %d\n", resolutioo

```

Input data
----------

The first byte contains the control number, 0x3C if the data is valid.
The second and third bytes contain the touches. For example:
0x80 - One touch
0xC0 - Two touches
0xC4 - Three touches.

For the first touch (finger) the X axis is the 4 bits MSB of byte 3 and 8 bits of byte 4.
Y axis is the 4 bit LSB of byte 3 and 8 bits of byte 5.

Linux error codes
-----------------

Linux error codes can be found in usr/include/asm-generic/errno-base.h and errno.h
