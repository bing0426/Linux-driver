arch = arm
CROSS_COMPILE = arm-buildroot-linux-gnueabihf-
#kernel_dir = /lib/modules/4.18.0-15-generic/build
kernel_dir = /home/book/100ask_imx6ull-sdk/Linux-4.9.88
pwd = $(shell pwd)
all:
	make -C ${kernel_dir} ARCH=${arch} CROSS_COMPILE=${CROSS_COMPILE} modules M=${pwd}
clean:
	make -C ${kernel_dir} ARCH=${arch} CROSS_COMPILE=${CROSS_COMPILE} clean M=${pwd}

obj-m += my_sample_driver.o
	