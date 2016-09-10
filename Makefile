CROSS_COMPILE=arm-none-eabi-
KVER=$(shell uname -r)
KDIR=/lib/modules/$(KVER)/build

obj-m += zet6223.o

default:
	make -C ${KDIR} ARCH=arm CROSS_COMPILE=${CROSS_COMPILE} M=$(PWD) modules
