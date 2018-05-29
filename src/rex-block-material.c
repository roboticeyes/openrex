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

#include "rex-block-material.h"
#include "util.h"

uint8_t *rex_block_read_material (uint8_t *ptr, struct rex_material_standard *mat)
{
    MEM_CHECK (ptr)
    MEM_CHECK (mat)

    rexcpy (&mat->ka_red, ptr, sizeof (float));
    rexcpy (&mat->ka_green, ptr, sizeof (float));
    rexcpy (&mat->ka_blue, ptr, sizeof (float));
    rexcpy (&mat->ka_textureId, ptr, sizeof (uint64_t));
    rexcpy (&mat->kd_red, ptr, sizeof (float));
    rexcpy (&mat->kd_green, ptr, sizeof (float));
    rexcpy (&mat->kd_blue, ptr, sizeof (float));
    rexcpy (&mat->kd_textureId, ptr, sizeof (uint64_t));
    rexcpy (&mat->ks_red, ptr, sizeof (float));
    rexcpy (&mat->ks_green, ptr, sizeof (float));
    rexcpy (&mat->ks_blue, ptr, sizeof (float));
    rexcpy (&mat->ks_textureId, ptr, sizeof (uint64_t));
    rexcpy (&mat->ns, ptr, sizeof (float));
    rexcpy (&mat->alpha, ptr, sizeof (float));
    return ptr;
}
