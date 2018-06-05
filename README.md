<p align="center">
<img src="doc/openrex.png" />
</p>

[![Build Status](https://travis-ci.org/roboticeyes/openrex.svg?branch=master)](https://travis-ci.org/roboticeyes/openrex)

OpenREX provides the specification for a binary file format which is used for the Robotic Eyes REX platform.

This repository contains

* the [complete specification](doc/rex-spec-v1.md)
* native reader/writer (in progress, contribution welcome!)
* tools for working with REX files

<p align="center">
<img src="doc/teaser.jpg" />
</p>

# Development

In order to compile the code, you need at least a C11 compiler (e.g. gcc, clang) and
`cmake` installed on your system. The code should work for Linux, MacOS, and Windows.

For compiling all tests, you also need to install [check](https://github.com/libcheck/check). You can simply use your
package manager to install the test library (e.g. `apt-get install check`).

## Build

There are different cmake options which can be obtained from the CMakeOptions.txt file. The default behavior only
compiles the REX library and the REX tools.

It is simple to compile the library on your system:

```
mkdir build
cd build
cmake ..
make
make install
```

## Additional tools

### Debug viewer

OpenREX contains a simple debug viewer which is able to show the geometry information of a REX file. Currenlty not materials
are supported! Enable the viewer by

```
cmake -DVIEWER=on ..
```

The debug viewer requires **SDL2** installed on your system including **GLEW**. Make sure to have the following packages installed (`libglew-dev` and `libsdl2-dev`).

### Importer (experimental)

There is an experimental REX importer using the Assimp library which is currently not in full production yet. Contribution
is welcome. To enable the importer use

```
cmake -DIMPORTER=on ..
```

## Source code formatting

If you plan a contribution, please make sure to format your code according to the following `astyle` settings:

```
--style=allman --indent=spaces=4 --align-pointer=name --align-reference=name --indent-switches --indent-cases --pad-oper --pad-paren-out --pad-header --unpad-paren --indent-namespaces --remove-braces --convert-tabs --mode=c

```

## Windows build

With Visual Studio 2017 you should be able to use the CMakeLists file directly.
See https://channel9.msdn.com/Events/Visual-Studio/Visual-Studio-2017-Launch/T138

## Contribution and ideas

Your contribution is welcome. Here are some ideas:

* rex-dump: dumps a certain block to the stdout which can then be piped into other programs (e.g. feh)
* rex-view: allow pipe input from commandline getting a certain data block
* Extend rex-info to support filtering (e.g. only show meshes, or materials)
* Save images from REX to files
* Continue to integrate assimp for data conversion

### Guidelines and pre-requisite(s)

* Keep it simple and follow the UNIX philosophy
* You should be able to write robust C code
* Make sure to format the code according to our style (see above)
* Check your code for memory leaks using `valgrind`
* Your code should run cross-platform (Windows/Linux/Mac)

## Contributions

* [linmath.h](https://github.com/datenwolf/linmath.h) for linear math operations
* [slash.h](https://bitbucket.org/busstop/helios) for reading LAS files

Please see the copyright notice in the according files.
