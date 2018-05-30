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

For compiling all tests, you also need to install [check](https://github.com/libcheck/check). You can simply use your
package manager to install the test library.

The debug viewer also requires **SDL2** installed on your system including **GLEW**. For Ubuntu-based system just use
the following commands:

```
apt-get install libglew-dev
apt-get install libsdl2-dev
apt-get install check
```

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

* [ ] Read/write lineset block
* [ ] Read/write vertex block
* [ ] Read/write text block
* [ ] Read/write people simulation block

## Contribution and ideas

Your contribution is welcome. Here are some ideas:

* rex-dump: dumps a certain block to the stdout which can then be piped into other programs (e.g. feh)
* rex-view: allow pipe input from commandline getting a certain data block
* Extend rex-info to support filtering (e.g. only show meshes, or materials)
* Save images from REX to files
* Integrate assimp for data conversion

### Guidelines and pre-requisite(s)

* Keep it simple and follow the UNIX philosophy
* You should be able to write robust C code
* Make sure to format the code according to our style (see above)
* Check your code for memory leaks using `valgrind`
* Your code should run under Windows/Linux/Mac


