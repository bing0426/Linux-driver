#include <linux/init.h>
#include <linux/module.h>
#include <linux/kernel.h>
#include <linux/init.h>
#include <linux/module.h>
#include <linux/slab.h>
#include <linux/delay.h>
#include <linux/mutex.h>
#include <linux/mod_devicetable.h>
#include <linux/log2.h>
#include <linux/bitops.h>
#include <linux/jiffies.h>
#include <linux/of.h>
#include <linux/acpi.h>
#include <linux/i2c.h>

static const struct of_device_id i2c_sample_match[] = {
	{ .compatible = "ahis,simple_device" },
	{/* END OF LIST */ },
};
static const struct i2c_device_id i2c_sample_ids[] = {
	{ "chip_name", (__kernel_ulong_t)NULL },
	{ /* END OF LIST */ }
};

static int i2c_sample_probe(struct i2c_client *client, const struct i2c_device_id *id){
    return 0;
}

static int i2c_sample_remove(struct i2c_client *client){
    return 0;
}
static struct i2c_driver i2c_sample_driver = {
	.driver = {
		.name = "i2c_sample",
		.of_match_table = i2c_sample_match,
	},
	.probe = i2c_sample_probe,
	.remove = i2c_sample_remove,
	.id_table = i2c_sample_ids,
};

static int __init i2c_sample_driver_init(void)
{
    i2c_add_driver(&i2c_sample_driver);
	return 0;
}
static void __exit i2c_sample_driver_exit(void)
{
    i2c_del_driver(&i2c_sample_driver);
}
module_init(i2c_sample_driver_init);
module_exit(i2c_sample_driver_exit);

MODULE_AUTHOR("ahis");
MODULE_LICENSE("GPL");
