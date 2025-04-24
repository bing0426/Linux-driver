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
#include <linux/gpio.h>
#include <linux/of.h>
#include <linux/of_gpio.h>
#include <asm-generic/gpio.h>
#include <linux/of_platform.h>
#include <linux/of_irq.h>
#include <linux/interrupt.h>
#include <linux/jiffies.h>
#include <linux/delay.h>
#include <linux/timer.h>
#include <linux/string.h>


#define DEV_NAME	        "my_platform_device"

char kernel_buf[128] = {0};    

struct my_platform_device
{   
    struct cdev* my_dev;
    struct class* my_class;
    struct device* my_device;
    int gpio_num;
};

struct mykey {
    u32 key1_irq;
    u32 key2_irq;
    u32  volatile irq;
};

struct my_platform_device my_led_device = {0};
struct mykey my_key = {0}; 
struct timer_list my_timer;



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
        gpio_set_value(my_led_device.gpio_num, 1);
    }
    if(kernel_buf[0] == '0'){
        gpio_set_value(my_led_device.gpio_num, 0);
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

static void timer_callback(unsigned long data){
    if(my_key.irq == my_key.key1_irq){
        gpio_set_value(my_led_device.gpio_num,  0);
    }
    else{
        gpio_set_value(my_led_device.gpio_num,  1);
    }
    return ;
}

irqreturn_t mykey_irq_handler_t(int irq, void *dev){
    printk("%s %d\n",__FUNCTION__, __LINE__);
    my_key.irq = irq;
    mod_timer(&my_timer, my_timer.expires);
    return IRQ_HANDLED;
}

void timer_init(void){
    init_timer(&my_timer);
    my_timer.expires = HZ * 10 + jiffies;
    //timer_setup(&my_timer, timer_callback, 0);
    my_timer.function = timer_callback;
    my_timer.flags = 0;
    add_timer(&my_timer);
}

int my_platform_driver_probe(struct platform_device * my_platform_dev){
    int ret;
    struct device_node* key_nod = NULL;
    struct device_node* led_nod = NULL;
    struct device_node *child = NULL; 
    printk("platform_driver_probe %d \n", __LINE__);
    //printk("GPIO_DR: %x\n", my_platform_dev->resource[1].start);
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

    //gpio相关操作
    led_nod = of_find_node_by_name(my_platform_dev->dev.of_node, "cpu");
    if(!led_nod){
        printk("led_nod fail %d \n",__LINE__);
        return -1;
    }
    my_led_device.gpio_num = of_get_named_gpio(led_nod, "gpios", 0);
    printk("led_num: %d LINE: %d\n",my_led_device.gpio_num, __LINE__);
    ret = gpio_request(my_led_device.gpio_num, "gpio1");
    if(ret){
        printk("gpio_request fail\n");
        return ret;
    }
    gpio_direction_output(my_led_device.gpio_num, 0);

    //key相关操作
    key_nod = of_find_node_by_path("/my-keys");
    if(key_nod == NULL){
        printk("of_find_node_by_path fail \n");
    }
    for_each_child_of_node(key_nod, child){
        int irq = 0;
        const char* name = of_node_full_name(child);
        printk("%s %d", name, __LINE__);
        irq = of_irq_get(child, 0);
        if(irq < 0){
            printk("of_irq_get fail %d\n", __LINE__);
            return irq;
        }
        if(strcmp(name, "/my-keys/user1") == 0){
            my_key.key1_irq = irq;
            printk("%d %d",my_key.key1_irq, __LINE__);
        }
        if(strcmp(name, "/my-keys/user2") == 0){
            my_key.key2_irq = irq;
            printk("%d %d",my_key.key2_irq, __LINE__);
        }
    }
    ret = request_irq(my_key.key1_irq, mykey_irq_handler_t, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "my_key1_irq", NULL);
    if(ret){
        printk("request_irq fial %d \n",__LINE__);
        return ret;
    }
    ret = request_irq(my_key.key2_irq, mykey_irq_handler_t, IRQF_TRIGGER_RISING | IRQF_TRIGGER_FALLING, "my_key2_irq", NULL);
    if(ret){
        printk("request_irq fial %d \n",__LINE__);
        return ret;
    }
    timer_init();
    return 0;
}

int my_platform_driver_remove(struct platform_device *my_platform_dev){
    printk("platform_driver_remove %d \n", __LINE__);
    del_timer(&my_timer);
    free_irq(my_key.key1_irq, NULL);
    free_irq(my_key.key2_irq, NULL);
    gpio_free(my_led_device.gpio_num);
    device_destroy(my_led_device.my_class, my_led_device.my_dev->dev);
    class_destroy(my_led_device.my_class);
    unregister_chrdev_region(my_led_device.my_dev->dev, my_led_device.my_dev->count);
    cdev_del(my_led_device.my_dev);
    kfree(my_led_device.my_class);
    return 0;
}

struct of_device_id my_match_table[] = {
    [0] = {.compatible = "ahis,gpio-leds"},
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
    platform_driver_unregister(&my_platform_driver);
    printk("all is ok\n");

}
//许可
MODULE_LICENSE("GPL");
//注册函数
module_init(my_sample_driver_init);
//注销函数
module_exit(my_sample_driver_exit);