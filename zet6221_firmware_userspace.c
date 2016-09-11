#include <assert.h>
#include <stdio.h>
#include <linux/i2c-dev.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#define SLAVE_ADDR 0x76

int main(int argc, char *argv[])
{
        int fd, ret;

        fd = open("/dev/i2c-1", O_RDWR);
        assert(fd > 0);

        ret = ioctl(fd, I2C_SLAVE, SLAVE_ADDR);
        assert(ret >= 0);

	// TODO: how to set gpio pin low..
	printf("Make sure the RESET PIN IS LOW\n");


	printf("Send password\n");
	int pwd_cmd[3] = {0x20, 0xC5, 0x9D};
	ret = write(fd, pwd_cmd, 3);
	assert(ret == 3);

        usleep(20000);

	int sfr_cmd[1] = {0x2C};
	int sfr_in_data[16];
	// Otherwise sfr_cmd_data[0] is random garbage
	int sfr_cmd_data[17] = {0};

	write(fd, sfr_cmd, 1);
	assert(ret == 1);

	ret = read(fd, sfr_in_data, 16);
	assert(ret == 16);

	for (int i = 0; i < 16; i++) {
		sfr_cmd_data[i+1] = sfr_in_data[i];
	}

	if (sfr_in_data[14] == 0x3D) { // Operation failed?
		sfr_cmd_data[0] = 0x2D;
		ret = write(fd, sfr_cmd, 1);
		assert(ret == 1);

		sfr_cmd_data[15] = 0x3D;
		sfr_cmd_data[0] = 0x2B;
		ret = write(fd, sfr_cmd_data, 17);
		assert(ret == 17);
	} else {
		sfr_cmd_data[15] = 0x3D;
		sfr_cmd_data[0] = 0x2B;
		ret = write(fd, sfr_cmd_data, 17);
		assert(ret == 17);
	}

	usleep(20000);

	int erase_cmd[1] = {0x24};
	ret = write(fd, erase_cmd, 1);
	assert(ret == 1);

}
