#include <fcntl.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>

#include "my_module/my_module.h"

int main(int argc, char *argv[]) {
    if (argc < 3) {
        printf("Вам необходимо ввести struct_id (0 - thread_group_cputimer, 1 - memblock_type,"
               " 2 - block_device) и при необходимости второй аргумент!\n");
        return 0;
    } else if (argc > 3) {
        printf("Некорректное количество аргументов!\n");
        return 0;
    }

    int struct_id = atoi(argv[1]);
    int dev;

    if (struct_id == 0) {
        struct my_thread_group_cputimer mtgct;
        mtgct.pid = atoi(argv[2]);

        printf("\nОткрытие драйвера...");
        dev = open("/dev/my_new_dev", O_RDWR);
        if (dev == -1) {
            printf("\nОшибка при открытии /dev/my_new_dev! Возможно не хватает прав!\n");
            return -1;
        }

        printf("Запись данных в драйвер...\n");
        if (ioctl(dev, WR_CPU_TIMER, (struct my_thread_group_cputimer *) &mtgct)){
            printf("произошла ошибка во время записи данных в драйвер...\n");
        }

        printf("Чтение данных из драйвера...\n");
        if(ioctl(dev, RD_CPU_TIMER, (struct my_thread_group_cputimer *) &mtgct)){
            printf("произошла ошибка во время чтения данных из драйвера...\n");
        }

        if (mtgct.valid == true) {
            printf("\nthread_group_cpu_timer struct, для PID %d: \n", mtgct.pid);

            if (!mtgct.timer_exist) {
                printf("thread_group_cpu_timer не запущен!\n");
            } else {
                printf("\tutime = %ld\n", mtgct.utime);
                printf("\tstime = %ld\n", mtgct.stime);
                printf("\tsum_exec_runtime = %ld\n", mtgct.sum_exec_runtime);
            }
        } else {
            printf("\nОшибка! Заданный PID не существует!");
        }
        printf("\nЗакрытие драйвера...\n");
        close(dev);
    } else if (struct_id == 1) { // TODO doesn't work, it needs kernel recompilation, go to the file mm/memblock.c and paste EXPORT_SYMBOL(memblock);
        printf("Функция временно не реализована, потому что нужно пересобирать ядро!\n");
/*
        struct my_memblock_type mmt;
        printf("\nОткрытие драйвера...");
        dev = open("/dev/my_new_dev", O_RDWR);
        if (dev == -1){
            printf("\nОшибка при открытии /dev/my_new_dev! Возможно не хватает прав!\n");
            return -1;
        }

        printf("\nОткрытие прошло успешно!\n");
        printf("Запись данных в драйвер...\n");
        if(ioctl(dev, WR_CPU_TIMER, (struct my_memblock_type*) &mmt)){
            printf("произошла ошибка во время работы драйвера...\n");
        }

        printf("Чтение данных из драйвера...\n");
        if(ioctl(dev, RD_CPU_TIMER, (struct my_memblock_type*) &mmt)){
            printf("произошла ошибка во время чтения данных из драйвера...\n");
        }

        printf("\tcnt = %lu\n", mmt.cnt);
        printf("\tmax = %lu\n", mmt.max);
        printf("\ttotal_size = %llu\n", mmt.total_size);

        printf("\nЗакрытие драйвера...\n");
        close(dev);
*/
    }  else if (struct_id == 2) {
        struct my_block_device mbd;
        mbd.device_path = argv[2];
        printf("\nОткрытие драйвера...");
        dev = open("/dev/my_new_dev", O_RDWR);
        if (dev == -1) {
            printf("\nОшибка при открытии /dev/my_new_dev! Возможно не хватает прав!\n");
            return -1;
        }

        printf("\nОткрытие прошло успешно!\n");
        printf("Запись данных в драйвер...\n");
        if (ioctl(dev, WR_BLOCK_DEVICE, (struct my_block_device *) &mbd)){
            printf("произошла ошибка во время работы драйвера...\n");
        }

        printf("Чтение данных из драйвера...\n");
        if(ioctl(dev, RD_BLOCK_DEVICE, (struct my_block_device *) &mbd)){
            printf("произошла ошибка во время чтения данных из драйвера...\n");
        }

        if (mbd.valid == true) {
            printf("\nblock_device struct, with path: %s\n", mbd.device_path);

            printf("\tbd_dev = %u\n", mbd.bd_dev);
            printf("\tbd_openers = %d\n", mbd.bd_openers);
            printf("\tbd_inode = %p\n", mbd.bd_inode);
            printf("\tbd_super = %p\n", mbd.bd_super);
            printf("\tbd_claiming = %p\n", mbd.bd_claiming);
            printf("\tbd_holder = %p\n", mbd.bd_holder);
            printf("\tbd_holders = %d\n", mbd.bd_holders);
            printf("\tbd_holders = %s\n", mbd.bd_write_holder ? "true" : "false");
            printf("\tbd_contains = %p\n", mbd.bd_contains);
            printf("\tbd_block_size = %u\n", mbd.bd_block_size);
            printf("\tbd_part = %p\n", mbd.bd_part);
            printf("\tbd_part_count = %u\n", mbd.bd_part_count);
            printf("\tbd_invalidated = %d\n", mbd.bd_invalidated);
            printf("\tbd_disk = %p\n", mbd.bd_disk);
            printf("\tbd_queue = %p\n", mbd.bd_queue);
            printf("\tbd_bdi = %p\n", mbd.bd_bdi);
            printf("\tbd_private = %lu\n", mbd.bd_private);
            printf("\tbd_fsfreeze_count = %d\n", mbd.bd_fsfreeze_count);
        } else {
            printf("\nОшибка! Нкорректный путь к файлу блочного устройства!");
        }
        printf("\nЗакрытие драйвера...\n");
        close(dev);
    } else {
        printf("\nОшибка! Введенный struct_id не существует!\n");
        return 0;
    }
}
