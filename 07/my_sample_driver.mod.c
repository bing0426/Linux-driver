#include <linux/build-salt.h>
#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(.gnu.linkonce.this_module) = {
	.name = KBUILD_MODNAME,
	.init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
	.exit = cleanup_module,
#endif
	.arch = MODULE_ARCH_INIT,
};

#ifdef CONFIG_RETPOLINE
MODULE_INFO(retpoline, "Y");
#endif

static const struct modversion_info ____versions[]
__used __section(__versions) = {
	{ 0xdd8f8694, "module_layout" },
	{ 0x37a0cba, "kfree" },
	{ 0x6091b333, "unregister_chrdev_region" },
	{ 0xb65e5a32, "class_destroy" },
	{ 0x22e92418, "device_destroy" },
	{ 0xd9a5ea54, "__init_waitqueue_head" },
	{ 0x7749276a, "device_create" },
	{ 0x2871e975, "__class_create" },
	{ 0xc4952f09, "cdev_add" },
	{ 0x22b90774, "cdev_del" },
	{ 0xe3ec2f2b, "alloc_chrdev_region" },
	{ 0x2064fa56, "cdev_init" },
	{ 0x4ddb27b7, "cdev_alloc" },
	{ 0xdecd0b29, "__stack_chk_fail" },
	{ 0x92540fbf, "finish_wait" },
	{ 0x8c26d495, "prepare_to_wait_event" },
	{ 0x1000e51, "schedule" },
	{ 0xfe487975, "init_wait_entry" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0xb44ad4b3, "_copy_to_user" },
	{ 0x56b1771b, "current_task" },
	{ 0x5ab904eb, "pv_ops" },
	{ 0xdbf17652, "_raw_spin_lock" },
	{ 0xbad25a6a, "fasync_helper" },
	{ 0x9793b172, "kill_fasync" },
	{ 0x3eeb2322, "__wake_up" },
	{ 0x362ef408, "_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0xc5850110, "printk" },
	{ 0x2ea2c95c, "__x86_indirect_thunk_rax" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "4B7789F2F5EAE91B9AC003D");
