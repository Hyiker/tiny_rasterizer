// this file is based on
// https://github.com/zauonlok/renderer/blob/9ed5082f0eda453f0b2a0d5ec37cf5a60f0207f6/renderer/platforms/macos.m

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
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <Cocoa/Cocoa.h>
#include <mach-o/dyld.h>
#include <mach/mach_time.h>
#include <unistd.h>
#include "graphics/platform.h"
#define PATH_SIZE 256
#define UNUSED_VAR(x) ((void)(x))

struct window {
    NSWindow *handle;
    image_t *surface;
    /* common data */
    int should_close;
    char keys[KEY_NUM];
    char buttons[BUTTON_NUM];
    callbacks_t callbacks;
    void *userdata;
};

/* platform initialization */

static NSAutoreleasePool *g_autoreleasepool = NULL;

static void create_menubar(void) {
    NSMenu *menu_bar, *app_menu;
    NSMenuItem *app_menu_item, *quit_menu_item;
    NSString *app_name, *quit_title;

    menu_bar = [[[NSMenu alloc] init] autorelease];
    [NSApp setMainMenu:menu_bar];

    app_menu_item = [[[NSMenuItem alloc] init] autorelease];
    [menu_bar addItem:app_menu_item];

    app_menu = [[[NSMenu alloc] init] autorelease];
    [app_menu_item setSubmenu:app_menu];

    app_name = [[NSProcessInfo processInfo] processName];
    quit_title = [@"Quit " stringByAppendingString:app_name];
    quit_menu_item = [[[NSMenuItem alloc] initWithTitle:quit_title
                                                 action:@selector(terminate:)
                                          keyEquivalent:@"q"] autorelease];
    [app_menu addItem:quit_menu_item];
}

static void create_application(void) {
    if (NSApp == nil) {
        g_autoreleasepool = [[NSAutoreleasePool alloc] init];
        [NSApplication sharedApplication];
        [NSApp setActivationPolicy:NSApplicationActivationPolicyRegular];
        create_menubar();
        [NSApp finishLaunching];
    }
}


void platform_initialize(void) {
    create_application();
}

void platform_terminate(void) {
    assert(g_autoreleasepool != NULL);
    [g_autoreleasepool drain];
    g_autoreleasepool = [[NSAutoreleasePool alloc] init];
}

/* window related functions */

@interface WindowDelegate : NSObject <NSWindowDelegate>
@end

@implementation WindowDelegate {
    window_t *_window;
}

- (instancetype)initWithWindow:(window_t *)window {
    self = [super init];
    if (self != nil) {
        _window = window;
    }
    return self;
}

- (BOOL)windowShouldClose:(NSWindow *)sender {
    UNUSED_VAR(sender);
    _window->should_close = 1;
    return NO;
}

@end

/*
 * for virtual-key codes, see
 * https://stackoverflow.com/questions/3202629/where-can-i-find-a-list-of-mac-virtual-key-codes
 */
static void handle_key_event(window_t *window, int virtual_key, char pressed) {
    keycode_t key;
    switch (virtual_key) {
        case 0x00: key = KEY_A;     break;
        case 0x02: key = KEY_D;     break;
        case 0x01: key = KEY_S;     break;
        case 0x0D: key = KEY_W;     break;
        case 0x31: key = KEY_SPACE; break;
        default:   key = KEY_NUM;   break;
    }
    if (key < KEY_NUM) {
        window->keys[key] = pressed;
        if (window->callbacks.kc) {
            window->callbacks.key_callback(window, key, pressed);
        }
    }
}

static void handle_button_event(window_t *window, button_t button,
                                char pressed) {
    window->buttons[button] = pressed;
    if (window->callbacks.bc) {
        window->callbacks.button_callback(window, button, pressed);
    }
}

static void handle_scroll_event(window_t *window, float offset) {
    if (window->callbacks.sc) {
        window->callbacks.scroll_callback(window, offset);
    }
}

@interface ContentView : NSView
@end

@implementation ContentView {
    window_t *_window;
}

- (instancetype)initWithWindow:(window_t *)window {
    self = [super init];
    if (self != nil) {
        _window = window;
    }
    return self;
}

- (BOOL)acceptsFirstResponder {
    return YES;  /* to receive key-down events */
}

- (void)drawRect:(NSRect)dirtyRect {
    image_t *surface = _window->surface;
    NSBitmapImageRep *rep = [[[NSBitmapImageRep alloc]
            initWithBitmapDataPlanes:&(surface->ldr_buffer)
                          pixelsWide:surface->width
                          pixelsHigh:surface->height
                       bitsPerSample:8
                     samplesPerPixel:3
                            hasAlpha:NO
                            isPlanar:NO
                      colorSpaceName:NSCalibratedRGBColorSpace
                         bytesPerRow:surface->width * 4
                        bitsPerPixel:32] autorelease];
    NSImage *nsimage = [[[NSImage alloc] init] autorelease];
    [nsimage addRepresentation:rep];
    [nsimage drawInRect:dirtyRect];
}

- (void)keyDown:(NSEvent *)event {
    handle_key_event(_window, [event keyCode], 1);
}

- (void)keyUp:(NSEvent *)event {
    handle_key_event(_window, [event keyCode], 0);
}

- (void)mouseDown:(NSEvent *)event {
    UNUSED_VAR(event);
    handle_button_event(_window, BUTTON_L, 1);
}

- (void)mouseUp:(NSEvent *)event {
    UNUSED_VAR(event);
    handle_button_event(_window, BUTTON_L, 0);
}

- (void)rightMouseDown:(NSEvent *)event {
    UNUSED_VAR(event);
    handle_button_event(_window, BUTTON_R, 1);
}

- (void)rightMouseUp:(NSEvent *)event {
    UNUSED_VAR(event);
    handle_button_event(_window, BUTTON_R, 0);
}

- (void)scrollWheel:(NSEvent *)event {
    float offset = (float)[event scrollingDeltaY];
    if ([event hasPreciseScrollingDeltas]) {
        offset *= 0.1f;
    }
    handle_scroll_event(_window, offset);
}

@end

static NSWindow *create_window(window_t *window, const char *title,
                               int width, int height) {
    NSRect rect;
    NSUInteger mask;
    NSWindow *handle;
    WindowDelegate *delegate;
    ContentView *view;

    rect = NSMakeRect(0, 0, width, height);
    mask = NSWindowStyleMaskTitled
           | NSWindowStyleMaskClosable
           | NSWindowStyleMaskMiniaturizable;
    handle = [[NSWindow alloc] initWithContentRect:rect
                                         styleMask:mask
                                           backing:NSBackingStoreBuffered
                                             defer:NO];
    assert(handle != nil);
    [handle setTitle:[NSString stringWithUTF8String:title]];
    [handle setColorSpace:[NSColorSpace genericRGBColorSpace]];

    /*
     * the storage semantics of NSWindow.setDelegate is @property(assign),
     * or @property(weak) with ARC, we must not autorelease the delegate
     */
    delegate = [[WindowDelegate alloc] initWithWindow:window];
    assert(delegate != nil);
    [handle setDelegate:delegate];

    view = [[[ContentView alloc] initWithWindow:window] autorelease];
    assert(view != nil);
    [handle setContentView:view];
    [handle makeFirstResponder:view];

    return handle;
}

static image_t *image_create(int width, int height, int channels, format_t format) {
    int num_elems = width * height * channels;
    image_t *image;

    assert(width > 0 && height > 0 && channels >= 1 && channels <= 4);
    assert(format == FORMAT_LDR || format == FORMAT_HDR);

    image = (image_t*)malloc(sizeof(image_t));
    image->format = format;
    image->width = width;
    image->height = height;
    image->channels = channels;
    image->ldr_buffer = NULL;
    image->hdr_buffer = NULL;

    if (format == FORMAT_LDR) {
        int size = sizeof(unsigned char) * num_elems;
        image->ldr_buffer = (unsigned char*)malloc(size);
        memset(image->ldr_buffer, 0, size);
    } else {
        int size = sizeof(float) * num_elems;
        image->hdr_buffer = (float*)malloc(size);
        memset(image->hdr_buffer, 0, size);
    }

    return image;
}
void image_release(image_t *image) {
    free(image->ldr_buffer);
    free(image->hdr_buffer);
    free(image);
}
window_t *window_create(const char *title, int width, int height) {
    window_t *window;

    assert(NSApp && width > 0 && height > 0);

    window = (window_t*)malloc(sizeof(window_t));
    memset(window, 0, sizeof(window_t));
    window->handle = create_window(window, title, width, height);
    window->surface = image_create(width, height, 4, FORMAT_LDR);

    [window->handle makeKeyAndOrderFront:nil];
    return window;
}

void window_destroy(window_t *window) {
    [window->handle orderOut:nil];

    [[window->handle delegate] release];
    [window->handle close];

    [g_autoreleasepool drain];
    g_autoreleasepool = [[NSAutoreleasePool alloc] init];

    image_release(window->surface);
    free(window);
}

int window_should_close(window_t *window) {
    return window->should_close;
}

void window_set_userdata(window_t *window, void *userdata) {
    window->userdata = userdata;
}

void *window_get_userdata(window_t *window) {
    return window->userdata;
}

static void present_surface(window_t *window) {
    [[window->handle contentView] setNeedsDisplay:YES];  /* invoke drawRect */
}

static void blit_rgb(Rasterizer::Scene* scene, image_t *dst) {
    int width = dst->width;
    int height = dst->height;
    int r, c;

    assert(scene->width == dst->width && scene->height == dst->height);
    assert(dst->format == FORMAT_LDR && dst->channels == 4);

    for (r = 0; r < height; r++) {
        for (c = 0; c < width; c++) {
            int src_index = (r * width + c) * 4;
            int dst_index = (r * width + c) * 4;
            Rasterizer::RGBColor src_pixel = scene->pixels[r][c] * 255.f;
            unsigned char *dst_pixel = &dst->ldr_buffer[dst_index];
            dst_pixel[0] = src_pixel.x;  /* red */
            dst_pixel[1] = src_pixel.y;  /* green */
            dst_pixel[2] = src_pixel.z;  /* blue */
        }
    }
}

void window_draw_buffer(window_t *window,  Rasterizer::Scene* buffer) {
    blit_rgb(buffer, window->surface);
    present_surface(window);
}

/* input related functions */

void input_poll_events(void) {
    while (1) {
        NSEvent *event = [NSApp nextEventMatchingMask:NSEventMaskAny
                                            untilDate:[NSDate distantPast]
                                               inMode:NSDefaultRunLoopMode
                                              dequeue:YES];
        if (event == nil) {
            break;
        }
        [NSApp sendEvent:event];
    }
    [g_autoreleasepool drain];
    g_autoreleasepool = [[NSAutoreleasePool alloc] init];
}

int input_key_pressed(window_t *window, keycode_t key) {
    assert(key >= 0 && key < KEY_NUM);
    return window->keys[key];
}

int input_button_pressed(window_t *window, button_t button) {
    assert(button >= 0 && button < BUTTON_NUM);
    return window->buttons[button];
}

void input_query_cursor(window_t *window, float *xpos, float *ypos) {
    NSPoint point = [window->handle mouseLocationOutsideOfEventStream];
    NSRect rect = [[window->handle contentView] frame];
    *xpos = (float)point.x;
    *ypos = (float)(rect.size.height - 1 - point.y);
}

void input_set_callbacks(window_t *window, callbacks_t callbacks) {
    window->callbacks = callbacks;
}

/* misc platform functions */

static double get_native_time(void) {
    static double period = -1;
    if (period < 0) {
        mach_timebase_info_data_t info;
        mach_timebase_info(&info);
        period = (double)info.numer / (double)info.denom / 1e9;
    }
    return mach_absolute_time() * period;
}

float platform_get_time(void) {
    static double initial = -1;
    if (initial < 0) {
        initial = get_native_time();
    }
    return (float)(get_native_time() - initial);
}