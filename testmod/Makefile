OBJ=testmod.o
CRYPTODEV_DIR="/home/comp3301/comp3301-2012/assigns/a2/cryptodev"

obj-m += $(OBJ)
MOD_DIR=/lib/modules/$(shell uname -r)/build
KBUILD_EXTMOD="$(CRYPTODEV_DIR)"

.PHONY: all
all:
	make -C $(MOD_DIR) M=$(PWD) modules

.PHONY: clean
clean:
	make -C $(MOD_DIR) M=$(PWD) clean

.PHONY: load
load: all
	sudo insmod testmod.ko
	dmesg | tail -n 1

.PHONY: unload
unload:
	sudo rmmod testmod.ko
	dmesg | tail -n 1
