#include <linux/module.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__attribute__((section(".gnu.linkonce.this_module"))) = {
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
__used
__attribute__((section("__versions"))) = {
	{ 0xa0a7cae5, __VMLINUX_SYMBOL_STR(module_layout) },
	{ 0x756a244b, __VMLINUX_SYMBOL_STR(platform_driver_unregister) },
	{ 0xf16401c6, __VMLINUX_SYMBOL_STR(__platform_driver_register) },
	{ 0xfa2a45e, __VMLINUX_SYMBOL_STR(__memzero) },
	{ 0x28cc25db, __VMLINUX_SYMBOL_STR(arm_copy_from_user) },
	{ 0x89df42aa, __VMLINUX_SYMBOL_STR(gpiod_set_raw_value) },
	{ 0x68ba5a9, __VMLINUX_SYMBOL_STR(cdev_add) },
	{ 0xb08f9e48, __VMLINUX_SYMBOL_STR(cdev_init) },
	{ 0xa08b248e, __VMLINUX_SYMBOL_STR(device_create) },
	{ 0x1327b810, __VMLINUX_SYMBOL_STR(__class_create) },
	{ 0x29537c9e, __VMLINUX_SYMBOL_STR(alloc_chrdev_region) },
	{ 0xe18142ff, __VMLINUX_SYMBOL_STR(of_get_named_gpio_flags) },
	{ 0x915cb96, __VMLINUX_SYMBOL_STR(kmem_cache_alloc_trace) },
	{ 0x7dbef969, __VMLINUX_SYMBOL_STR(kmalloc_caches) },
	{ 0x7485e15e, __VMLINUX_SYMBOL_STR(unregister_chrdev_region) },
	{ 0x37af6c70, __VMLINUX_SYMBOL_STR(class_destroy) },
	{ 0x52c7f37c, __VMLINUX_SYMBOL_STR(device_destroy) },
	{ 0x71873fdc, __VMLINUX_SYMBOL_STR(cdev_del) },
	{ 0xc1514a3b, __VMLINUX_SYMBOL_STR(free_irq) },
	{ 0xfe990052, __VMLINUX_SYMBOL_STR(gpio_free) },
	{ 0xd6b8e852, __VMLINUX_SYMBOL_STR(request_threaded_irq) },
	{ 0xf273bfc7, __VMLINUX_SYMBOL_STR(gpiod_set_debounce) },
	{ 0x3d0ddb5d, __VMLINUX_SYMBOL_STR(gpiod_to_irq) },
	{ 0x2e7c4960, __VMLINUX_SYMBOL_STR(gpiod_direction_input) },
	{ 0xba7fa7c1, __VMLINUX_SYMBOL_STR(gpiod_direction_output_raw) },
	{ 0x47229b5c, __VMLINUX_SYMBOL_STR(gpio_request) },
	{ 0xdb7305a1, __VMLINUX_SYMBOL_STR(__stack_chk_fail) },
	{ 0xf4fa543b, __VMLINUX_SYMBOL_STR(arm_copy_to_user) },
	{ 0x8d232dcf, __VMLINUX_SYMBOL_STR(gpiod_get_raw_value) },
	{ 0x53aa5f51, __VMLINUX_SYMBOL_STR(gpio_to_desc) },
	{ 0x8f678b07, __VMLINUX_SYMBOL_STR(__stack_chk_guard) },
	{ 0x2e5810c6, __VMLINUX_SYMBOL_STR(__aeabi_unwind_cpp_pr1) },
	{ 0x27e1a049, __VMLINUX_SYMBOL_STR(printk) },
	{ 0xb1ad28e0, __VMLINUX_SYMBOL_STR(__gnu_mcount_nc) },
};

static const char __module_depends[]
__used
__attribute__((section(".modinfo"))) =
"depends=";


MODULE_INFO(srcversion, "47CFB006F9F1ED4F641A212");
