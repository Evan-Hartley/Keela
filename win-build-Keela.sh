#!/bin/bash

if [[ "$#" < 1 ]]; then
    echo " "
    echo "For faster build use the command: bash build-Keela.sh <job-number>"
    echo "<job-number> max = # of computer processors"
    echo "<job-number> should be a multiple of 4"
    echo "Cancel current build using: Ctl+C"
    echo " "
fi

set -euo pipefail

buildDir="$PWD"

cd ~

InstallPkg()
{
    local PACKAGE=$1
    PkgName=$2
    bold='\e[1m'
    txtgrn='\e[0;32m'
    txtcyn='\e[0;36m'
    txtrst='\e[0m'

    if pacman -Q $PACKAGE > /dev/null; then
        echo -e "${bold}${txtgrn}$PkgName is already installed and up to date.${txtrst}"
    else
        echo -e "${bold}${txtcyn}Installing $PkgName${txtrst}"

        yes Y | pacman -S --quiet $PACKAGE

        echo -e "${bold}${txtgrn}Successfully installed $PkgName.${txtrst}"
    fi
}

# if [[ $PkgName = "Toolchain" ]]; then
#     echo -e "${bold}${txtcyn}Please press enter when asked to make a selection."
#     echo -e "${bold}${txtcyn}Please answer Y when asked to proceed with installation"
#     pacman -S $PACKAGE
# fi
# InstallPkg "toolchain" "Toolchain"
InstallPkg "mingw-w64-ucrt-x86_64-cmake" "CMake"
InstallPkg "mingw-w64-ucrt-x86_64-gtkmm3" "GTKMM3"
InstallPkg "mingw-w64-ucrt-x86_64-gstreamer" "GStreamer"
InstallPkg "mingw-w64-ucrt-x86_64-gst-plugins-base" "GST-Plugins-Base"
InstallPkg "mingw-w64-ucrt-x86_64-gst-plugins-good" "GST-Plugins-Good"
InstallPkg "mingw-w64-ucrt-x86_64-gst-plugins-bad" "GST-Plugins-Bad"
InstallPkg "mingw-w64-ucrt-x86_64-gst-plugins-ugly" "GST-Plugins-Ugly"
InstallPkg "mingw-w64-ucrt-x86_64-gst-libav" "GST-LibAV"
InstallPkg "mingw-w64-ucrt-x86_64-aravis" "Aravis"
InstallPkg "mingw-w64-ucrt-x86_64-aravis-gst" "Aravis-GST"
InstallPkg "mingw-w64-ucrt-x86_64-spdlog" "SPDLOG"
InstallPkg "mingw-w64-ucrt-x86_64-tbb" "oneTBB"
InstallPkg "mingw-w64-ucrt-x86_64-doxygen" "DOXYGEN"
InstallPkg "mingw-w64-ucrt-x86_64-nsis" "NSIS"

mkdir build

cd build

cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

if [[ "$#" < 1 ]]; then
    ninja
else
    ninja -j${#1}
fi

cpack -G NSIS