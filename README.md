# Custom Linux Kernel for tiny-gpu (NixOS)

This directory scaffolds a reproducible Linux kernel build (linux-tkg style) for
developing and testing the fused RISC-V CPU-GPU stack. It does not by itself
implement GPU branch divergence or memory coalescing — those are RTL features
in `src/`. What this gives you is a tuned kernel + driver development base.

## Contents
- `tiny-gpu-kernel.nix` — NixOS module overriding `boot.kernelPackages`.
- `flake.nix` — standalone flake for building the kernel package.
- `patches/` — drop `.patch` files here and list them in the module.

## Usage (NixOS)
Import the module in your `configuration.nix`:

```nix
imports = [ /home/palontologist/Downloads/dev/tiny-gpu/kernel/tiny-gpu-kernel.nix ];
```

Then `sudo nixos-rebuild switch`.

## Notes
- Building from source takes time and disk; consider `nix build` on a fast machine.
- For driver work, prefer `sched_ext` (scx) and a out-of-tree module via `boot.extraModulePackages`.

## Bandwidth-aware driver proposal
- Design: [docs/bandwidth-mitigation.md](docs/bandwidth-mitigation.md)
- Driver scaffold: [driver/](driver/) (out-of-tree char device stub, UAPI, QUERY_CAPS)

This targets effective bandwidth utilization on the Artix-7 + DDR3L tiny-gpu hardware (contiguous/aligned buffers, batched DMA, async submit/wait, capability-driven runtime). Physical DDR bandwidth is a hardware constraint; RTL coalescing/L2 and a stable FPGA transport are required for full benefit.
