Fork of PBRT v2 for COMP 408 at VUW.
Not really a fork because of build system changes and removing all but the renderer source.

Original [here](http://github.com/mmp/pbrt-v2).

PBRT for COMP 408
=================

This is just the PBRT renderer (no tools / exporters), with a CMake build system.

Bison is required in order to generate the parser.

The `master` branch has the CMake and pbrt-v2 source (modified just enough to build nicely).

Building on Linux
-----------------

From repository root:
```
mkdir build
cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
make -j4
cd ..
```
Then run like:
```
./build/bin/pbrt ./scenes/bunny.pbrt
```

Building on Windows (VS2013, x64)
---------------------------------

A (somewhat old but sufficient) version of Bison can be obtained from [here](http://gnuwin32.sourceforge.net/packages/bison.htm). This must be installed to a path without spaces; note that the default does _not_ satisfy this. The binary directory must be added to your path to allow CMake to find the executable.

From repository root, in cmd:
```
mkdir build
cd build
cmake .. -G "Visual Studio 12 Win64"
```

In VS:
- Set ```pbrt``` as the startup project
- Set its working directory to ```$(SolutionDir)..``` (all configs)
- Set its command arguments to e.g. ```./scenes/bunny.pbrt``` (all configs)
- Switch to release config, as non-optimized builds will be very slow to render

