ifneq ($(KERNELRELEASE),)
	obj-m := luirc.o

else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD)  modules

load:
	sudo insmod luirc.ko

clean:
	rm luirc.mod.c luirc.ko modules.order Module.symvers Module.markers luirc.o luirc.mod.o

endif
