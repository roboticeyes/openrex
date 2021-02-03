/*
 * Copyright 2018 Robotic Eyes GmbH
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.*
 */
#pragma once

 /**
  * \file
  * \brief REX scenenode block storing metadata and transformation of a node which is embedded into a scene graph
  *
  * | **size [bytes]** | **name**     | **type** | **description**                                      |
  * |------------------|--------------|----------|------------------------------------------------------|
  * | 8                | geometryId   | uint64   | Id of a data block containing geometry (can be zero) |
  * | 32               | name         | string   | name of the node (can be empty)                      |
  * | 4                | tx           | float    | translation X                                        |
  * | 4                | ty           | float    | translation y                                        |
  * | 4                | tz           | float    | translation Z                                        |
  * | 4                | rx           | float    | rotation X                                           |
  * | 4                | ry           | float    | rotation Y                                           |
  * | 4                | rz           | float    | rotation Z                                           |
  * | 4                | rw           | float    | rotation W                                           |
  * | 4                | sx           | float    | scale X                                              |
  * | 4                | sy           | float    | scale Y                                              |
  * | 4                | sz           | float    | scale Z                                              |
  * 
  * The transformation is based on a local coordinate system. 
  * The transformation matrix is built by T * R * S; first the scale is applied to the vertices, 
  * then the rotation, and then the translation.
  * 
  * The translation is given in unit meters. The rotation is given in quaternion (x,y,z,w), 
  * where w is the scalar. The scale vector contains the scale values in each direction given in unit meters.

  * The geometryId can be zero which means that no geometry should be displayed. 
  * This indicates that the scenenode is an intermediate node in the scenegraph. 
  * All leafnodes in the scenegraph have to contain geometry information.
  */

#include <stdint.h>
#include "rex-header.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
* Stores all the properties for a REX scenenode
*/
struct rex_scenenode
{
    uint64_t geometryId;                     //!< Id of a data block containing geometry
    char name[REX_SCENENODE_NAME_MAX_SIZE];  //!< name of the node
    float tx;                                //!< translation X
    float ty;                                //!< translation Y
    float tz;                                //!< translation Z
    float rx;                                //!< rotation X
    float ry;                                //!< rotation Y
    float rz;                                //!< rotation Z
    float rw;                                //!< rotation W
    float sx;                                //!< scale X
    float sy;                                //!< scale Y 
    float sz;                                //!< scale Z 
};

/**
 * Reads a scenenode block from the given pointer. This call will allocate memory
 * for the scenenode. The caller is responsible to free this memory!
 *
 * \param ptr pointer to the block start
 * \param scenenode the rex_scenenode structure which gets filled
 * \return the pointer to the memory block after the rex_scenenode block
 */
uint8_t* rex_block_read_scenenode(uint8_t* ptr, struct rex_scenenode* scenenode);

/**
 * Writes a scenenode block to binary. Memory will be allocated and the caller
 * must take care of releasing the memory.
 *
 * \param id the data block ID
 * \param header the REX header which gets modified according the the new block, can be NULL
 * \param scenenode the scenenode which should get serialized
 * \param sz the total size of the of the data block which is returned
 * \return a pointer to the data block
 */
uint8_t* rex_block_write_scenenode(uint64_t id, struct rex_header* header, struct rex_scenenode* scenenode, long* sz);

#ifdef __cplusplus
}
#endif
