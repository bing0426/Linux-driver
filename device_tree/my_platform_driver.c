#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>
#include <linux/slab.h>
#include <linux/cdev.h>
#include <linux/device.h>
#include <linux/fs.h>
#include <linux/types.h>
#include <linux/io.h>
#include <linux/uaccess.h>

#define DEV_NAME	        "my_platform_device"

struct my_imx6ull_dev{   
        //GPIO5 CG15 30-31
        uint32_t *CCM_CCGR_CG;// = 0x020c406C;
        //0-3 ALT5 0x5 
        uint32_t *IOMUXC_SNVS_SW;// = 0x02290014;
        //iomuxc clock 2-3
        uint32_t *IOMUXC_CLK_ENABLE;// = 0x020c4078;
        uint32_t *GPIO_DR;// = 0x020AC000;
        uint32_t *GPIO_GDIR;// = 0x020AC004;
         
    };

char kernel_buf[128] = {0};    

struct my_platform_device
{   
    struct cdev* my_dev;
    struct class* my_class;
    struct device* my_device;
    struct my_imx6ull_dev my_imx6ull;
};

struct my_platform_device my_led_device = {0};

int my_open(struct inode *inode, struct file *file){
    printk("open  %d\n",__LINE__ );
    return 0;
}

ssize_t my_read(struct file *file, char __user *userbuf, size_t size, loff_t *offset){
    printk("read  %d\n",__LINE__ );
    return size;
}

ssize_t my_write(struct file *file, const char __user *userbuf, size_t size, loff_t *offset){
    int ret;
    printk("write  %d\n",__LINE__ );
    if(size > sizeof(kernel_buf)){
        size = sizeof(kernel_buf);
    }
    ret = copy_from_user(kernel_buf, userbuf, size);
    if(ret != 0){
        printk("copy_from_user fail \n");
        return -EIO;
    }
    if(kernel_buf[0] == '1'){
        *my_led_device.my_imx6ull.GPIO_DR &= ~(0x1<<3);
    }
    if(kernel_buf[0] == '0'){
        *my_led_device.my_imx6ull.GPIO_DR |= (0x1<<3);
    }
    return size;
}

int my_close(struct inode *inode, struct file *file){
    printk("close  %d\n",__LINE__ );
    return 0;
}

struct file_operations fps = {
    .open = my_open,
    .read = my_read,
    .write = my_write,
    .release = my_close,
};


int my_platform_driver_probe(struct platform_device * my_platform_dev){
    int ret;
    //struct resource* res = NULL;
    printk("platform_driver_probe %d \n", __LINE__);
    printk("GPIO_DR: %x\n", my_platform_dev->resource[1].start);
    //res = platform_get_resource(my_platform_dev, IORESOURCE_IRQ, 0);
    //printk("iqr %ld\n", res->desc);

    my_led_device.my_dev = cdev_alloc();
    if(my_led_device.my_dev == NULL){
        printk("cdev_alloc fail \n");
        return -ENOMEM;
    }
    cdev_init(my_led_device.my_dev, &fps);
    ret = alloc_chrdev_region(&my_led_device.my_dev->dev, 0, 1, my_platform_dev->name);
    if(ret){
        printk("alloc_chrdev_region fail \n");
        cdev_del(my_led_device.my_dev);
        return ret;
    }
    ret = cdev_add(my_led_device.my_dev, my_led_device.my_dev->dev, 1);  
    if(ret){
        printk("cdev_add fail\n");
        register_chrdev_region(my_led_device.my_dev->dev, 1, my_platform_dev->name);
        return ret;
    }
    my_led_device.my_class = class_create(THIS_MODULE, "MYLED");
    my_led_device.my_device = device_create(my_led_device.my_class, NULL, my_led_device.my_dev->dev, NULL, DEV_NAME);
    my_led_device.my_imx6ull.IOMUXC_CLK_ENABLE = ioremap(my_platform_dev->resource[2].start, 4);
    *my_led_device.my_imx6ull.IOMUXC_CLK_ENABLE |= (0x3<<1);
    my_led_device.my_imx6ull.CCM_CCGR_CG = ioremap(my_platform_dev->resource[0].start, 4);
    *my_led_device.my_imx6ull.CCM_CCGR_CG |= (0x3<<30);
    my_led_device.my_imx6ull.IOMUXC_SNVS_SW = ioremap(my_platform_dev->resource[1].start, 4);
    *my_led_device.my_imx6ull.IOMUXC_SNVS_SW |= (0x5);
	*my_led_device.my_imx6ull.IOMUXC_SNVS_SW &= ~(0x1<<2);
    my_led_device.my_imx6ull.GPIO_GDIR = ioremap(my_platform_dev->resource[4].start, 4);
    *my_led_device.my_imx6ull.GPIO_GDIR |= (0x1<<3);
    my_led_device.my_imx6ull.GPIO_DR = ioremap(my_platform_dev->resource[3].start, 4);
    return 0;
}

int my_platform_driver_remove(struct platform_device *my_platform_dev){
    printk("platform_driver_remove %d \n", __LINE__);
    return 0;
}

struct of_device_id my_match_table[] = {
    [0] = {.compatible = "my_platform_device"},
    [1] = {},
};

struct platform_driver my_platform_driver = {
    .probe = my_platform_driver_probe,
    .remove = my_platform_driver_remove,
    .driver = {
        .name = DEV_NAME,
        .of_match_table = my_match_table,
    },
};

// 函数入口
static int __init my_sample_driver_init(void)
{
    int ret;
    printk("driver 入口函数\n");

    ret = platform_driver_register(&my_platform_driver);
    if(ret < 0){
        return -1;
    }
    return 0;
}
//函数出口
void __exit my_sample_driver_exit(void){
    printk("出口函数\n");
    device_destroy(my_led_device.my_class, my_led_device.my_dev->dev);
    class_destroy(my_led_device.my_class);
    unregister_chrdev_region(my_led_device.my_dev->dev, my_led_device.my_dev->count);
    cdev_del(my_led_device.my_dev);
    kfree(my_led_device.my_class);
    platform_driver_unregister(&my_platform_driver);
    printk("all is ok\n");

}
//许可
MODULE_LICENSE("GPL");
//注册函数
module_init(my_sample_driver_init);
//注销函数
module_exit(my_sample_driver_exit);