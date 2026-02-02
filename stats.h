#pragma once

#include <string>

namespace stats {

struct Metrics {
    double currentFps = 0.0;
    double avgFps = 0.0;
    double minFrameMs = 0.0;
    double maxFrameMs = 0.0;
    double lastFrameMs = 0.0;
    unsigned long long frameCount = 0;
    int width = 0;
    int height = 0;
};

void init();
void set_resolution(int w, int h);
// Optional per-frame context for logs (e.g., page, motion flag).
void set_frame_context(int page, bool motion_present);
void frame_start();
void frame_end();
const Metrics& get();

// Draw small overlay with key metrics in the top-left corner.
void draw_overlay();

} // namespace stats
