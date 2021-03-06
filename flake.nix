{
  description = "Wayland signal/listener wrapper for C++";

  inputs = { nixpkgs.url = "github:NixOS/nixpkgs"; };

  outputs = { self, nixpkgs }:
    let
      allSystems = [ "x86_64-linux" "i686-linux" "aarch64-linux" ];
      forAllSystems = f: nixpkgs.lib.genAttrs allSystems (system: f system);
      nixpkgsFor = forAllSystems (system:
        import nixpkgs {
          inherit system;
          overlays = [ self.overlay ];
        });

    in {
      overlay = final: prev: rec {
        waysig = with final;
          stdenv.mkDerivation rec {
            name = "waysig-${version}";
            version = "0.3.2";

            src = ./.;

            nativeBuildInputs = [ cmake catch2 pkg-config wayland ];

            doCheck = true;
            checkTarget = "test";

            cmakeFlags =
              [ "-DWAYSIG_BUILD_TESTS=ON" "-DWAYSIG_BUILD_TESTS_WAYLAND=ON" ];

            meta = with stdenv.lib; {
              homepage = "https://github.com/dreyri/waysig";
              description = "Wayland signal/listener wrapper for C++";
              license = licenses.mit;
            };
          };
      };

      packages =
        forAllSystems (system: { inherit (nixpkgsFor.${system}) waysig; });

      defaultPackage = forAllSystems (system: self.packages.${system}.waysig);

      devShell = forAllSystems (system:
        let pkgs = import nixpkgs { inherit system; };
        in with pkgs;
        mkShell {
          buildInputs =
            [ pkgconfig ccls cmake wayland extra-cmake-modules catch2 ];

          shellHook = ''
            cat << EOF > .ccls
            %compile_commands.json
            %cpp -std=c++14
            -Iinclude
            -I${pkgs.wayland}/include 
            EOF
          '';
        });
    };
}
