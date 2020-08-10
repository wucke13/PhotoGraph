with import <nixpkgs> {};

let 
  ImageGraph = import ../ImageGraph/default.nix;
  QNodeGraph = import ../QNodeGraph/default.nix;
in (overrideCC stdenv gcc10).mkDerivation {
  name = "PhotoGraph";

  src = ./.;

  nativeBuildInputs = [
    cmake
    pkg-config
    qt5.wrapQtAppsHook
  ];

  cmakeFlags = [ "-DCMAKE_BUILD_TYPE=Release" ];

  buildInputs = [
    qt512.qtbase
    libsForQt5.kconfigwidgets.dev
    boost.dev
    vips
    tbb
    abseil-cpp
    gsl

    ImageGraph
    QNodeGraph
  ];
}

#   abseil-cpp
#    boost.dev
#    fftw.dev
#    glib.dev
#    gsl
#    imagemagick.dev
#    libexif
#    libgsf.dev
#    openexr.dev
#    orc.dev
#    pcg_c
#    pcre.dev
#    tbb
#    vips
