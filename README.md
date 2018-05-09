<p align="center">
<img src="doc/openrex.png" />
</p>

[![Build Status](https://travis-ci.org/roboticeyes/openrex.svg?branch=master)](https://travis-ci.org/roboticeyes/openrex)

OpenREX provides the specification for a binary file format which is used for the Robotic Eyes REX platform.

This repository contains

* the [complete specification](doc/rex-spec-v1.md)
* native reader/writer (in progress, contribution welcome!)
* tools for working with REX files

# Development

In order to compile the code, you need at least a C11 compiler (e.g. gcc, clang) and
`cmake` installed on your system. The code should work for Linux, MacOS, and Windows.

## Build

```
mkdir build
cd build
cmake ..
make
make install
```

## Source code formatting

Please use `astyle` to format your code with the following settings:

```
--style=allman --indent=spaces=4 --align-pointer=name --align-reference=name --indent-switches --indent-cases --pad-oper --pad-paren-out --pad-header --unpad-paren --indent-namespaces --remove-braces --convert-tabs --mode=c

```

### Windows build

With Visual Studio 2017 you should be able to use the CMakeLists file directly.
See https://channel9.msdn.com/Events/Visual-Studio/Visual-Studio-2017-Launch/T138

## Implementation status

* [ ] Read lineset block
* [ ] Write lineset block
* [ ] Read vertex block
* [ ] Write vertex block
* [ ] Read text block
* [ ] Write text block
* [ ] Read peoplesimulation block
* [ ] Write peoplesimulation block
* [ ] Read unitypackage block
* [ ] Write unitypackage block

## Contribution and ideas

Your contribution is welcome. Here are some ideas:

* rex-view: SDL2-based OpenGL viewer of REX files
* Extend rex-info to support filtering (e.g. only show meshes, or materials)
* Save images from REX to files

### Guidelines and pre-requisite(s)

* Keep it simple and follow the UNIX philosophy
* You should be able to write robust C code
* Make sure to format the code according to our style (see above)
* Check your code for memory leaks using `valgrind`
* Your code should run under Windows/Linux/Mac


