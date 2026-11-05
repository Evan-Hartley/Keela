#!/bin/bash

if [[ "$#" < 1 ]]; then
    echo ' '
    echo "For faster build use the command: bash build-Keela.sh <job-number>"
    echo "<job-number> max = # of computer processors"
    echo "<job-number> should be a multiple of 4"
    echo "Cancel current build using: Ctl+C"
    echo ' '
fi

set -euo pipefail

buildDir=$PWD

cd ~

bold='\e[1m'
txtred='\e[0;31m'
txtgrn='\e[0;32m'
txtylw='\e[0;33m'
txtcyn='\e[0;36m'
txtrst='\e[0m'

InstallPkg()
{
    local PACKAGE=$1
    PkgName=$2

    echo -e "${bold}${txtcyn}Installing $PkgName...${txtrst}"

    yes Y | pacman -S --noconfirm --quiet $PACKAGE >/dev/null

    if [ $? -eq 0 ]; then
        echo -e "${bold}${txtred}Failure: $PkgName ran into an error while installing.${txtrst}"
    else
        echo -e "${bold}${txtgrn}Successfully installed $PkgName.${txtrst}"
    fi
} 

KeelaInstallPkgs()
{
    set +e
    InstallPkg 'mingw-w64-ucrt-x86_64-gcc' 'C/C++ Compiler'
    InstallPkg 'mingw-w64-ucrt-x86_64-cmake' 'CMake'
    InstallPkg 'mingw-w64-ucrt-x86_64-freetype' 'FreeType'
    InstallPkg 'mingw-w64-ucrt-x86_64-libtiff' 'LibTiff'
    InstallPkg 'mingw-w64-ucrt-x86_64-gtkmm-4.0' 'GTKMM4'
    InstallPkg 'mingw-w64-ucrt-x86_64-gstreamer' 'GStreamer'
    InstallPkg 'mingw-w64-ucrt-x86_64-gst-plugins-base' 'GST-Plugins-Base'
    InstallPkg 'mingw-w64-ucrt-x86_64-gst-plugins-good' 'GST-Plugins-Good'
    InstallPkg 'mingw-w64-ucrt-x86_64-gst-plugins-bad-libs' 'GST-Plugins-Bad'
    InstallPkg 'mingw-w64-ucrt-x86_64-gst-plugins-ugly' 'GST-Plugins-Ugly'
    InstallPkg 'mingw-w64-ucrt-x86_64-gst-libav' 'GST-LibAV'
    InstallPkg 'mingw-w64-ucrt-x86_64-aravis' 'Aravis'
    InstallPkg 'mingw-w64-ucrt-x86_64-aravis-gst' 'Aravis-GST'
    InstallPkg 'mingw-w64-ucrt-x86_64-spdlog' 'SPDLOG'
    InstallPkg 'mingw-w64-ucrt-x86_64-tbb' 'oneTBB'
    InstallPkg 'mingw-w64-ucrt-x86_64-graphviz' 'GraphViz'
    InstallPkg 'mingw-w64-ucrt-x86_64-doxygen' 'DOXYGEN'
    InstallPkg 'mingw-w64-ucrt-x86_64-nsis' 'NSIS'
    set -e
}

read -p "Would you like to install Keela's mandatory packages (neccessary for first installation)? [Y/n]: " yn

case "$yn" in
    [Yy]*) KeelaInstallPkgs ;;
    [Nn]*) echo -e "${bold}${txtylw}Skipping dependency package installation/update step.${txtrst}" ;;
    *)     echo "Invalid input. Please answer y or n." ;;
esac


cd "${buildDir}"

mkdir -p build

cd build

cmake .. -G Ninja -DCMAKE_BUILD_TYPE=Release

if [[ "$#" < 1 ]]; then
    ninja
else
    ninja -j${#1}
fi

# cpack -G NSIS --config build/CPackConfig.cmake