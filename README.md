WIP Linux mainline driver for the zet62xx touchscreen.
======================================================


Drivers found in the wild (Android based):

* https://github.com/wingrime/zet6221-ts-drv
* https://github.com/ChrisP-Android/BananaPi-Android-4.2.2-Liab/blob/master/lichee/linux-3.4/drivers/input/sw_touchscreen/zet622x.c
* https://groups.google.com/forum/#!topic/linux-sunxi/HPEOxmWT7NI 

[Fex file](https://github.com/linux-sunxi/sunxi-boards/blob/26d49398df4abcc5919bfd9d213df1997d9a434f/sys_config/a23/qa88_v1.3_2014_03_15.fex) for the tablet

GPIO ctp_reset

gpio_wakeup_hdle = gpio_request_ex("ctp_para", "ctp_wakeup");

Set reset low
// gpio_write_one_pin_value(gpio_wakeup_hdle, 0, "ctp_wakeup")

mdelay(1)

Set reset high
gpio_write_one_pin_value(gpio_wakeup_hdle, 1, "ctp_wakeup")


ZET 6251 Blue tablet.

Blue grote tablet A13 Denver u-boot =>  inet98v_rev2_defconfig
ZET 6223
