/*
 * Copyright 2019 Robotic Eyes GmbH
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

#include <GL/glew.h>

#include "bbox.h"
#include "camera.h"
#include "rex-block-mesh.h"
#include "linmath.h"
#include "shader.h"

/**
 * This is the mesh which can be rendered
 */
struct material_standard
{
    uint64_t id;
    float    kd_red;       //!< RED component for diffuse color (0..1)
    float    kd_green;     //!< GREEN component for diffuse color (0..1)
    float    kd_blue;      //!< BLUE component for diffuse color (0..1)
};

void material_standard_init (struct material_standard *);
void material_standard_free (struct material_standard *);

