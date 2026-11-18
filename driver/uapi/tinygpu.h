/* SPDX-License-Identifier: GPL-2.0 WITH Linux-syscall-note */
#ifndef _UAPI_TINYGPU_H_
#define _UAPI_TINYGPU_H_

#include <linux/types.h>
#include <linux/ioctl.h>

#define TINYGPU_IOCTL_MAGIC 'T'

struct tinygpu_caps {
	__u32 version;
	__u32 num_cores;
	__u32 threads_per_block;
	__u32 vector_width_bits;
	__u64 mem_bytes;
	__u32 align_bytes;
	__u32 features; /* bitmask: INT4_DP4A, VEC_SIMD, etc. */
};

#define TINYGPU_IOC_QUERY_CAPS _IOR(TINYGPU_IOCTL_MAGIC, 0x00, struct tinygpu_caps)
/* Future:
 * TINYGPU_IOC_BO_CREATE / BO_MAP / BO_DESTROY
 * TINYGPU_IOC_SUBMIT / TINYGPU_IOC_WAIT
 */

#endif /* _UAPI_TINYGPU_H_ */
