#include <linux/module.h>
#include <linux/init.h>

    //函数入口
int __init my_test_driver_init(void){
    printk("函数注册\n");
    return 0;
}
    //出口函数
void __exit my_test_driver_exit(void){
    printk("驱动卸载\n");
}

    //许可
MODULE_LICENSE("GPL");
module_init(my_test_driver_init);
module_exit(my_test_driver_exit);


    //函数注册
