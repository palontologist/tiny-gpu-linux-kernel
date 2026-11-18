// SPDX-License-Identifier: GPL-2.0
/*
 * tinygpu - bandwidth-aware host driver scaffold for tiny-gpu FPGA (Artix-7 + DDR3L)
 *
 * Phase 0 stub: char device + QUERY_CAPS placeholder.
 * See docs/bandwidth-mitigation.md for the full proposal.
 */

#include <linux/module.h>
#include <linux/miscdevice.h>
#include <linux/fs.h>
#include <linux/uaccess.h>

#include "uapi/tinygpu.h"

#define DRV_NAME "tinygpu"

static long tinygpu_ioctl(struct file *filp, unsigned int cmd, unsigned long arg)
{
	struct tinygpu_caps caps = {};

	switch (cmd) {
	case TINYGPU_IOC_QUERY_CAPS:
		caps.version = 0;
		caps.num_cores = 2;
		caps.threads_per_block = 4;
		caps.vector_width_bits = 128;
		caps.mem_bytes = 256; /* placeholder: word rows in sim */
		caps.align_bytes = 64;
		if (copy_to_user((void __user *)arg, &caps, sizeof(caps)))
			return -EFAULT;
		return 0;
	default:
		return -ENOTTY;
	}
}

static const struct file_operations tinygpu_fops = {
	.owner          = THIS_MODULE,
	.unlocked_ioctl = tinygpu_ioctl,
#ifdef CONFIG_COMPAT
	.compat_ioctl   = tinygpu_ioctl,
#endif
};

static struct miscdevice tinygpu_misc = {
	.minor = MISC_DYNAMIC_MINOR,
	.name  = DRV_NAME,
	.fops  = &tinygpu_fops,
};

static int __init tinygpu_init(void)
{
	return misc_register(&tinygpu_misc);
}

static void __exit tinygpu_exit(void)
{
	misc_deregister(&tinygpu_misc);
}

module_init(tinygpu_init);
module_exit(tinygpu_exit);

MODULE_AUTHOR("George Karani");
MODULE_DESCRIPTION("tinygpu FPGA accelerator driver (scaffold)");
MODULE_LICENSE("GPL");
