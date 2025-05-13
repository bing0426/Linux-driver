ARCH = arm
#MODULE_NAME := $(basename $(notdir $(wildcard $(PWD)/*.c)))
#$(info $(wildcard *.c))
#$(info "MODULE_NAME" = $(MODULE_NAME))
obj-m := ap3216_drv.o
CROSS_COMPILE = arm-buildroot-linux-gnueabihf-
#KERNEL_DIR = /lib/modules/4.18.0-15-generic/build
KERNEL_DIR = /home/book/100ask_imx6ull-sdk/Linux-4.9.88
PWD := $(shell pwd)
$(info "PWD" = $(PWD))
all:
	make -C ${KERNEL_DIR} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} modules M=${PWD}
clean:
	make -C ${KERNEL_DIR} ARCH=${ARCH} CROSS_COMPILE=${CROSS_COMPILE} clean M=${PWD}