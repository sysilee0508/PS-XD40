#ifeq ($(KERNELRELEASE),)
#export MMZ_SRC_BASE=$(PWD)
#endif

DATE = $(shell date '+%y%m%d_%H%M%S')

VIDEO_DECODER_PATH:=$(PWD)/../
KERNEL_PATH?=$(LINUX_ROOT)

obj-m := raptor4.o
raptor4-objs := raptor4_drv.o raptor4_common.o raptor4_table.o raptor4_video.o raptor4_function.o
#jaguar1-objs := raptor4_drv.o raptor4_i2c.o raptor4_video.o raptor4_coax_protocol.o raptor4_motion.o raptor4_video_additional_set.o raptor4_video_eq.o
#jaguar1-objs +=video_eq.o

#ifeq ($(BUILD_SOC),hi3531a) To do.
ifeq ($(BUILD_SOC),)
  ifeq ($(PARAM_FILE), )
  	PARAM_FILE:=../../Makefile.param
  		include $(PARAM_FILE)
  endif
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

  EXTRA_CFLAGS += -Wframe-larger-than=4096

# add Show/hide NVP6134(C) function (2016-09-10, sat)
# add enhancing video quality(AHD 4M/5M) (2016-11-23, wed)
 
  EXTRA_CFLAGS += -I$(VIDEO_DECODER_PATH)
  endif
endif

default:
	@make -C $(KERNEL_PATH) M=$(PWD) modules 
ifeq ($(HIARCH),hi3520D)
	cp *.ko /nfs/hi3520d/app/ko/extdrv/
endif
ifeq ($(HIARCH),hi3521a)
	cp *.ko /nfs/$(shell whoami)/raptor4/hi3520d/raptor4.ko
	#cp *.ko /nfs/hi3521a/app/ko/extdrv/
endif
ifeq ($(HIARCH),hi3531)
	cp *.ko /nfs/hi3531/ko/extdrv/
endif
ifeq ($(HIARCH),hi3531a)
#	cp *.ko /nfs/hi3531a/app/ko/extdrv/
	cp *.ko /nfs/$(shell whoami)/raptor4/raptor4.ko
#	cp *.ko /nfs/$(shell whoami)/jaguar1/jaguar1_$(DATE).ko
endif
clean: 
	@make -C $(KERNEL_PATH) M=$(PWD) clean 


