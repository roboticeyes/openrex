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

### Windows build

With Visual Studio 2017 you should be able to use the CMakeLists file directly.
See https://channel9.msdn.com/Events/Visual-Studio/Visual-Studio-2017-Launch/T138

## Implementation status

### Prio 1
* [x] Read file header
* [x] Write file header
* [x] Read mesh block
* [x] Write mesh block
* [x] Read material block
* [x] Write material block
* [ ] Read image block
* [x] Write image block

### Prio 2
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
