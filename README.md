# modern-mutual-information
Histogram-based calculation of mutual information using C++11.
To be more precise two arrays of data are shifted against each other while mutual information is calculated for each step.
This allows i.a. for analyzing two signals where the response in delayed by some unknown amount of time.

The calculations run parallized over all CPU cores using OpenMP.

There is an interface for the command line and for MATLAB.

## Command line interface

### Installation
This requires [CMake](https://cmake.org/) for building on multiple platforms. 
The essential steps after downloading/cloning the repository are:
```
cd modern-mutual-information      # Move to the newly created directory
mkdir build                       # Create a new folder for the build
cd build                          # and switch to this folder
cmake ..                          # Run CMake to generate the necessary build files
make                              # Run your build system e.g. make (Linux), Visual Studio (Windows)
```

Now there should be two executables: `shiftmi` and `run_tests` where the latter one
checks if there are obvious errors in the program.

### Usage
run `shiftmi --help` for usage instructions.

Essentially the program reads two files containing some numeric data. The data can be stored as CSV or in binary representation
(with single or double precision). After calculation the output gets printed on the screen or is written to a file.
One can use bootstrapping for a more robust output but it will also take much longer since multiple iterations are necessary.

## MATLAB
See [the matlab folder](matlab) for instructions on installation.

For each function a .cpp file for the MATLAB interface as well as a .m file for a MATLAB wrapper are included.
After successful compilation you will want to run the function with the functions defined in the .m files.

## Notes
* There is a prototype of [a CUDA implementation](src/CudaMI.cu) included for running the calculations on the GPU.
This is ignored during compilation because the calculations can not be easily parallized on the GPU and are therefore
not much faster than CPU calculations but less robust. Reason is that some for-loops can not be unrolled and therefore
fast shared memory can not be used.
