#include <linux/module.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/uaccess.h>
#include <linux/io.h>
#include <linux/types.h>
#include <linux/device.h>
#include <linux/cdev.h>
#include <linux/slab.h>
#include "./requestcmd.h"


#define DEV_NAME	        "my_dev"
#define CCM_CCGR_CG_	    0x020c406C
#define IOMUXC_SNVS_SW_		0x02290014
#define IOMUXC_CLK_ENABLE_	0x020c4078
#define GPIO_DR_            0x020AC000
#define GPIO_GDIR_          0x020AC004        


struct my_imx6ull_dev
    {   
        //GPIO5 CG15 30-31
        uint32_t *CCM_CCGR_CG;// = 0x020c406C;
        //0-3 ALT5 0x5 
        uint32_t *IOMUXC_SNVS_SW;// = 0x02290014;
        //iomuxc clock 2-3
        uint32_t *IOMUXC_CLK_ENABLE;// = 0x020c4078;
        uint32_t *GPIO_DR;// = 0x020AC000;
        uint32_t *GPIO_GDIR;// = 0x020AC004;
         
    };

struct my_dev_struct{
    struct cdev* c_dev;
    struct my_imx6ull_dev my_imx6ull;
    struct class* my_class;
    struct device* mydev;
};

//创建对
struct my_dev_struct xxx_chardev;
char kernel_buf[128] = {0};
int ret;

int xxx_chardev_open(struct inode *inode, struct file *file){
    printk("xxx_chardev open\n");
    return 0;
}
ssize_t xxx_chardev_read(struct file *file, char __user *userbuf, size_t size, loff_t *offset){
    printk("xxx_chardev read\n");
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
    if(kernel_buf[0] == '0'){
        *xxx_chardev.my_imx6ull.GPIO_DR |= (0x1<<3);
    }
    if(kernel_buf[0] == '1'){
        *xxx_chardev.my_imx6ull.GPIO_DR &= ~(0x1<<3);
    }
    return size;
}
int xxx_chardev_release(struct inode *inode, struct file *file){
    printk("xxx_chardev close\n");
    return 0;
}
long xxx_chardev_ioctl(struct file * file, unsigned int cmd, unsigned long args){
    enum LED_STATUS status;
    int ret;
    printk("%s %d",__FUNCTION__, __LINE__);
    ret = copy_from_user(&status, (void __user *)args, sizeof(status));
    if(ret){
        printk("copy_from_user err \n");
        return (long) ret;
    }
    switch(status){
        case LED_ON : {*xxx_chardev.my_imx6ull.GPIO_DR &= ~(0x1<<3);printk("%d\n",__LINE__);}break;
        case LED_OFF : {*xxx_chardev.my_imx6ull.GPIO_DR |= (0x1<<3);printk("%d\n",__LINE__);}break;
        default:
            printk("status err\n");
            break;
    }
    printk("%d\n",__LINE__);
    return 0;
}

struct file_operations  fops = {
    .open = xxx_chardev_open,
    .write= xxx_chardev_write,
    .read = xxx_chardev_read,
    .release = xxx_chardev_release,
    .unlocked_ioctl = xxx_chardev_ioctl
};


//函数入口
static int __init my_sample_driver_init(void){
    printk("入口函数\n");
    //int ret;
    xxx_chardev.my_imx6ull.IOMUXC_CLK_ENABLE = ioremap(IOMUXC_CLK_ENABLE_, 4);
    *xxx_chardev.my_imx6ull.IOMUXC_CLK_ENABLE |= (0x3<<1);
	xxx_chardev.my_imx6ull.CCM_CCGR_CG = ioremap(CCM_CCGR_CG_, 4);
	*xxx_chardev.my_imx6ull.CCM_CCGR_CG |= (0x3<<30);
	xxx_chardev.my_imx6ull.IOMUXC_SNVS_SW = ioremap(IOMUXC_SNVS_SW_, 4);
	*xxx_chardev.my_imx6ull.IOMUXC_SNVS_SW |= (0x5);
	*xxx_chardev.my_imx6ull.IOMUXC_SNVS_SW &= ~(0x1<<2);
	xxx_chardev.my_imx6ull.GPIO_GDIR = ioremap(GPIO_GDIR_,4);
	*xxx_chardev.my_imx6ull.GPIO_GDIR |= (0x1<<3);
	xxx_chardev.my_imx6ull.GPIO_DR = ioremap(GPIO_DR_,4);	
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
    return 0;
}
//函数出口
void __exit my_sample_driver_exit(void){
    printk("出口函数\n");
    device_destroy(xxx_chardev.my_class, xxx_chardev.c_dev->dev);
    class_destroy(xxx_chardev.my_class);
    cdev_del(xxx_chardev.c_dev);
    unregister_chrdev_region(xxx_chardev.c_dev->dev, xxx_chardev.c_dev->count);
    iounmap(xxx_chardev.my_imx6ull.GPIO_GDIR);
    iounmap(xxx_chardev.my_imx6ull.GPIO_DR);
    iounmap(xxx_chardev.my_imx6ull.IOMUXC_CLK_ENABLE);
    iounmap(xxx_chardev.my_imx6ull.IOMUXC_SNVS_SW);
    iounmap(xxx_chardev.my_imx6ull.CCM_CCGR_CG);
    printk("all is ok\n");

}
//许可
MODULE_LICENSE("GPL");
//注册函数
module_init(my_sample_driver_init);
//注销函数
module_exit(my_sample_driver_exit);