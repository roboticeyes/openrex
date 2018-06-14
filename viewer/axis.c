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

#include "axis.h"
#include "rex-block-lineset.h"
#include "polyline.h"

#include <stdio.h>

static struct polyline x_axis;
static struct polyline y_axis;
static struct polyline z_axis;

#define AXIS_LENGTH (10.0f)

float x_data[] =
{
    0.0f, 0.0f, 0.0f,
    AXIS_LENGTH, 0.0f, 0.0f
};

float y_data[] =
{
    0.0f, 0.0f, 0.0f,
    0.0f, AXIS_LENGTH, 0.0f
};

float z_data[] =
{
    0.0f, 0.0f, 0.0f,
    0.0f, 0.0f, AXIS_LENGTH
};

void axis_init()
{
    {
        struct rex_lineset axis =
        {
            .red = 1.0f,
            .green = 0.0f,
            .blue = 0.0f,
            .nr_vertices = 2,
            .vertices = x_data
        };
        polyline_init (&x_axis);
        polyline_set_rex_lineset (&x_axis, &axis);
    }
    {
        struct rex_lineset axis =
        {
            .red = 0.0f,
            .green = 1.0f,
            .blue = 0.0f,
            .nr_vertices = 2,
            .vertices = y_data
        };
        polyline_init (&y_axis);
        polyline_set_rex_lineset (&y_axis, &axis);
    }
    {
        struct rex_lineset axis =
        {
            .red = 0.0f,
            .green = 0.0f,
            .blue = 1.0f,
            .nr_vertices = 2,
            .vertices = z_data
        };
        polyline_init (&z_axis);
        polyline_set_rex_lineset (&z_axis, &axis);
    }
}

void axis_render (struct shader *s, struct camera *cam, mat4x4 projection)
{
    polyline_render (&x_axis, s, cam, projection);
    polyline_render (&y_axis, s, cam, projection);
    polyline_render (&z_axis, s, cam, projection);
}

void axis_destroy()
{
    polyline_free (&x_axis);
    polyline_free (&y_axis);
}
