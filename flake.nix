{
  inputs = {
    nixpkgs.url = "nixpkgs/nixos-unstable";
    flake-utils.url = "github:numtide/flake-utils";
  };
  outputs = { self, nixpkgs, flake-utils }:
    (flake-utils.lib.eachDefaultSystem
      (system:
        let 
        pkgs = import nixpkgs { 
        inherit system; 
        config.allowUnfree = true;
      };
      in
      {
        packages = rec {
          compile = pkgs.writeShellScriptBin "compile" ''
            NIX_ENFORCE_NO_NATIVE=0 g++ display.cpp beg.cpp -o beg -O3 -std=c++23 -march=native -ffast-math -fopenmp
          '';
          default = compile;
        };

        devShells.default = pkgs.mkShell {
          buildInputs = with pkgs; [
            gcc
            gdb
            hyperfine
          ];
        };
      })
    );
}
