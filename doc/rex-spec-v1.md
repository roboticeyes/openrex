<!-- TITLE: REX Data Format -->
<!-- SUBTITLE: Specification -->

# REX data format v1

The Robotic Eyes eXplorer data format (rex) is designed to store REX-relevant information efficiently in order to
optimize the data transfer between a mobile client and a server.  The suffix of the rex file is chosen to be **.rex**.

The rex format can store up to 2^16 different data blocks (65535), where one specific data block stores a certain type
of data. This data can vary from geospatial data to simulation or any binary-typed data.

The rex format stores all the information in binary representation (big endian). This allows for optimized data storage
and performant read/write operations. For instance, reading an OBJ file takes 20x more time than reading the same data
as rex.

Although the coordinates which are user facing have Z in upward direction, the REX stores the coordinates in
a form that Y is facing towards up.

## Related formats

A file format which is related to the rex format is the
[glTF](https://github.com/KhronosGroup/glTF/blob/master/specification/README.md) format.

Another similar file format is the [OpenCTM](http://openctm.sourceforge.net) specification. This is pretty much
tailored to 3D meshes only but proposes a nice compression algorithm.

## Coordinate system

Coordinate systems are the most crucial parts in computer graphics and computer vision. Every application may have its
own definition of the coordinate system, so does REX.

The geometry in REX is defined by a right-handed 3D
[Cartesian coordinate system](https://en.wikipedia.org/wiki/Cartesian_coordinate_system) as shown in the figure below.

<p align="center">
<img src="https://github.com/roboticeyes/openrex/raw/coordinate_system/doc/right-handed.png"/>
</p>

Please make sure that your input geometry is transformed according to our coordinate system specification. As an example,
if you export FBX from Revit directly, no transformation is required because all the coordinates are already in the
required right-handed system. Other systems may require a transformation (see the transformation option in `rex-importer`).

Please note, that the y-coordinate is pointing upwards, which means that the "height" in CAD is encoded as `y` in the
REX file format. However, if you position the REX model in the real world, the `y` coordinate is pointing to the sky.

The triangle orientation is required to be counter-clockwise (CCW), see the example in the data directory. Here is a
simple example which shows the `coordsys.obj` file from the `data` directory in the viewer, and in real world with the
REX Go app.

<p align="center">
<img src="https://github.com/roboticeyes/openrex/raw/coordinate_system/doc/coordinate_example.png"/>
</p>

### SketchUp

[SketchUp](https://www.sketchup.com/) has a right-handed coordinate system with a 90 degrees rotation around the X axis.
This means that `z` is pointing upwards and y is pointing backwards. We have drawn a cube in SketchUp where the green
face is pointing towards the user and the red face is pointing to the right side (see screenshot below).

If you export this model using the standard COLLADA export from SketchUp and import this data in REX, you will get the
same visualization in REX Go (see screenshot). The automated REX importer takes care that the coordinates are converted
as expected. The COLLADA file can be found in the `data` directory. You will notice that the `up_axis` is defined
as `Z_UP`. This information is interpreted and the coordinate transformation of the REX importer is activated
accordingly.

<p align="center">
<img src="https://github.com/roboticeyes/openrex/raw/coordinate_system/doc/sketchup_example.jpg"/>
</p>

## File layout

### General file structure

The rex format starts with a fixed header block, followed by a coordinate system block.
A list of different data blocks can follow. The rex can store multiple different data blocks in
one file.

| **name**                | **description**                       |
|-------------------------|---------------------------------------|
| File header block       | header information (meta-data)        |
| Coordinate system block | coordinate system for all stored data |
| Data block 1            | data                                  |
| Data block 2            | data                                  |
| ...                     | ...                                   |
| Data block n            | data                                  |


### File header block

| **size [bytes]** | **name**       | **type** | **description**           |
|------------------|----------------|----------|---------------------------|
| 4                | magic          | string   | REX1                      |
| 2                | version        | uint16_t | file version              |
| 4                | CRC32          | uint32_t | crc32 (auto calculated)   |
| 2                | nrOfDataBlocks | uint16_t | number of data blocks     |
| 2                | startData      | uint16_t | start of first data block |
| 8                | sizeDataBlocks | uint64_t | size of all data blocks   |
| 42               | reserved       | -        | reserved                  |

The size of the header block is 64 bytes.

### Coordinate system block

| **size [bytes]** | **name** | **type** | **description**                     |
|------------------|----------|----------|-------------------------------------|
| 4                | srid     | uint32_t | spatial reference system identifier |
| 2+sz             | authName | string   | name of the used system             |
| 4                | offsetX  | float    | global x-offset                     |
| 4                | offsetY  | float    | global y-offset                     |
| 4                | offsetZ  | float    | global z-offset                     |

### Data block

Every data block has a general data header block which stores general information.
The specific data block can still contain further header information, depending on the data block itself.

| **name**          | **description**    |
|-------------------|--------------------|
| Data header block | header information |
| Data block        | data               |

#### Data header block

| **size [bytes]** | **name** | **type** | **description**                  |
|------------------|----------|----------|----------------------------------|
| 2                | type     | uint16_t | data type                        |
| 2                | version  | uint16_t | version for this data block      |
| 4                | size     | uint32_t | data block size (without header) |
| 8                | dataId   | uint64_t | id which is used in the database |

The currently supported data block types are as follows. Please make sure that the IDs are not reordered.

Total size of the header is **16 bytes**.

| **Id** | **Type**         | **Description**                                                     |
|--------|------------------|---------------------------------------------------------------------|
| 0      | LineSet          | A list of vertices which get connected by line segments             |
| 1      | Text             | A position information and the actual text                          |
| 2      | PointList        | A list of 3D points with color information (e.g. point cloud)       |
| 3      | Mesh             | A triangle mesh datastructure                                       |
| 4      | Image            | A single of arbitrary format can be stored in this block            |
| 5      | MaterialStandard | A standard (mesh) material definition                               |
| 6      | PeopleSimulation | Stores people simulation data timestamp and x/y/z coordinates       |
| 7      | UnityPackage     | Stores a valid unity package (asset bundle)                         |

Please note that some of the data types offer a LOD (level-of-detail) information. This value
can be interpreted as 0 being the highest level. As data type we use 32bit for better memory alignment.

#### DataType LineSet (0)

| **size [bytes]** | **name**     | **type** | **description**               |
|------------------|--------------|----------|-------------------------------|
| 4                | red          | float    | red channel                   |
| 4                | green        | float    | green channel                 |
| 4                | blue         | float    | blue channel                  |
| 4                | nrOfVertices | uint32_t | number of vertices            |
| 4                | x0           | float    | x-coordinate of first vertex  |
| 4                | y0           | float    | y-coordinate of first vertex  |
| 4                | z0           | float    | z-coordinate of first vertex  |
| 4                | x1           | float    | x-coordinate of second vertex |
| ...              |              |          |                               |


#### DataType Text (1)

| **size [bytes]** | **name**  | **type** | **description**                   |
|------------------|-----------|----------|-----------------------------------|
| 4                | positionX | float    | x-coordinate of the position      |
| 4                | positionY | float    | y-coordinate of the position      |
| 4                | positionZ | float    | z-coordinate of the position      |
| 4                | fontSize  | float    | font size in font units (e.g. 24) |
| 2+sz             | text      | string   | text for the label                |


#### DataType PointList (2)

This data type can be used to store a set of points (e.g. colored point clouds).
The number of vertices  The number of colors can be zero. If this is the case the
points are rendered with a pre-defined color (e.g. white). If color is provided
the number of color entries must fit the number of vertices (i.e. every point needs
to have an RGB color).

| **size [bytes]** | **name**     | **type** | **description**                              |
|------------------|--------------|----------|----------------------------------------------|
| 4                | nrOfVertices | uint32_t | number of vertices                           |
| 4                | nrOfColors   | uint32_t | number of colors                             |
| 4                | x            | float    | x-coordinate of first vertex                 |
| 4                | y            | float    | y-coordinate of first vertex                 |
| 4                | z            | float    | z-coordinate of first vertex                 |
| 4                | x            | float    | x-coordinate of second vertex                |
| ...              |              |          |                                              |
| 4                | red          | float    | red component of the first vertex            |
| 4                | green        | float    | green component of the first vertex          |
| 4                | blue         | float    | blue component of the first vertex           |
| 4                | red          | float    | red component of the second vertex           |
| ...              |              |          |                                              |


#### DataType Mesh (3)

##### Mesh header

The offsets in this block refer to the index of the beginning of this data block (this means the position of the lod
field, not the general data block header!). If one needs to access from the global REX stream, the offset of the
mesh block must be added.

| **size [bytes]** | **name**       | **type** | **description**                                                  |
|------------------|----------------|----------|------------------------------------------------------------------|
| 2                | lod            | uint16_t | level of detail for the given geometry                           |
| 2                | maxLod         | uint16_t | maximal level of detail for given geometry                       |
| 4                | nrOfVtxCoords  | uint32_t | number of vertex coordinates                                     |
| 4                | nrOfNorCoords  | uint32_t | number of normal coordinates (can be zero)                       |
| 4                | nrOfTexCoords  | uint32_t | number of texture coordinates (can be zero)                      |
| 4                | nrOfVtxColors  | uint32_t | number of vertex colors (can be zero)                            |
| 4                | nrTriangles    | uint32_t | number of triangles                                              |
| 4                | startVtxCoords | uint32_t | start vertex coordinate block (relative to mesh block start)     |
| 4                | startNorCoords | uint32_t | start vertex normals block (relative to mesh block start)        |
| 4                | startTexCoords | uint32_t | start of texture coordinate block (relative to mesh block start) |
| 4                | startVtxColors | uint32_t | start of colors block (relative to mesh block start)             |
| 4                | startTriangles | uint32_t | start triangle block for vertices (relative to mesh block start) |
| 8                | materialId     | uint64_t | id which refers to the corresponding material block in this file |
| 2                | string size    | uint16_t | size of the following string name                                |
| 74               | name           | string   | name of the mesh (this is user-readable)                         |

It is assumed that the mesh data is vertex-oriented, so that additional properties such as color, normals, or
texture information is equally sized to the nrOfVtxCoords. If not available the number should/can be 0.

The mesh references a separate material block which is identified by the materialId (dataId of the material block).
Each DataMesh can only have one material block. This is similar to the `usemtl` in the OBJ file format. **If the
materialId is `INT64_MAX` (`Long.MAX_VALUE` in Java), then no material is available.**

The mesh header size is fixed with **128** bytes.

##### Vertex coordinates block

| **size [bytes]** | **name** | **type** | **description**               |
|------------------|----------|----------|-------------------------------|
| 4                | x        | float    | x-coordinate of first vertex  |
| 4                | y        | float    | y-coordinate of first vertex  |
| 4                | z        | float    | z-coordinate of first vertex  |
| 4                | x        | float    | x-coordinate of second vertex |
| ...              |          |          |                               |

##### Normals coordinates block (optional)

| **size [bytes]** | **name** | **type** | **description**                   |
|------------------|----------|----------|-----------------------------------|
| 4                | nx       | float    | x-coordinate of the first normal  |
| 4                | ny       | float    | y-coordinate of the first normal  |
| 4                | nz       | float    | z-coordinate of the first normal  |
| 4                | nx       | float    | x-coordinate of the second normal |
| ...              |          |          |                                   |

##### Texture coordinates block (optional)

| **size [bytes]** | **name** | **type** | **description**                          |
|------------------|----------|----------|------------------------------------------|
| 4                | u        | float    | u-component of first texture coordinate  |
| 4                | v        | float    | v-component of first texture coordinate  |
| 4                | u        | float    | u-component of second texture coordinate |
|                  |          |          |                                          |

##### Colors block (optional)

| **size [bytes]** | **name** | **type** | **description**                     |
|------------------|----------|----------|-------------------------------------|
| 4                | red      | float    | red component of the first vertex   |
| 4                | green    | float    | green component of the first vertex |
| 4                | blue     | float    | blue component of the first vertex  |
| 4                | red      | float    | red component of the second vertex  |
| ...              |          |          |                                     |

##### Triangle block

This is a list of integers which form one triangle. Please make sure that normal and texture
coordinates are inline with the vertex coordinates. One index refers to the same normal and texture position.

| **size [bytes]** | **name** | **type** | **description**                     |
|------------------|----------|----------|-------------------------------------|
| 4                | v0       | uint32_t | first index of the first triangle   |
| 4                | v1       | uint32_t | second index of the  first triangle |
| 4                | v2       | uint32_t | third index of the first triangle   |
| 4                | v0       | uint32_t | first index of the second triangle  |
| ...              |          |          |                                     |

#### DataType Image (4)

The Image data block can either contain an arbitrary image or a texture for a given 3D mesh. If a texture
is stored, the 3D mesh will refer to it by the `dataId`.  The data block size in the header refers to the total size of
this block (compression + data_size).

| **size [bytes]** | **name**    | **type** | **description**                        |
|------------------|-------------|----------|----------------------------------------|
| 4                | compression | uint32_t | id for supported compression algorithm |
|                  | data        | bytes    | data of the file content               |

##### Supported compression

| **ID** | **Name**           |
|--------|--------------------|
| 0      | Raw24 (RGB 24 bit) |
| 1      | Jpeg               |
| 2      | Png                |

#### DataType DataMaterialStandard (5)

The standard material block is used to set the material for the geometry specified in the mesh data block.

| **size [bytes]** | **name**     | **type** | **description**                                         |
|------------------|--------------|----------|---------------------------------------------------------|
| 4                | Ka red       | float    | RED component for ambient color                         |
| 4                | Ka green     | float    | GREEN component for ambient color                       |
| 4                | Ka blue      | float    | BLUE component for ambient color                        |
| 8                | Ka textureId | uint64_t | dataId of the referenced texture for ambient component  |
| 4                | Kd red       | float    | RED component for diffuse color                         |
| 4                | Kd green     | float    | GREEN component for diffuse color                       |
| 4                | Kd blue      | float    | BLUE component for diffuse color                        |
| 8                | Kd textureId | uint64_t | dataId of the referenced texture for diffuse component  |
| 4                | Ks red       | float    | RED component for specular color                        |
| 4                | Ks green     | float    | GREEN component for specular color                      |
| 4                | Ks blue      | float    | BLUE component for specular color                       |
| 8                | Ks textureId | uint64_t | dataId of the referenced texture for specular component |
| 4                | Ns           | float    | specular exponent                                       |
| 4                | alpha        | float    | alpha between 0..1, 1 means full opaque                 |

If no texture is available/set, then the `textureId` is set to `INT64_MAX` (`Long.MAX_VALUE` in Java) value.

#### DataType PeopleSimulation (6)

##### PeopleSimulation header

The simulation contains of two different blocks. The first block stores the mapping between every person ID to its
geometric information stored in a different REX file. For every ID, a texture mapping is expected. If no information is
available, the visualization component will pick a standard representation (e.g. a simple cylinder with radius 10cm and
height 1m).

| **size [bytes]** | **name**         | **type** | **description**                                             |
|------------------|------------------|----------|-------------------------------------------------------------|
| 4                | numberOfMappings | uint32_t | stores the number of people to geometry information mapping |
| 8                | numberOfEvents   | uint64_t | stores the total number of simulation events                |

##### PersonToMesh block

| **size [bytes]** | **name** | **type** | **description**                                                          |
|------------------|----------|----------|--------------------------------------------------------------------------|
| 8                | personId | uint64_t | personId in the simulation event block                                   |
| 8                | meshId   | uint64_t | meshId pointing to a valid mesh information (stored in a different file) |
| 8                | personId | uint64_t | personId in the simulation event block                                   |
| 8                | meshId   | uint64_t | meshId pointing to a valid mesh information (stored in a different file) |
| ...              |          |          |                                                                          |

##### SimulationEvents block

| **size [bytes]** | **name**  | **type** | **description**                                              |
|------------------|-----------|----------|--------------------------------------------------------------|
| 4                | timestamp | float    | timestamp information represented by a float value (seconds) |
| 8                | personId  | uint64_t | personId for this entry                                      |
| 4                | x         | float    | x coordinate (meters)                                        |
| 4                | y         | float    | y coordinate (meters)                                        |
| 4                | z         | float    | z coordinate (meters)                                        |
| 4                | flag      | int      | can be used for internal flagging                            |
| 4                | density   | float    | density information (typically between 0..1)                 |
| ...              |           |          |                                                              |


#### DataType UnityPackage (7)

The UnityPackage data block contains an arbitrary pre-prepared Unity package. An example is to store animation data
which can then directly be used by the Unity app to be included. The data block size in the header refers to the total
size of this block plus the additional two fields. E.g.

| **size [bytes]** | **name**        | **type**  | **description**                                              |
|------------------|-----------------|-----------|--------------------------------------------------------------|
| 2                | target platform | uint16_t  | target platform for the asset package                        |
| 2                | unity version   | uint16_t  | Unity version that was used to build the assetbundle (at least 20180)    |
|                  | data            | bytes     | data of the unity asset content                              |

The current values for target platform are:

| **value** | **platform** |
|-----------|--------------|
| 0         | Android      |
| 1         | iOS          |
| 2         | WSA          |

Please note that the **UnityPackage is only supported by REX Holo**. Android and iOS cannot be used because
of some Unity bugs in the asset handling.
