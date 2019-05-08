{
  nixpkgs ? <nixpkgs>,
  pkgs ? import nixpkgs {}
}:

with pkgs;

let
in stdenv.mkDerivation {
  name = "server";
  src = ./.;

  nativeBuildInputs = [ cmake binutils ];

  installPhase = ''
    mkdir -p $out/bin
    cp server $out/bin/server
  '';
}
