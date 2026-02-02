#include "stats.h"

#include <GL/glut.h>
#include <chrono>
#include <cstdio>
#include <fstream>

namespace stats {

namespace {

Metrics g_metrics;
using clock_t = std::chrono::steady_clock;
clock_t::time_point g_startTime;
clock_t::time_point g_frameStart;

bool g_inited = false;

// Simple CSV logging of per-frame metrics for offline analysis.
std::ofstream g_csv;
bool g_csvReady = false;
int g_ctxPage = 0;
bool g_ctxMotion = false;

void drawText(float x, float y, const char* text) {
    glRasterPos2f(x, y);
    for (const char* c = text; *c != '\0'; ++c) {
        glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *c);
    }
}

} // namespace

void init() {
    if (g_inited) return;
    g_startTime = clock_t::now();
    g_inited = true;
}

void set_resolution(int w, int h) {
    g_metrics.width = w;
    g_metrics.height = h;
}

void set_frame_context(int page, bool motion_present) {
    g_ctxPage = page;
    g_ctxMotion = motion_present;
}

void frame_start() {
    if (!g_inited) init();
    g_frameStart = clock_t::now();
}

void frame_end() {
    auto now = clock_t::now();
    double frameMs = std::chrono::duration<double, std::milli>(now - g_frameStart).count();
    g_metrics.lastFrameMs = frameMs;
    g_metrics.frameCount++;

    if (g_metrics.frameCount == 1) {
        g_metrics.minFrameMs = frameMs;
        g_metrics.maxFrameMs = frameMs;
    } else {
        if (frameMs < g_metrics.minFrameMs) g_metrics.minFrameMs = frameMs;
        if (frameMs > g_metrics.maxFrameMs) g_metrics.maxFrameMs = frameMs;
    }

    double secondsSinceStart = std::chrono::duration<double>(now - g_startTime).count();
    if (secondsSinceStart > 0.0) {
        g_metrics.currentFps = 1000.0 / (frameMs > 0.0 ? frameMs : 1.0);
        g_metrics.avgFps = static_cast<double>(g_metrics.frameCount) / secondsSinceStart;
    }

    // Lazy-open CSV on first frame and write header.
    if (!g_csvReady) {
        g_csv.open("performance_log.csv", std::ios::out | std::ios::trunc);
        if (g_csv.is_open()) {
            g_csv << "frame,time_s,frame_ms,fps,avg_fps,min_ms,max_ms,width,height,pixels_per_frame,pixels_per_second,page,motion_present" << '\n';
            g_csvReady = true;
        }
    }

    if (g_csvReady) {
        double pixelsPerFrame = static_cast<double>(g_metrics.width) * static_cast<double>(g_metrics.height);
        double pixelsPerSecond = pixelsPerFrame * g_metrics.currentFps;
        g_csv
            << g_metrics.frameCount << ','
            << secondsSinceStart << ','
            << g_metrics.lastFrameMs << ','
            << g_metrics.currentFps << ','
            << g_metrics.avgFps << ','
            << g_metrics.minFrameMs << ','
            << g_metrics.maxFrameMs << ','
            << g_metrics.width << ','
            << g_metrics.height << ','
            << pixelsPerFrame << ','
            << pixelsPerSecond << ','
            << g_ctxPage << ','
            << (g_ctxMotion ? 1 : 0)
            << '\n';
    }
}

const Metrics& get() {
    return g_metrics;
}

void draw_overlay() {
    // Save state
    glPushAttrib(GL_ENABLE_BIT | GL_COLOR_BUFFER_BIT);
    glDisable(GL_LIGHTING);
    glDisable(GL_TEXTURE_2D);

    glMatrixMode(GL_PROJECTION);
    glPushMatrix();
    glLoadIdentity();
    gluOrtho2D(0.0, g_metrics.width ? g_metrics.width : 800.0, 0.0, g_metrics.height ? g_metrics.height : 600.0);

    glMatrixMode(GL_MODELVIEW);
    glPushMatrix();
    glLoadIdentity();

    glColor3f(1.0f, 1.0f, 0.0f);

    char buffer[256];
    std::snprintf(buffer, sizeof(buffer), "FPS: %.1f (avg %.1f)", g_metrics.currentFps, g_metrics.avgFps);
    drawText(10.0f, (g_metrics.height ? g_metrics.height : 600.0f) - 20.0f, buffer);

    std::snprintf(buffer, sizeof(buffer), "Frame ms: %.2f (min %.2f / max %.2f)",
                  g_metrics.lastFrameMs, g_metrics.minFrameMs, g_metrics.maxFrameMs);
    drawText(10.0f, (g_metrics.height ? g_metrics.height : 600.0f) - 40.0f, buffer);

    double pixelsPerFrame = static_cast<double>(g_metrics.width) * static_cast<double>(g_metrics.height);
    double pixelsPerSecond = pixelsPerFrame * g_metrics.currentFps;
    std::snprintf(buffer, sizeof(buffer), "Resolution: %dx%d, Throughput: %.0f px/s",
                  g_metrics.width, g_metrics.height, pixelsPerSecond);
    drawText(10.0f, (g_metrics.height ? g_metrics.height : 600.0f) - 60.0f, buffer);

    // Restore matrices
    glPopMatrix(); // modelview
    glMatrixMode(GL_PROJECTION);
    glPopMatrix();
    glMatrixMode(GL_MODELVIEW);

    glPopAttrib();
}

} // namespace stats
