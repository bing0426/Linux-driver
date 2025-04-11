#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include <linux/wait.h>
#include <linux/poll.h>


#define DEV_NAME	        "my_dev"


struct my_dev_struct{
    struct cdev* c_dev;
    struct class* my_class;
    struct device* mydev;
    wait_queue_head_t my_wait;
    u8 contiation;
    u8 status;
    struct fasync_struct* my_fap;
    spinlock_t my_lock;

};

//创建对
struct my_dev_struct xxx_chardev;
char kernel_buf[128] = {0};
int ret;

int xxx_chardev_open(struct inode *inode, struct file *file){
    printk("xxx_chardev open\n");
    spin_lock(&xxx_chardev.my_lock);
    if(xxx_chardev.status != 0){
        spin_unlock(&xxx_chardev.my_lock);
        return -EBUSY;
    } 
    xxx_chardev.status = 1;
    spin_unlock(&xxx_chardev.my_lock);
    printk("pid:%d\n", current->pid);
    return 0;
}
ssize_t xxx_chardev_read(struct file *file, char __user *userbuf, size_t size, loff_t *offset){
    printk("xxx_chardev read\n");
    if(file->f_flags & O_NONBLOCK){
        if(xxx_chardev.contiation == 0){
            return -EAGAIN;
        }
        else{
            if (size > sizeof(kernel_buf))
            {
                size = sizeof(kernel_buf);
            }
            ret = copy_to_user(userbuf, kernel_buf, size);
            if (ret != 0)
            {
                printk("copy_to_user fail\n");
                return -EIO;
            }
            xxx_chardev.contiation = 0;
            return size;
        }
    }
    wait_event_interruptible(xxx_chardev.my_wait, xxx_chardev.contiation);
    xxx_chardev.contiation = 0;
    if(size > sizeof(kernel_buf)){
        size = sizeof(kernel_buf);
    }
    ret = copy_to_user(userbuf, kernel_buf, size);
    if(ret != 0 ){
        printk("copy_to_user fail\n");
        return -EIO;
    }
    return size;
    
}
ssize_t xxx_chardev_write(struct file *file, const char __user *userbuf, size_t size, loff_t *offset){
    printk("xxx_chardev write\n");
    if(size > sizeof(kernel_buf) -1){
        size = sizeof(kernel_buf) - 1;
    }
    memset(kernel_buf, 0, sizeof(kernel_buf));
    printk("%d\n",__LINE__);
    ret = copy_from_user(kernel_buf, userbuf, size);
    if(ret != 0){
        printk("copy_from_user fail \n");
        return -EIO;
    }
    xxx_chardev.contiation = 1;
    wake_up_interruptible(&xxx_chardev.my_wait);
    kill_fasync(&xxx_chardev.my_fap, SIGIO, POLL_IN);
    return size;
}
int xxx_chardev_release(struct inode *inode, struct file *file){
    printk("xxx_chardev close\n");
    xxx_chardev.status = 0;
    return 0;
}

unsigned int xxx_chardev_poll(struct file *file, struct poll_table_struct *table){
    int mark = 0;
    poll_wait(file,&xxx_chardev.my_wait, table);
    if(xxx_chardev.contiation == 1){
        return mark | POLL_IN;
    }
    return mark;
}

int xxx_chardev_fasync(int fd, struct file * file, int on){
    return fasync_helper(fd, file, on, &xxx_chardev.my_fap);
}

struct file_operations  fops = {
    .owner = THIS_MODULE,
    .open = xxx_chardev_open,
    .write= xxx_chardev_write,
    .read = xxx_chardev_read,
    .release = xxx_chardev_release,
    .poll = xxx_chardev_poll,
    .fasync = xxx_chardev_fasync,
};


//函数入口
static int __init my_sample_driver_init(void){
    printk("入口函数\n");
    //int ret;
    xxx_chardev.c_dev = cdev_alloc();
    if(xxx_chardev.c_dev == NULL){
        printk("cdev alloc err\n");
        return -ENOMEM;
    }
    //xxx_chardev.c_dev->owner = THIS_MODULE;
    cdev_init(xxx_chardev.c_dev, &fops);

    ret = alloc_chrdev_region(&(xxx_chardev.c_dev->dev), 0, 1, DEV_NAME);
    if(ret){
        printk("alloc_cahrdev_region err\n");
        cdev_del(xxx_chardev.c_dev);
        return ret;
    }

    ret = cdev_add(xxx_chardev.c_dev, xxx_chardev.c_dev->dev, 1);
    if(ret){
        printk("cdev_add err \n");
        cdev_del(xxx_chardev.c_dev);
        return ret;
    }
    xxx_chardev.my_class = class_create(THIS_MODULE,"MYLED");
    if(IS_ERR(xxx_chardev.my_class)){
        printk("class_create err \n");
        return PTR_ERR(xxx_chardev.my_class);
    }
    xxx_chardev.mydev = device_create(xxx_chardev.my_class,NULL, xxx_chardev.c_dev->dev, NULL, DEV_NAME);
    if(IS_ERR(xxx_chardev.mydev)){
        printk("device_create err\n");
        return PTR_ERR(xxx_chardev.mydev);
    }
    printk("major: %d\n", xxx_chardev.c_dev->dev);
    init_waitqueue_head(&xxx_chardev.my_wait);
    xxx_chardev.contiation = 0;
    spin_lock_init(&xxx_chardev.my_lock);
    xxx_chardev.status = 0;
    return 0;
}
//函数出口
void __exit my_sample_driver_exit(void){
    printk("出口函数\n");
    device_destroy(xxx_chardev.my_class, xxx_chardev.c_dev->dev);
    class_destroy(xxx_chardev.my_class);
    cdev_del(xxx_chardev.c_dev);
    printk("%d %d",xxx_chardev.c_dev->dev,__LINE__);
    unregister_chrdev_region(xxx_chardev.c_dev->dev, xxx_chardev.c_dev->count);
    kfree(xxx_chardev.c_dev);
    printk("all is ok\n");

}
//许可
MODULE_LICENSE("GPL");
//注册函数
module_init(my_sample_driver_init);
//注销函数
module_exit(my_sample_driver_exit);