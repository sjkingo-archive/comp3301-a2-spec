OBJ=cryptodev.o

obj-m += $(OBJ)
MOD_DIR=/lib/modules/$(shell uname -r)/build

.PHONY: all
all:
	make -C $(MOD_DIR) M=$(PWD) modules

.PHONY: clean
clean:
	make -C $(MOD_DIR) M=$(PWD) clean

.PHONY: load
load: all
	sudo insmod cryptodev.ko
	dmesg | tail -n 1

.PHONY: unload
unload:
	sudo rmmod cryptodev.ko
	dmesg | tail -n 1
