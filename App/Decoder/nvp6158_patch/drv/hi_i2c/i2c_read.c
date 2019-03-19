#include <stdio.h>
#include <ctype.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <unistd.h>

#include "strfunc.h"
//#include "hi_i2c.h"

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


unsigned int reg_width  = 1;
unsigned int data_width = 1;
unsigned int reg_step   = 1;

int main(int argc , char* argv[])
{
    int fd = -1;
    int ret;
 //   I2C_DATA_S  i2c_data ;
    unsigned int device_addr, reg_addr, reg_addr_end, reg_value;
	unsigned char recvbuf[2];

    if (argc < 3)
    {
    	printf("usage: %s <device_addr> <reg_addr> <end_reg_addr> <reg_width> <data_width> <reg_step>. sample: \n", argv[0]);
        printf("------------------------------------------------------------------------------------\n");
        printf("\t\t%s 0x56 0x0 0x10 2 2. \n", argv[0]);
        printf("\t\t%s 0x56 0x0 0x10 2 2 2. \n", argv[0]);
        printf("\t\t%s 0x56 0x0 0x10. default reg_width, data_width, reg_step is 1. \n", argv[0]);

        return -1;
    }

    fd = open("/dev/i2c-1", O_RDWR);
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

    if(argc > 3)
    {
        if (StrToNumber(argv[3], &reg_addr_end))
        {
            close(fd);
            return 0;
        }
        if (reg_addr_end < reg_addr)
        {
            printf("end addr(0x%2x) should bigger than start addr(0x%x)\n", reg_addr_end, reg_addr);
            close(fd);
            return 0;
        }

        if((argc > 4) && StrToNumber(argv[4], &reg_width))
        {
            close(fd);
            return 0;
        }

        if((argc > 5) && StrToNumber(argv[5], &data_width))
        {
            close(fd);
            return 0;
        }
        if ((argc > 6) && StrToNumber(argv[6], &reg_step))
        {
            close(fd);
            return 0;
        }
    }
    else
    {
        reg_addr_end = reg_addr;
    }


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

    printf("dev_addr:0x%2x; reg_addr:0x%2x; reg_addr_end:0x%2x; reg_width: %d; data_width: %d. \n\n", device_addr,
            reg_addr, reg_addr_end, reg_width, data_width);
    {
        int cur_addr;

        for (cur_addr = reg_addr; cur_addr < reg_addr_end + reg_width; cur_addr += reg_step)
        {
		/*
            i2c_data.dev_addr       = device_addr ;
            i2c_data.reg_addr       = cur_addr;
            i2c_data.addr_byte_num  = reg_width;
            i2c_data.data_byte_num  = data_width;
		*/
			recvbuf[0] = cur_addr & 0xFF;
			recvbuf[1] = 0x00;

            ret = read(fd, recvbuf, 1);
            if (ret < 0)
            {
            	printf("CMD_I2C_READ error!\n");
				close(fd);
            	return -1;
            }

			reg_value = recvbuf[0];
            printf("0x%x 0x%x\n", cur_addr, reg_value);
        }
    }

    close(fd);

    return ret;
}
