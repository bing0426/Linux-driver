arch = x86
kernel_dir = /lib/modules/5.4.0-150-generic/build
pwd = $(shell pwd)
all:
	make -C ${kernel_dir} ARCH=${arch} modules M=${pwd}
clean:
	make -C ${kernel_dir} ARCH=${arch} clean M=${pwd}

obj-m += my_test_driver.o
	