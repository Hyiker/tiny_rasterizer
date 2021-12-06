#ifndef VIEWER_H
#define VIEWER_H

#include <cmath>
#include <iostream>

#include "graphics/platform.h"
#include "rasterizer/Scene.hpp"
#include "rasterizer/scenes/SceneManager.hpp"
#include "utils/Measure.hpp"
class Viewer {
   public:
    Rasterizer::Scene* scene;
    std::string name;
    Viewer() { platform_initialize(); }
    void load(const std::string& scene_name) {
        name = scene_name;

        scene = Rasterizer::SceneManager::getPredefinedSceneByName(scene_name);
    }
    void show() {
        window_t* wd = window_create(name.c_str(), 800, 800);
        scene->render();
        callbacks_t wd_cb = {0};
        wd_cb.sc = 1;
        wd_cb.kc = 1;
        wd_cb.bc = 1;
        wd_cb.scroll_callback = [&](window_t* wd, float offset) {
            Rasterizer::Vec3 fwd =
                (scene->cam.center - scene->cam.eye_pos).normalized();
            scene->cam.eye_pos = scene->cam.eye_pos + fwd * offset * 0.10f;
            scene->clear();
        };
        wd_cb.button_callback = [&](window_t* wd, button_t btn, int pressed) {
            scene->clear();
        };
        wd_cb.key_callback = [&](window_t* wd, keycode_t key, int pressed) {
            Rasterizer::Mat4 rotation(Rasterizer::Mat4::identity());
            float a = DEG2RAD(7);
            switch (key) {
                case KEY_D:
                    a = -a;
                case KEY_A:
                    rotation << std::cos(a), 0, std::sin(a), 0, 0, 1, 0, 0,
                        -std::sin(a), 0, std::cos(a), 0, 0, 0, 0, 1;
                    break;
                case KEY_W:
                    a = -a;
                case KEY_S:
                    rotation << 1, 0, 0, 0, 0, std::cos(a), -std::sin(a), 0, 0,
                        std::sin(a), std::cos(a), 0, 0, 0, 0, 1;
                    break;
            }
            scene->models[0]->transform =
                rotation * scene->models[0]->transform;
            scene->clear();
        };
        input_set_callbacks(wd, wd_cb);
        while (!window_should_close(wd)) {
            input_poll_events();
            int64_t duration = measure([&]() {
                scene->render();
                window_draw_buffer(wd, scene);
            });
            std::cout << "\rrender takes " << duration
                      << "ms, estimated fps: " << 1000.f / float(duration)
                      << std::flush;
        }
    }
    ~Viewer() { platform_terminate(); }
};

#endif /* VIEWER_H */
