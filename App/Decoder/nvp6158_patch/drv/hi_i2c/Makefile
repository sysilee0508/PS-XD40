ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../../../Makefile.param
	include $(PARAM_FILE)
endif

#LINUX_ROOT = ../../../../../../osdrv/opensource/kernel/linux-3.10.y/


EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)
EXTRA_CFLAGS += $(LINUX_ROOT)/include

all:
	$(CC) -g -Wall -o dec i2c_dec.c strfunc.c
	$(CC) -g -Wall -o dd i2c_dd.c strfunc.c
	$(CC) -g -Wall -o i2c_read i2c_read.c strfunc.c
	$(CC) -g -Wall -o i2c_write i2c_write.c strfunc.c

clean:
	make -C $(LINUX_ROOT) M=$(PWD) clean
	rm -f  dd dec i2c_read i2c_write

