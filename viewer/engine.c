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

#include "engine.h"

#include <SDL2/SDL.h>

static char frame_rate_string_var[12];

static int frame_rate_var = 0;
static double frame_time_var = 0.0;

static unsigned long frame_start_time = 0.0;
static unsigned long frame_end_time = 0.0;

static const double frame_update_rate = 0.5;

static int frame_counter = 0;
static double frame_acc_time = 0.0;


void frame_begin() {
  frame_start_time = SDL_GetTicks();
}

void frame_end() {

  frame_end_time = SDL_GetTicks();

  frame_time_var = ((double)(frame_end_time - frame_start_time) / 1000.0f);
  frame_acc_time += frame_time_var;
  frame_counter++;

  if (frame_acc_time > frame_update_rate) {
    frame_rate_var = round((double)frame_counter / frame_acc_time);
    frame_counter = 0;
    frame_acc_time = 0.0;
  }

  sprintf(frame_rate_string_var,"%i",frame_rate_var);
}

char* frame_rate_string() {
  return frame_rate_string_var;
}

