#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/fs.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/of.h>
#include <linux/acpi.h>
#include <linux/i2c.h>
#include <asm/uaccess.h>

int major = 0;
static struct class *ap3216_class;
static struct device *ap3216_device;
static struct i2c_client *ap3216_client;

static const struct of_device_id i2c_ap3216_match[] = {
	{ .compatible = "ahis,simple_device" },
	{/* END OF LIST */ },
};
static const struct i2c_device_id i2c_ap3216_ids[] = {
	{ "ap3216", (__kernel_ulong_t)NULL },
	{ /* END OF LIST */ }
};

int ap3216_open(struct inode *inode, struct file *file){
	i2c_smbus_write_byte_data(ap3216_client, 0, 0x04);
	mdelay(20);
	i2c_smbus_write_byte_data(ap3216_client, 0, 0x03);
	return 0;
}

ssize_t ap3216_read(struct file *file, char __user *usrbuf, size_t size, loff_t *offset){
	char kernel_buf[6] = {0};
	u16 ret;
	int err;
	//红外
	ret = i2c_smbus_read_word_data(ap3216_client, 0xa);
	kernel_buf[0] = ret & 0xff;
	kernel_buf[1] = (ret>>8) & 0xff;
	//光强
	ret = i2c_smbus_read_word_data(ap3216_client, 0xc);
	kernel_buf[2] = ret & 0xff;
	kernel_buf[3] = (ret>>8) & 0xff;
	//距离
	ret = i2c_smbus_read_word_data(ap3216_client, 0xe);
	kernel_buf[4] = ret & 0xff;
	kernel_buf[5] = (ret>>8) & 0xff;
	err = copy_from_user(usrbuf, kernel_buf, min(size, sizeof(kernel_buf)));
	return min(size, sizeof(kernel_buf));
}

static struct file_operations ap3216_ops = {
	.owner = THIS_MODULE,
	.open = ap3216_open,
	.read = ap3216_read,
};

static int i2c_ap3216_probe(struct i2c_client *client, const struct i2c_device_id *id){
	printk("%s %d", __FUNCTION__, __LINE__);
	ap3216_client = client;
	major = register_chrdev(major, "ap3216", &ap3216_ops);
	ap3216_class = class_create(THIS_MODULE, "ap3216_class");
	ap3216_device = device_create(ap3216_class, NULL, MKDEV(major, 0), NULL, "ap3216_device");
    return 0;
}

static int i2c_ap3216_remove(struct i2c_client *client){
	device_destroy(ap3216_class, MKDEV(major, 0));
	class_register(ap3216_class);
	unregister_chrdev(major, "ap3216");
    return 0;
}
static struct i2c_driver i2c_ap3216_driver = {
	.driver = {
		.name = "i2c_ap3216",
		.of_match_table = i2c_ap3216_match,
	},
	.probe = i2c_ap3216_probe,
	.remove = i2c_ap3216_remove,
	.id_table = i2c_ap3216_ids,
};

static int __init i2c_ap3216_driver_init(void)
{
    i2c_add_driver(&i2c_ap3216_driver);
	return 0;
}
static void __exit i2c_ap3216_driver_exit(void)
{
    i2c_del_driver(&i2c_ap3216_driver);
}
module_init(i2c_ap3216_driver_init);
module_exit(i2c_ap3216_driver_exit);

MODULE_AUTHOR("ahis");
MODULE_LICENSE("GPL");
