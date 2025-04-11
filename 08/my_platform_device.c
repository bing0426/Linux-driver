#include <linux/module.h>
#include <linux/init.h>
#include <linux/platform_device.h>


#define DEV_NAME	        "my_platform_device"
#define CCM_CCGR_CG_	    0x020c406C
#define IOMUXC_SNVS_SW_		0x02290014
#define IOMUXC_CLK_ENABLE_	0x020c4078
#define GPIO_DR_            0x020AC000
#define GPIO_GDIR_          0x020AC004

struct resource my_platform_device_resource[] = {
    [0] = {
        .start = 0x020c406C,
        .end = 0x020c406C + 0x3,
        .name = "CCM_CCGR_CG",
        .flags = IORESOURCE_MEM,
        .desc = CCM_CCGR_CG_,
    },
    [1] = {
        .start = 0x02290014,
        .end = 0x02290014 + 0x3,
        .name = "IOMUXC_SNVS_SW",
        .flags = IORESOURCE_MEM,
        .desc = IOMUXC_SNVS_SW_,
    },
    [2] = {
        .start = 0x020c4078,
        .end = 0x020c4078 + 0x3,
        .name = "IOMUXC_CLK_ENABLE",
        .flags = IORESOURCE_MEM,
        .desc = IOMUXC_CLK_ENABLE_,
    },
    [3] = {
        .start = 0x020AC000,
        .end = 0x020AC000 + 0x3,
        .name = "GPIO_DR",
        .flags = IORESOURCE_MEM,
        .desc = GPIO_DR_,
    },
    [4] = {
        .start = 0x020AC004,
        .end = 0x020AC004 + 0x3,
        .name = "GPIO_GDIR",
        .flags = IORESOURCE_MEM,
        .desc = GPIO_GDIR_,
    },
    [5] = {
        .start = 74,
        .end = 74,
        .flags = IORESOURCE_IRQ,
        .desc = 88
    }
};

void my_platform_device_release(struct device* dev){

}

struct platform_device my_platform_device = {
    .name = DEV_NAME,
    .id = -1, 
    .resource = my_platform_device_resource,
    .num_resources = 6,
    .dev = {
        .release = my_platform_device_release,
    }
};

// 函数入口
static int __init
my_sample_driver_init(void)
{
    int ret;
    printk("device 入口函数\n");
    ret = platform_device_register(&my_platform_device);
    if(ret < 0){
        return -1;
    }
    return 0;
}
//函数出口
void __exit my_sample_driver_exit(void){
    printk("出口函数\n");
    platform_device_unregister(&my_platform_device);
    printk("all is ok\n");

}
//许可
MODULE_LICENSE("GPL");
//注册函数
module_init(my_sample_driver_init);
//注销函数
module_exit(my_sample_driver_exit);