#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);

struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
 .name = KBUILD_MODNAME,
 .init = init_module,
#ifdef CONFIG_MODULE_UNLOAD
 .exit = cleanup_module,
#endif
 .arch = MODULE_ARCH_INIT,
};

static const struct modversion_info ____versions[]
__used
__attribute__((section("__versions"))) = {
	{ 0x14522340, "module_layout" },
	{ 0x6bc3fbc0, "__unregister_chrdev" },
	{ 0x1fedf0f4, "__request_region" },
	{ 0x4f1939c7, "per_cpu__current_task" },
	{ 0xc917223d, "pci_bus_read_config_byte" },
	{ 0xf9a482f9, "msleep" },
	{ 0xd6ee688f, "vmalloc" },
	{ 0x69a358a6, "iomem_resource" },
	{ 0xc8b57c27, "autoremove_wake_function" },
	{ 0xd691cba2, "malloc_sizes" },
	{ 0x973873ab, "_spin_lock" },
	{ 0x1a6d6e4f, "remove_proc_entry" },
	{ 0x7edc1537, "device_destroy" },
	{ 0xfa0d49c7, "__register_chrdev" },
	{ 0xd3364703, "x86_dma_fallback_dev" },
	{ 0xeae3dfd6, "__const_udelay" },
	{ 0x999e8297, "vfree" },
	{ 0x3c2c5af5, "sprintf" },
	{ 0xffc7c184, "__init_waitqueue_head" },
	{ 0x9629486a, "per_cpu__cpu_number" },
	{ 0xaf559063, "pci_set_master" },
	{ 0x400d591, "pci_get_subsys" },
	{ 0xea147363, "printk" },
	{ 0xecde1418, "_spin_lock_irq" },
	{ 0xacdeb154, "__tracepoint_module_get" },
	{ 0xa1c76e0a, "_cond_resched" },
	{ 0x85f8a266, "copy_to_user" },
	{ 0xb4390f9a, "mcount" },
	{ 0x16305289, "warn_slowpath_null" },
	{ 0x748caf40, "down" },
	{ 0x2d2cf7d, "device_create" },
	{ 0x859c6dc7, "request_threaded_irq" },
	{ 0xcbd81171, "module_put" },
	{ 0x78764f4e, "pv_irq_ops" },
	{ 0x92103a79, "pci_disable_link_state" },
	{ 0xb2fd5ceb, "__put_user_4" },
	{ 0x42c8de35, "ioremap_nocache" },
	{ 0x84b453e6, "pci_bus_read_config_word" },
	{ 0xc5aa6d66, "pci_bus_read_config_dword" },
	{ 0xf0fdf6cb, "__stack_chk_fail" },
	{ 0x3bd1b1f6, "msecs_to_jiffies" },
	{ 0xd62c833f, "schedule_timeout" },
	{ 0x6d6b15ff, "create_proc_entry" },
	{ 0x7c61340c, "__release_region" },
	{ 0x2044fa9e, "kmem_cache_alloc_trace" },
	{ 0x32047ad5, "__per_cpu_offset" },
	{ 0x642e54ac, "__wake_up" },
	{ 0x4d7d27b8, "pci_bus_write_config_byte" },
	{ 0x37a0cba, "kfree" },
	{ 0x236c8c64, "memcpy" },
	{ 0x33d92f9a, "prepare_to_wait" },
	{ 0xedc03953, "iounmap" },
	{ 0x3f1899f1, "up" },
	{ 0xe06bb002, "class_destroy" },
	{ 0x9ccb2622, "finish_wait" },
	{ 0xa12add91, "pci_enable_device" },
	{ 0xa2654165, "__class_create" },
	{ 0x3302b500, "copy_from_user" },
	{ 0x6e9681d2, "dma_ops" },
	{ 0xf20dabd8, "free_irq" },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "CFB63CAACA34B90E9AB558C");

static const struct rheldata _rheldata __used
__attribute__((section(".rheldata"))) = {
	.rhel_major = 6,
	.rhel_minor = 6,
};
