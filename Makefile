user:
	gcc -o user_program user_program.c

all: user
	make -C ./my_module

ins_mod:
	sudo rmmod my_module
	sudo insmod my_module/my_module.ko
	sudo chmod a+rw /dev/my_new_dev

clean:
	rm -f user_program
	make -C ./my_module clean

# sudo dmesg			- show debug kernel log
# sudo dmesg -C			- clear debug kernel log
