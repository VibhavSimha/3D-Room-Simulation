#include <GL/glut.h>
#include <iostream>
#include <cstdlib>
#include <string>

#include "audio.h"
#include "bitmap.h"
#include "light.h"
#include "tooltip.h"
#include "checklist.h"
#include "stats.h"

// Initialize TooltipSystem
TooltipSystem tooltipSystem;
ChecklistSystem checklistSystem;
int mouseGlobalX = 0;
int mouseGlobalY = 0;

// Parameter and Logic Includes
// Note: These headers likely define global variables, so order matters.
#include "parameter.h"
#include "motion.h"
#include "objects.h"

/* TEXTURE HANDLING */
void loadTexture(GLuint texture, const char* filename) {
	BmpLoader image(filename);
	glBindTexture(GL_TEXTURE_2D, texture);

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image.iWidth, image.iHeight, GL_RGB, GL_UNSIGNED_BYTE, image.data);
}

void textureInit() {
	// Create Texture.
	textures = new GLuint[NUM_TEXTURE];
	glGenTextures(NUM_TEXTURE, textures);

	// Load the Texture.
	for (int i = 0; i < NUM_TEXTURE; i++)
		loadTexture(textures[i], texPath[i]);
}

/* RENDERING HANDLING */
void change_size(int w, int h) {
	// Update global parameters
	width = w;
	hight = h;
	stats::set_resolution(w, h);
	// Do reshape
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	float ratio = 0.0f;
	h = h == 0 ? 1 : h;
	ratio = (float)w / (float)h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(80.0f, ratio, 0.7f, 100.0f);
	glMatrixMode(GL_MODELVIEW);
}

void renderScene()
{
	stats::frame_start();
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
	gluLookAt(x, 5.0f, z,
		x + lx, y, z + lz,
		0.0f, 1.0f, 0.0f);

	// 3D audio listener follows the camera.
	audio::update_listener(
		{(float)x, 5.0f, (float)z},
		{(float)lx, (float)(y - 5.0), (float)lz},
		{0.0f, 1.0f, 0.0f}
	);
	
	// App Logic - Tooltips
	std::string forcedComponent = "";
	if (enterPressed) {
		if (objIndex == REMOVE_FAN || objIndex == REMOVE_FAN - 1) forcedComponent = "CPU Cooling Unit";
		else if (objIndex == REMOVE_RAM_STICK || objIndex == REMOVE_RAM_STICK - 1) forcedComponent = "DDR4 RAM";
		else if (objIndex == REMOVE_PROCESSOR || objIndex == REMOVE_PROCESSOR - 1) forcedComponent = "Processor";
		else if (objIndex == REMOVE_PSU || objIndex == REMOVE_PSU - 1) forcedComponent = "Power Supply";
		else if (objIndex == REMOVE_HDD || objIndex == REMOVE_HDD - 1) forcedComponent = "Hard Disk";
		else if (objIndex == REMOVE_GPU || objIndex == REMOVE_GPU - 1) forcedComponent = "NVIDIA GTX Graphics";
		else if (objIndex == REMOVE_MOTHERBOARD || objIndex == REMOVE_MOTHERBOARD - 1) forcedComponent = "Motherboard";
	}
	tooltipSystem.update(mouseGlobalX, mouseGlobalY, forcedComponent);

	// Save simple context for performance logs.
	stats::set_frame_context(page, motion_present);

	if (page == 1) {
		drawGround();
		drawCube();
		cpuView();
		drawCPU();

		// Update dynamic positions and visibility based on offset
		point3D gpuOff = gpu_.getOffset();
		tooltipSystem.updateComponent("NVIDIA GTX Graphics", 7.55f + gpuOff.x,
									4.2f + gpuOff.y, -4.65f + gpuOff.z, gpuOff.x);

		point3D fanOff = fan_.getOffset();
		tooltipSystem.updateComponent("CPU Cooling Unit", 7.746f + fanOff.x,
									4.841f + fanOff.y, -4.566f + fanOff.z,
									fanOff.x);

		point3D psuOff = psu_.getOffset();
		tooltipSystem.updateComponent("Power Supply", 8.0f + psuOff.x,
									3.4f + psuOff.y, -4.79f + psuOff.z, psuOff.x);

		point3D hddOff = harddisk_.getOffset();
		tooltipSystem.updateComponent("Hard Disk", 8.0f + hddOff.x,
									3.86f + hddOff.y, -3.2f + hddOff.z, hddOff.x);

		point3D chipOff = chipset_.getOffset();
		tooltipSystem.updateComponent("Processor", 8.0f + chipOff.x,
									4.77f + chipOff.y, -4.7f + chipOff.z,
									chipOff.x);
									
		point3D ramOff = ram_.getOffset();
		tooltipSystem.updateComponent("DDR4 RAM", 8.0f + ramOff.x,
									4.85f + ramOff.y, -4.25f + ramOff.z, ramOff.x);

		point3D mbOff = motherboard_.getOffset();
		tooltipSystem.updateComponent("Motherboard", 8.0f + mbOff.x,
									4.65f + mbOff.y, -4.6f + mbOff.z, mbOff.x);

		// Draw tooltips on top
		tooltipSystem.draw((float)x, 5.0f, (float)z);
		
		// Draw Checklist HUD
		checklistSystem.draw(objIndex, width, hight);
	}
	else if(page == 0) {
		front_page();
		progress_wheel();
	}
	stats::frame_end();
	stats::draw_overlay();
	glutSwapBuffers();
}

void opengl_init(void) {
	glEnable(GL_DEPTH_TEST);
	stats::init();
	// Optional 3D audio (enabled when built with USE_OPENAL).
	if (audio::init()) {
		audio::preload_defaults();
		std::atexit(audio::shutdown);
	}
	
	// Register AR Tooltips
	tooltipSystem.registerComponent("NVIDIA GTX Graphics", "High performance GPU",
                                  7.55f, 4.2f, -4.65f, 0.6f);
	tooltipSystem.registerComponent("CPU Cooling Unit", "Spinning at 2000 RPM",
                                  7.746f, 4.841f, -4.566f, 0.5f);
	tooltipSystem.registerComponent("DDR4 RAM", "16GB 3200MHz Memory", 8.0f, 4.85f, -4.25f, 0.4f);
	tooltipSystem.registerComponent("Motherboard", "Main Circuit Board", 8.0f, 4.65f, -4.6f, 0.7f, "CPU Cooling Unit");
	tooltipSystem.registerComponent("Processor", "Intel Core i7 CPU", 8.0f, 4.77f,
                                  -4.7f, 0.3f, "CPU Cooling Unit");
	tooltipSystem.registerComponent("Power Supply", "750W Gold Rated", 8.0f, 3.4f,
                                  -4.79f, 0.6f);
	tooltipSystem.registerComponent("Hard Disk", "2TB Mechanical Storage", 8.0f,
                                  3.86f, -3.2f, 0.5f);

	textureInit();
	glutDisplayFunc(renderScene);
	glutIdleFunc(renderScene);
	glutReshapeFunc(change_size);
	glutKeyboardFunc(processNormalKeys);
	glutSpecialFunc(processSpecialKeys);
	glutPassiveMotionFunc(mouse_follow); // Track mouse, from motion.h
	show_light_effect();
}

void setDeltaTime() {
	int timeSinceStart = glutGet(GLUT_ELAPSED_TIME);
	deltaTime = (timeSinceStart - oldTimeSinceStart) * 0.2f;
	oldTimeSinceStart = timeSinceStart;
}

int main(int argc, char ** argv) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(width, hight);
	glutCreateWindow("Graphical Simulation of Desktop & it's Components");
	opengl_init();
	glutFullScreen();
	setDeltaTime();
	glutMainLoop();
	audio::shutdown();
	getchar();
	return 0;
}
