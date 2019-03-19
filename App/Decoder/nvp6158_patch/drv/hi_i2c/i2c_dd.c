#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "strfunc.h"

/* /dev/i2c-X ioctl commands.  The ioctl's parameter is always an
 * unsigned long, except for:
 *      - I2C_FUNCS, takes pointer to an unsigned long
 *      - I2C_RDWR, takes pointer to struct i2c_rdwr_ioctl_data
 *      - I2C_SMBUS, takes pointer to struct i2c_smbus_ioctl_data
 */

#define I2C_RETRIES     0x0701  /* number of times a device address should
				   be polled when not acknowledging */
#define I2C_TIMEOUT     0x0702  /* set timeout in units of 10 ms */
/* NOTE: Slave address is 7 or 10 bits, but 10-bit addresses
 * are NOT supported! (due to code brokenness)
 */
#define I2C_SLAVE       0x0703  /* Use this slave address */
#define I2C_SLAVE_FORCE 0x0706  /* Use this slave address, even if it
				   is already in use by a driver! */
#define I2C_TENBIT      0x0704  /* 0 for 7 bit addrs, != 0 for 10 bit */

#define I2C_FUNCS       0x0705  /* Get the adapter functionality mask */

#define I2C_RDWR        0x0707  /* Combined R/W transfer (one STOP only) */

#define I2C_PEC         0x0708  /* != 0 to use PEC with SMBus */
#define I2C_SMBUS       0x0720  /* SMBus transfer */
#define I2C_16BIT_REG   0x0709  /* 16BIT REG WIDTH */
#define I2C_16BIT_DATA  0x070a  /* 16BIT DATA WIDTH */


int main(int argc , char* argv[])
{
    int fd = -1;
    int ret;
	unsigned int i,j;
    unsigned int device_addr, reg_addr, reg_value;
	unsigned char recvbuf[2];

    if (argc < 3)
    {
    	printf("usage: <device_addr> <reg_addr> <data_value>\n" );
        return -1;
    }

    fd = open("/dev/i2c-1", O_RDWR); // Hi3531D + NVP Decoder Connected i2c_num : 1
    if (fd<0)
    {
    	printf("Open i2c-1 dev error!\n");
    	return -1;
    }

    if (StrToNumber(argv[1], &device_addr))
    {
        close(fd);
    	return 0;
    }
    if (StrToNumber(argv[2], &reg_addr))
    {
        close(fd);
    	return 0;
    }

    printf("dev_addr:0x%2x; bank_addr:0x%2x; \n", device_addr, reg_addr);
	if(argc == 3)
    {
        printf("--------------------[BANK = %02x]----------------------\n",reg_addr);
        printf("     00 01 02 03 04 05 06 07 08 09 0A 0B 0C 0D 0E 0F\n");
        printf("----------------------------------------------------\n");

		ret = ioctl(fd, I2C_SLAVE_FORCE, device_addr); // Slave Address 
		if (ret < 0) {
			printf("CMD_SET_DEV error!\n");
			close(fd);
			return -1;
		}

		ret = ioctl(fd, I2C_16BIT_REG, 0);

		if (ret < 0) {
			printf("CMD_SET_REG_WIDTH error!\n");
			close(fd);
			return -1;
		}

		ret = ioctl(fd, I2C_16BIT_DATA, 0);

		if (ret < 0) {
			printf("CMD_SET_DATA_WIDTH error!\n");
			close(fd);
			return -1;
		}

		recvbuf[0] = 0xFF;
		recvbuf[1] = reg_addr & 0xFF;

		ret = write(fd, recvbuf, 2 ); // Bank Change

		if(ret < 0)
		{
			printf("I2C_WRITE error!\n");
			close(fd);
			return -1;
		}

		reg_addr = 0x00;
		recvbuf[0] = 0x00;
		recvbuf[1] = reg_addr & 0xFF;

        for(j = 0 ; j < 16; j++)
        {
			printf("%02x | ",j*0x10);

			for(i = 0; i < 16; i++)
			{
				reg_addr = i+(0x10*j);

				recvbuf[0] = reg_addr & 0xff;

				ret = read(fd, recvbuf, 1);

				if (ret < 0) {
					printf("CMD_I2C_READ error!\n");
					close(fd);
					return -1;
				}

				reg_value = recvbuf[0];

				printf("%02x ", reg_value);


			}
			printf("\n");
		}
		printf("\n");
    }

    close(fd);

    return ret;
}
