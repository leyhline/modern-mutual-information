The files in this folder are for creating a MATLAB interface. For this purpose MATLAB's `mex` command is used.
The following instructions assume Windows with MATLAB and Visual Studio installed. 

The .cpp files in this folder contain the necessary mex functions. For compilation call:

```
mex -v COMPFLAGS="$COMPFLAGS /W3 /openmp" -O -largeArrayDims shiftmiFloat.cpp ..\src\Histogram1d.cpp ..\src\Histogram2d.cpp ..\src\utilities.cpp
```

The following arguments are passed:
* -v for verbose mode
* COMPFLAGS="$COMPFLAGS /W3 /openmp" adds more warnings (/W3) and OpenMP support for parallelization of the calculations
* -O explicitely activates optimization
* -largeArrayDims for use of the default MATLAB large-array-handling API
* The source files necessary for building the function

For further explanation see: <https://de.mathworks.com/help/matlab/ref/mex.html>
