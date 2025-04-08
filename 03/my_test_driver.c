#include <linux/module.h>
#include <linux/init.h>
#include <linux/slab.h>
#include <linux/string.h>
#include <linux/vmalloc.h>


struct test_memery {
    char* kmalloc_memery;
    char* kzalloc_memery;
    char* vmalloc_memery;
};

struct test_memery my_test_memery;

static int __init my_test_driver_init(void){
    my_test_memery.kmalloc_memery = kmalloc(64, GFP_KERNEL);
    if(my_test_memery.kmalloc_memery == NULL){
        
        goto kmalloc_fail;
    }
    memset(my_test_memery.kmalloc_memery, 0, 64);
    strcpy(my_test_memery.kmalloc_memery, "Hello kmalloc");
    printk("%s %d \n",my_test_memery.kmalloc_memery, __LINE__);

    my_test_memery.kzalloc_memery = kzalloc(64, GFP_KERNEL);
    if(my_test_memery.kzalloc_memery == NULL){

        goto kzalloc_fail;
    }
    strcpy(my_test_memery.kzalloc_memery, "Hello kzalloc");
    printk("%s %d \n",my_test_memery.kzalloc_memery, __LINE__);

    my_test_memery.vmalloc_memery = vmalloc(64);
    if(my_test_memery.vmalloc_memery == NULL){
        goto vmalloc_fail;
    }
    memset(my_test_memery.vmalloc_memery, 0, 64);
    strcpy(my_test_memery.vmalloc_memery, "Hello vmalloc");
    printk("%s %d \n",my_test_memery.vmalloc_memery, __LINE__);

    return 0;

vmalloc_fail:
    printk("vmalloc err %d\n",__LINE__);
    kfree(my_test_memery.kzalloc_memery);
kzalloc_fail:
    printk("kzalloc err %d\n",__LINE__);
    kfree(my_test_memery.kmalloc_memery);
kmalloc_fail:
    printk("kmalloc err %d\n",__LINE__);
    return -ENOMEM;
}
void __exit my_test_driver_exit(void){
    kfree(my_test_memery.kmalloc_memery);
    kfree(my_test_memery.kzalloc_memery);
    vfree(my_test_memery.vmalloc_memery);
    printk("%d\n",__LINE__);
}

MODULE_LICENSE("GPL");
module_init(my_test_driver_init);
module_exit(my_test_driver_exit);