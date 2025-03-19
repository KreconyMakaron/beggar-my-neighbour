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
          packages = {
            compile = pkgs.writeShellScriptBin "compile" ''
                NIX_ENFORCE_NO_NATIVE=0 g++ $1.cpp -o $1 -O3 -std=c++23 -march=native -ffast-math -fopenmp
            '';
            compile_and_run = pkgs.writeShellScriptBin "compile_and_run" ''


            '';
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
