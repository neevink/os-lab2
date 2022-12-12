#include <asm/page.h>
#include <asm/pgtable.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/export.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/ioctl.h>
#include <linux/kernel.h>
#include <linux/memblock.h>
#include <linux/mm.h>
#include <linux/mm_types.h>
#include <linux/module.h>
#include <linux/mount.h>
#include <linux/namei.h>
#include <linux/pagemap.h>
#include <linux/path.h>
#include <linux/pci.h>
#include <linux/pid.h>
#include <linux/proc_fs.h>
#include <linux/sched.h>
#include <linux/sched/cputime.h>
#include <linux/sched/signal.h>
#include <linux/seq_file.h>
#include <linux/uaccess.h>
#include <linux/vmalloc.h>

#include "my_module.h"


MODULE_LICENSE("Dual BSD/GPL");
MODULE_AUTHOR("Кирилл Неевин P33111");
MODULE_DESCRIPTION("ОСИ | Лабораторная работа №2 | ioctl: block_device, memblock_type, thread_group_cputimer");
MODULE_VERSION("1.0");

dev_t first = 0; //Global variable for the first device number
static struct cdev c_dev; //Global variable for the character device structure
static struct class *cl; //Global variable for the device class

struct task_struct *ts;
struct my_thread_group_cputimer mtgct;
struct my_memblock_type mmt;
struct my_block_device mbd;

static ssize_t my_ictl_read(struct file *file, char __user *buf, size_t len, loff_t *off);
static ssize_t my_ictl_write(struct file *file, const char *buf, size_t len, loff_t *off);
static int my_ictl_open(struct inode *inode, struct file *file);
static long my_ictl_ioctl(struct file *file, unsigned int cmd, unsigned long arg);
static int my_ictl_release(struct inode *inode, struct file *file);
static int __init ofcd_init(void);
static void __exit ofcd_exit(void);

void thread_group_cputimer_handler(void);
void memblock_type_handler(void);
void block_device_handler(void);


static struct file_operations fops =
        {
                .owner          = THIS_MODULE,
                .read           = my_ictl_read,
                .write          = my_ictl_write,
                .open           = my_ictl_open,
                .unlocked_ioctl = my_ictl_ioctl,
                .release        = my_ictl_release,
        };

struct mutex etx_mutex;

static int my_ictl_open(struct inode *inode, struct file *file) {
    mutex_lock(&etx_mutex);
    printk(KERN_INFO "Была вызвана функция открытия!\n");
    return 0;
}

static int my_ictl_release(struct inode *inode, struct file *file) {
    mutex_unlock(&etx_mutex);
    printk(KERN_INFO "Была вызвана функция закрытия!\n");
    return 0;
}

static ssize_t my_ictl_read(struct file *file, char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "Была вызвана функция чтения!\n");
    return 0;
}

static ssize_t my_ictl_write(struct file *file, const char __user *buf, size_t len, loff_t *off) {
    printk(KERN_INFO "Была вызвана функция записи!\n");
    return 0;
}

static long my_ictl_ioctl(struct file *file, unsigned int cmd, unsigned long arg) {
    switch (cmd) {
        case WR_CPU_TIMER:
            if (copy_from_user(&mtgct, (struct my_thread_group_cputimer *) arg, sizeof(mtgct))) {
                printk(KERN_ERR "Ошибка копирования данных от пользователя!\n");
            }
            thread_group_cputimer_handler();
            break;
        case WR_MEMBLOCK_TYPE:
            if (copy_from_user(&mmt, (struct my_memblock_type *) arg, sizeof(mmt))) {
                printk(KERN_ERR "Ошибка копирования данных от пользователя!\n");
            }
            memblock_type_handler();
            break;
        case WR_BLOCK_DEVICE:
            if (copy_from_user(&mbd, (struct my_block_device *) arg, sizeof(mbd))) {
                printk(KERN_ERR "Ошибка копирования данных от пользователя!\n");
            }
            block_device_handler();
            break;
        case RD_CPU_TIMER:
            if (copy_to_user((struct my_thread_group_cputimer *) arg, &mtgct, sizeof(mtgct))) {
                printk(KERN_ERR "Ошибка копирования данных пользователю!\n");
            }
            break;
        case RD_MEMBLOCK_TYPE:
            if (copy_to_user((struct my_memblock_type *) arg, &mmt, sizeof(mmt))) {
                printk(KERN_ERR "Ошибка копирования данных пользователю!\n");
            }
            break;
        case RD_BLOCK_DEVICE:
            if (copy_to_user((struct my_block_device *) arg, &mbd, sizeof(mbd))) {
                printk(KERN_ERR
                "Ошибка копирования данных пользователю!\n");
            }
            break;
        default:
            printk(KERN_INFO "Заданная команда нереализована!");
            break;
    }
    return 0;
}

static int __init ofcd_init(void) {
    mutex_init(&etx_mutex);

    //регистрация MAJOR and MINOR
    if ((alloc_chrdev_region(&first, 0, 1, "my_new_dev")) < 0) {
        printk(KERN_ERR "Не удалось зарегистрировать диапазон символьных номеров устройств\n");
        return -1;
    }
    //создание класса устройства
    if ((cl = class_create(THIS_MODULE, "my_new_class")) == NULL) {
        printk(KERN_ERR "Невозможно создать структуру класса\n");
        unregister_chrdev_region(first, 1);
        return -1;
    }
    //заполнение класса информацией об устройстве (MAJOR,MINOR)
    if ((device_create(cl, NULL, first, NULL, "my_new_dev")) == NULL) {
        printk(KERN_ERR "Невозможно создать девайс\n");
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    //заполняем структуру устройства необходимыми функциями
    cdev_init(&c_dev, &fops);
    //добавляем девайс в систему
    if ((cdev_add(&c_dev, first, 1)) == -1) {
        printk(KERN_ERR "Невозможно добавить девайс в систему\n");
        device_destroy(cl, first);
        class_destroy(cl);
        unregister_chrdev_region(first, 1);
        return -1;
    }
    printk(KERN_INFO "Установка драйвера произошла успешно!\n");
    return 0;
}

static void __exit ofcd_exit(void) {
    cdev_del(&c_dev);
    device_destroy(cl, first);
    class_destroy(cl);
    unregister_chrdev_region(first, 1);
    printk(KERN_INFO "Девайс уничтожен!");
}


void thread_group_cputimer_handler() {
    struct thread_group_cputimer *timer;
    ts = get_pid_task(find_get_pid(mtgct.pid), PIDTYPE_PID);

    if (ts == NULL) {
        mtgct.valid = false;
        return;
    } else {
        mtgct.valid = true;
    }

    timer = get_running_cputimer(ts);
    if (timer == NULL) {
        mtgct.timer_exist = false;
        return;
    } else {
        mtgct.timer_exist = true;
    }

    mtgct.utime = timer->cputime_atomic.utime.counter;
    mtgct.stime = timer->cputime_atomic.stime.counter;
    mtgct.sum_exec_runtime = timer->cputime_atomic.sum_exec_runtime.counter;
}

void memblock_type_handler() {
//    struct memblock_type mem = memblock.memory; // TODO need kernel recompilation
//    mmt = (struct my_memblock_type){
//        .cnt = mem.cnt,
//        .max = mem.max,
//        .total_size = mem.total_size,
//        *regions,
//        *name,
//    };
}

void block_device_handler() {
    char *path_name = mbd.device_path;
    struct block_device *bdev_raw = lookup_bdev(path_name);
    if (IS_ERR(bdev_raw)) {
        mbd.valid = false;
        printk("error opening raw device <%lu>\n", PTR_ERR(bdev_raw));
        return;
    }

    mbd.valid = true;

    mbd.bd_dev = bdev_raw->bd_dev;
    mbd.bd_openers = bdev_raw->bd_openers;
    mbd.bd_inode = bdev_raw->bd_inode;
    mbd.bd_super = bdev_raw->bd_super;
    mbd.bd_claiming = bdev_raw->bd_claiming;
    mbd.bd_holder = bdev_raw->bd_holder;
    mbd.bd_holders = bdev_raw->bd_holders;
    mbd.bd_holders = bdev_raw->bd_holders;
    mbd.bd_write_holder = bdev_raw->bd_write_holder;
    mbd.bd_block_size = bdev_raw->bd_block_size;
    mbd.bd_part = bdev_raw->bd_part;
    mbd.bd_part_count = bdev_raw->bd_part_count;
    mbd.bd_invalidated = bdev_raw->bd_invalidated;
    mbd.bd_disk = bdev_raw->bd_disk;
    mbd.bd_queue = bdev_raw->bd_queue;
    mbd.bd_bdi = bdev_raw->bd_bdi;
    mbd.bd_private = bdev_raw->bd_private;
    mbd.bd_fsfreeze_count = bdev_raw->bd_fsfreeze_count;
}


module_init(ofcd_init);
module_exit(ofcd_exit);
