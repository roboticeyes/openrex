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

#include <stdint.h>
#include <stdio.h>

#define LEN(x) (sizeof (x) / sizeof *(x))

void warn (const char *, ...);
void die (const char *, ...);

#define FP_CHECK(fp) \
if (!fp) \
{ \
    warn ("File pointer is not valid"); \
    return REX_ERROR_FILE_OPEN; \
}

