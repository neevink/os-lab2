#ifndef __MY_MUDULE_H__
#define __MY_MUDULE_H__


#define MAJOR_NUM 100

// Макросы, используемые для создания команд ioctl.
#define WR_CPU_TIMER _IOW(MAJOR_NUM, 1, struct my_thread_group_cputimer*)
#define WR_MEMBLOCK_TYPE _IOW(MAJOR_NUM, 2, struct my_memblock_type*)
#define WR_BLOCK_DEVICE _IOW(MAJOR_NUM, 3, struct my_block_device*)
#define RD_CPU_TIMER _IOR(MAJOR_NUM, 4, struct my_thread_group_cputimer*)
#define RD_MEMBLOCK_TYPE _IOR(MAJOR_NUM, 5, struct my_memblock_type*)
#define RD_BLOCK_DEVICE _IOR(MAJOR_NUM, 6, struct my_block_device*)


struct my_thread_group_cputimer {
    bool valid;
    bool timer_exist;

    long utime;
    long stime;
    long sum_exec_runtime;

    pid_t pid;
};

struct my_memblock_type {
    bool valid;

    unsigned long cnt;
    unsigned long max;
    unsigned long long total_size;
    struct memblock_region *regions;
    char *name;

    int memblock_field;
};

struct my_block_device {
    bool valid;

    unsigned int bd_dev;
    int bd_openers;
    struct inode *bd_inode;
    struct super_block *bd_super;
    void *bd_claiming;
    void *bd_holder;
    int bd_holders;
    bool bd_write_holder;
    struct block_device *bd_contains;
    unsigned bd_block_size;
    struct hd_struct *bd_part;
    unsigned bd_part_count;
    int bd_invalidated;
    struct gendisk *bd_disk;
    struct request_queue *bd_queue;
    struct backing_dev_info *bd_bdi;
    unsigned long bd_private;
    int bd_fsfreeze_count;

    char* device_path;
};

#endif
