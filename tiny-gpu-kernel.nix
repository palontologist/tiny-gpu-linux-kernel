# NixOS module: custom Linux kernel for tiny-gpu development.
# Import this from configuration.nix:
#   imports = [ /path/to/tiny-gpu/kernel/tiny-gpu-kernel.nix ];
{ pkgs, ... }:

let
  tinyGpuKernel = pkgs.linux_latest.override {
    # Add patch files here, e.g.:
    # kernelPatches = [
    #   { name = "tiny-gpu-sched"; patch = ./patches/0001-sched.patch; }
    # ];
    kernelPatches = [ ];

    # Tuning knobs (examples). Adjust to your hardware.
    extraConfig = ''
      # General performance
      SCHED_AUTOGROUP y
      PREEMPT_VOLUNTARY y
      # Help driver development
      DEBUG_INFO y
      # RISC-V / vector (host build is x86_64; target board is separate)
    '';
  };
in
{
  boot.kernelPackages = pkgs.linuxPackagesFor tinyGpuKernel;

  # Example: out-of-tree module placeholder for a tiny-gpu driver.
  # boot.extraModulePackages = [ (pkgs.callPackage ./modules/tiny-gpu-driver.nix { kernel = tinyGpuKernel; }) ];
}
