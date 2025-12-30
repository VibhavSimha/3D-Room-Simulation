#pragma once

#include <string>

namespace audio {

struct Vec3 {
	float x;
	float y;
	float z;
};

// When USE_OPENAL is not defined (default), all functions become no-ops
// so the project builds without extra dependencies.

bool init();
void shutdown();

// Preload commonly used sfx (safe to call even if init() failed).
void preload_defaults();

// Update 3D listener (camera) each frame.
void update_listener(Vec3 position, Vec3 forward, Vec3 up);

// Play a one-shot sound at a world position.
void play3d(const std::string& soundPath, Vec3 position, float gain = 1.0f);

// Convenience: play at listener position (non-spatial UI click).
void play_ui(const std::string& soundPath, float gain = 1.0f);

} // namespace audio
