# Bandwidth-Aware Driver Proposal for tiny-gpu (Artix-7 + DDR3L)

## Context
- **Target hardware:** tiny-gpu on AMD Xilinx Artix-7 (XC7A100T) with DDR3L x16 (e.g. MT41K256M16TW-107). Peak theoretical bandwidth is ~2–4 GB/s; effective bandwidth is lower.
- **Unified design:** CPU and GPU/vector engines share the same memory pool, so all instruction fetch, scalar/vector data, and DMA contend for one DDR interface.
- **Current RTL gaps (high impact):** no memory coalescing, no shared L2, limited scheduling—making DDR efficiency critical.

> **Note:** Software cannot increase physical DDR bandwidth. This proposal focuses on improving *effective* bandwidth utilization and reducing unnecessary traffic.

## Goals
1. Maximize DDR burst efficiency (aligned, contiguous, coalescable accesses).
2. Minimize copies and host↔device transfer overhead.
3. Enable tiling/double-buffering and submission patterns that match hardware capabilities.
4. Provide a stable userspace ABI for the tiny-gpu runtime (`tinygpu` in the tiny-gpu repo).

## Proposed Linux-side design (out-of-tree driver/module)

### 1. Buffer allocation & memory management
- Allocate **physically contiguous, DMA-aligned** buffers (e.g. CMA/dma_alloc_coherent or contiguous helpers) sized/aligned to burst and cache-line boundaries.
- Expose **buffer object API**: create/destroy, map to userspace, export/import (dma-buf/fd) for zero-copy sharing where possible.
- Track **placement hints** (host-visible, device-local/on-FPGA BRAM-mapped regions if exposed) so the runtime can keep hot data on-chip.

### 2. DMA & transfer scheduling
- **Batch** small transfers into larger DMA transactions; avoid scatter/gather patterns that defeat bursts.
- Support **async DMA** with completion fences/events so compute and transfer can overlap (double/triple buffering).
- Enforce **alignment/size constraints** reported by device capabilities (burst width, max transfer).

### 3. Kernel submission & execution model
- **Command queue** for kernel launches with explicit dependencies/fences.
- **Descriptor packing:** submit program + args + buffer handles in one ioctl/queue entry to reduce syscall/doorbell overhead.
- **Capability query ioctl:** return cores, threads/block, vector width, memory sizes, alignment requirements, and supported ops (INT8/INT4 DP4A, vector SIMD) so the runtime is capability-driven.

### 4. Bandwidth-aware policies (software-side)
- **Tiling hints:** allow userspace/runtime to declare tile sizes and access patterns; driver can schedule transfers in tile order.
- **Prefetch/read-ahead** of next tiles when bus is idle (if device supports).
- **Throttle/fairness:** simple rate limiting or round-robin across queues to prevent starvation in unified CPU+GPU scenarios.
- **Stats/counters:** expose DMA bytes, burst efficiency (if hardware reports), queue latency via debugfs/sysfs for tuning.

### 5. UAPI sketch (illustrative)
- `TINYGPU_IOC_QUERY_CAPS` → caps (memory, alignment, cores/threads, vector, features)
- `TINYGPU_IOC_BO_CREATE` / `BO_MAP` / `BO_DESTROY`
- `TINYGPU_IOC_SUBMIT` → {kernels[], dmAs[], fences[]} (async)
- `TINYGPU_IOC_WAIT` → fence/completion

(Exact structs/ABI to be finalized with the RTL/FPGA register and transport interface.)

## Integration with tiny-gpu runtime
- The `tinygpu` Python/runtime scaffold in `tiny-gpu` should add an **FPGA/driver backend** implementing the same `Device` interface by calling this UAPI.
- Simulator backend remains for development without hardware.

## Phased implementation
1. **Phase 0:** Stub char device + QUERY_CAPS + contiguous BO alloc/map; manual MMIO/PIO for bring-up.
2. **Phase 1:** DMA engine integration and async submit/wait matching FPGA transport.
3. **Phase 2:** Double-buffering helpers, stats, and bandwidth-aware scheduling hooks.
4. **Phase 3:** Align with RTL coalescing/L2 features as they land; expose hardware perf counters.

## Hardware dependencies / co-design
- RTL: memory coalescing, shared L2/cache policy, burst-friendly LSU behavior, and (optionally) perf counters.
- FPGA: stable host transport (e.g. PCIe/XDMA, USB, or UART+DMA bridge) and documented register map.
- Without coalescing in hardware, driver-side alignment/batching helps but has limited effect—treat as co-design.
