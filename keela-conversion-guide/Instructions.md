# File Conversion

## Package Installation

In MSYS MINGW64 run this command (only needed for first run on each device):
`pacman -S mingw-w64-x86_64-ffmpeg`

## Conversion Commands

In MSYS MINGW64 run these commands:
`cd '<path-to-keela-conversion-guide>'`
`bash ConvertData.sh <root-path-to-recording-files> <YYYYMMDD> <number-of-cameras> <number-of-bits>`

These converts the files to .mat arrays, selected for compatibility with the following analysis code recommendations

# Analysis Software Recommendations

Some options for analysis:
- Creating your own analysis software and protocol using MatLab
- VizCOM from the CHAOS Lab at Georgia Institute of Technology, with special alterations to allow for varying dimensions: [VizCOM-adjustable](https://github.com/Evan-Hartley/VizCOM)
- optimap from the Cardiac Vision Laboratory at University of California San Fransisco: [optimap site](https://cardiacvision.ucsf.edu/optimap)
