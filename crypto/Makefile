# Change this to the object file you will be generating
OBJ=crypto.o
OBJS=a2.o buffer.o

# WARNING!
# Don't change anything past this point unless you know what you are doing!!
# WARNING!

obj-m += $(OBJ)
crypto-objs := $(OBJS)

KOBJ=$(OBJ:.o=.ko)
MOD_DIR=/lib/modules/$(shell uname -r)/build

.PHONY: all
all:
	make -C $(MOD_DIR) M=$(PWD) modules

.PHONY: clean
clean:
	make -C $(MOD_DIR) M=$(PWD) clean

.PHONY: load
load: all
	cp -p $(KOBJ) /tmp/
	sudo insmod /tmp/$(KOBJ)

.PHONY: unload
unload:
	sudo rmmod $(OBJ:.o=)
	rm -f /tmp/$(KOBJ)
