#include <linux/module.h>
#define INCLUDE_VERMAGIC
#include <linux/build-salt.h>
#include <linux/elfnote-lto.h>
#include <linux/vermagic.h>
#include <linux/compiler.h>

BUILD_SALT;
BUILD_LTO_INFO;

MODULE_INFO(vermagic, VERMAGIC_STRING);
MODULE_INFO(name, KBUILD_MODNAME);

__visible struct module __this_module
__section(".gnu.linkonce.this_module") = {
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
__used __section("__versions") = {
	{ 0xd9726f80, "module_layout" },
	{ 0x3da95b0c, "debugfs_remove" },
	{ 0xb3b1be2f, "debugfs_create_file" },
	{ 0xc5850110, "printk" },
	{ 0xbca5adcc, "debugfs_create_dir" },
	{ 0xcbd4898c, "fortify_panic" },
	{ 0xc959d152, "__stack_chk_fail" },
	{ 0x56470118, "__warn_printk" },
	{ 0xa916b694, "strnlen" },
	{ 0x656e4a6e, "snprintf" },
	{ 0x53377cb8, "pid_task" },
	{ 0x9592c55e, "find_get_pid" },
	{ 0xbcab6ee6, "sscanf" },
	{ 0x13c49cc2, "_copy_from_user" },
	{ 0x88db9f48, "__check_object_size" },
	{ 0x619cb7dd, "simple_read_from_buffer" },
	{ 0xbdfb6dbb, "__fentry__" },
};

MODULE_INFO(depends, "");


MODULE_INFO(srcversion, "4CA195CF06B47BC2EE4438F");
