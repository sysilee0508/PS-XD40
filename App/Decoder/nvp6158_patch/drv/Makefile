#ifeq ($(KERNELRELEASE),)
#export MMZ_SRC_BASE=$(PWD)
#endif
DATE = $(shell date '+%y%m%d_%H%M%S')

ifeq ($(PARAM_FILE), )
	PARAM_FILE:=../../../Makefile.param
		include $(PARAM_FILE)
endif
obj-m := raptor3.o
raptor3-objs := raptor3_drv.o video_output.o video_input.o video_auto_detect.o clock.o coax_protocol.o motion.o audio.o video_eq.o

ifeq ($(HIARCH),hi3520D)
EXTRA_CFLAGS += -DHI_GPIO_I2C
EXTRA_CFLAGS += -I$(PWD)/../gpio_i2c
EXTRA_CFLAGS += -DSUPPORT_HI3520D
endif
ifeq ($(HIARCH),hi3521a)
EXTRA_CFLAGS += -DHI_CHIP_HI3521A
#EXTRA_CFLAGS += -DI2C_INTERNAL
EXTRA_CFLAGS += -DHI_I2C
EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += -I$(PWD)/../hi_i2c
EXTRA_CFLAGS += $(DRV_CFLAGS)
EXTRA_CFLAGS += -DSUPPORT_3520D_6158C
endif
ifeq ($(HIARCH),hi3531)
EXTRA_CFLAGS += -DHI_GPIO_I2C
EXTRA_CFLAGS += -I$(PWD)/../gpio_i2c_8b
endif
ifeq ($(HIARCH),hi3531a)
EXTRA_CFLAGS += -DHI_CHIP_HI3531A
EXTRA_CFLAGS += -DI2C_INTERNAL
EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)
endif
ifeq ($(HIARCH),hi3531d)
EXTRA_CFLAGS += -DHI_CHIP_HI3531D
EXTRA_CFLAGS += -DI2C_INTERNAL
EXTRA_CFLAGS += -I$(REL_INC)
EXTRA_CFLAGS += $(DRV_CFLAGS)
endif

EXTRA_CFLAGS += -Wframe-larger-than=4096

VIDEO_DECODER_PATH:=$(PWD)/../

# add Show/hide NVP6134(C) function (2016-09-10, sat)
EXTRA_CFLAGS += -D__DEC_HIDE_SHOW_FUNCTION

# add enhancing video quality(AHD 4M/5M) (2016-11-23, wed)
EXTRA_CFLAGS += -D__ENHANCING_VIDEO_QUALITY_OF_AHD

# For HIKVISION EQ & AHD/CVI Long Distance Cable Distinguish
EXTRA_CFLAGS += -DTVI_FMT_COAX_DEBUG

# add for DEBUG
#EXTRA_CFLAGS += -D__DEBUG_MSG
#EXTRA_CFLAGS += -D__DEBUG_COLORBAR

EXTRA_CFLAGS += -I$(VIDEO_DECODER_PATH)

default:
	@make -C $(LINUX_ROOT) M=$(PWD) modules
ifeq ($(HIARCH),hi3520D)
	cp *.ko /nfs/hi3520d/app/ko/extdrv/
endif
ifeq ($(HIARCH),hi3521a)
#	cp *.ko /nfs/$(shell whoami)/raptor3/hi3521a/raptor3.ko
endif
ifeq ($(HIARCH),hi3531)
#	cp *.ko /nfs/hi3531/ko/extdrv/
endif
ifeq ($(HIARCH),hi3531a)
	cp *.ko /nfs/$(shell whoami)/raptor3/raptor3.ko
endif
ifeq ($(HIARCH),hi3531d)
#	cp *.ko /nfs/$(shell whoami)/raptor3/hi3531d/raptor3.ko
endif
clean:
	@make -C $(LINUX_ROOT) M=$(PWD) clean


