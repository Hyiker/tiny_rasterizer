// this file is based on
// https://github.com/zauonlok/renderer/blob/master/renderer/core/platform.h
// MIT License
//
// Copyright (c) 2020 Zhou Le
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
#ifndef PLATFORM_H
#define PLATFORM_H
#include <functional>

#include "rasterizer/Scene.hpp"
typedef struct window window_t;
typedef enum { KEY_A, KEY_D, KEY_S, KEY_W, KEY_SPACE, KEY_NUM } keycode_t;
typedef enum { BUTTON_L, BUTTON_R, BUTTON_NUM } button_t;
typedef struct {
    bool kc, bc, sc;
    std::function<void(window_t *, keycode_t, int pressed)> key_callback;
    std::function<void(window_t *, button_t, int pressed)> button_callback;
    std::function<void(window_t *window, float)> scroll_callback;
} callbacks_t;
typedef enum { FORMAT_LDR, FORMAT_HDR } format_t;

typedef struct {
    format_t format;
    int width, height, channels;
    unsigned char *ldr_buffer;
    float *hdr_buffer;
} image_t;

/* platform initialization */
void platform_initialize(void);
void platform_terminate(void);

/* window related functions */
window_t *window_create(const char *title, int width, int height);
void window_destroy(window_t *window);
int window_should_close(window_t *window);
void window_set_userdata(window_t *window, void *userdata);
void *window_get_userdata(window_t *window);
void window_draw_buffer(window_t *window, Rasterizer::Scene *buffer);

/* input related functions */
void input_poll_events(void);
int input_key_pressed(window_t *window, keycode_t key);
int input_button_pressed(window_t *window, button_t button);
void input_query_cursor(window_t *window, float *xpos, float *ypos);
void input_set_callbacks(window_t *window, callbacks_t callbacks);

/* misc platform functions */
float platform_get_time(void);
#endif /* PLATFORM_H */
