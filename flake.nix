{
  description = "tiny-gpu custom Linux kernel (Nix)";
  inputs.nixpkgs.url = "github:NixOS/nixpkgs/nixos-unstable";
  outputs = { self, nixpkgs }:
    let
      system = "x86_64-linux";
      pkgs = nixpkgs.legacyPackages.${system};
      kernel = pkgs.linux_latest.override {
        kernelPatches = [ ];
        extraConfig = ''
          SCHED_AUTOGROUP y
          PREEMPT_VOLUNTARY y
          DEBUG_INFO y
        '';
      };
    in
    {
      packages.${system}.linux-tinygpu = kernel;
      packages.${system}.default = kernel;
    };
}
